#include "../h/riscv.hpp"
#include "../h/mem.hpp"
#include "../h/TCB.hpp"
#include "../h/_semaphore.hpp"
#include "../h/ConsoleController.hpp"
#include "../h/syscall_c.hpp"
#include "../h/print.hpp"

void Riscv::popSppSpie()
{
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}
void Riscv::supervisorTrapHandler() {
    uint64 volatile scause = r_scause();
    uint64 volatile sstatus = r_sstatus();
    uint64 volatile a0 = r_a0();
    uint64 volatile a1 = r_a1();
    uint64 volatile a2 = r_a2();
    uint64 volatile a3 = r_a3();
    uint64 volatile a4 = r_a4();
    uint64 volatile sepc = r_sepc();
    if (scause ==0x0000000000000008UL || scause == 0x0000000000000009UL) {
        sepc = r_sepc() + 4;
        w_sepc(sepc);
        if (a0==0x01) {//mem alloc
            uint64 memory=(uint64)MemoryAllocator::_mem_allocate(a1);
            asm volatile("sd %0, 10* 8(s0)"::"r"(memory));
        }
        else if (a0==0x02) {//mem free
            void * ptr=(void*) a1;
            uint64 status =-1;
            if (ptr) {
                status=(uint64)MemoryAllocator::_mem_free(ptr);
            }
            asm volatile("sd %0, 10* 8(s0)"::"r"(status));
        }
        else if (a0==0x03) {//rest of the memory
            uint64 freeMem=0;
            freeMem=MemoryAllocator::mem_get_free_space();
            asm volatile("sd %0, 10* 8(s0)"::"r"(freeMem));
        }
        else if (a0==0x04) {//largest block
            uint64 largestFreeBlock=0;
            largestFreeBlock=MemoryAllocator::mem_get_largest_free_block();
            asm volatile("sd %0, 10* 8(s0)"::"r"(largestFreeBlock));
        }
        else if (a0==0x11) {//thread_create
            TCB** thread= (TCB**)a1;
            *thread=TCB::createThread((TCB::Body)a2,(void *)a3, (uint64*) a4);
            if (*thread)
                asm volatile("sd %0,10* 8 (s0)"::"r"(0));
            else
                asm volatile("sd %0,10* 8 (s0)"::"r"(-2));//error for creatign thread
        }
        else if (a0==0x12) {//tread_exit
            TCB::running->setFinished(true);
            TCB::yield();
        }
        else if (a0==0x13) {//dispatch

            TCB::yield();
            asm volatile("sd %0,10* 8 (s0)"::"r"(0));

        }
        else if (a0==0x21) {//sem_open
            _semaphore **s = (_semaphore**)a1;
            *s = new _semaphore(a2);
            if(*s!= nullptr)
                asm volatile("sd %0, 10 * 8(s0) ": : "r"(0));
            else
                asm volatile("sd %0, 10 * 8(s0) ": : "r"(-3)); //error if didnt create semaphore

        }
        else if (a0==0x22) {//sem_close
            sem_t handle = (sem_t)a1; //sem_t is pointer to semaphore
            if(handle) {
                delete handle;
                asm volatile("sd %0, 10 * 8(s0) ": : "r"(0));
            }
            else {
                asm volatile("sd %0, 10 * 8(s0) ": : "r"(-4)); //error trying to delete nullptr
            }
        }else if (a0==0x23) {//sem_wait

            sem_t handle = (sem_t)a1;
            int status = -5; // error in semaphore wait
            if(handle)
                status = handle->wait();
            asm volatile("sd %0, 10 * 8(s0) ": : "r"(status));

        }else if (a0==0x24) {//sem_signal

            sem_t handle = (sem_t)a1;
            int status = -6; //error signal for semaphore signal
            if(handle)
                status = handle->signal();
            asm volatile("sd %0, 10 * 8(s0) ": : "r"(status));

        }else if (a0==0x25) {// timedwait ,this is not neccesary for this f=porject but it's easy to implement anyways
            sem_t handle = (sem_t)a1;
            time_t time = (time_t)a2;
            int status = -7; //error for sem timed wait
            if(handle)
                status = handle->timedwait(time);
            asm volatile("sd %0, 10 * 8(s0) ": : "r"(status));
        }else if (a0==0x26) {
            //sem_trywait ,also not neccesary
            sem_t handle = (sem_t)a1;
            int status = -8; //code error for trywait
            if(handle)
                status = handle->trywait();
            asm volatile("sd %0, 10 * 8(s0) ": : "r"(status));

        }else if (a0==0x31) {//time_sleep
            time_t time = (time_t)a1;
            if(time==0)
                goto ret;
            TCB::sleep(time);
            asm volatile("sd %0, 10 * 8(s0) ": : "r"(0));

        }else if (a0==0x41) {//getc
            /*
            ConsoleController::getcworker();
            plic_complete((int)CONSOLE_IRQ);
            char c = ConsoleController::getFromInputBuffer();
            asm volatile("sd %0, 10 * 8(s0) ": : "r"(c));
            */
            ConsoleController::getcworker();
            while (!ConsoleController::canGetFromInputBuffer()) {
                ConsoleController::getcworker(); // možda je stiglo nešto između
                TCB::yield();                    // pusti druge niti / čekaj na prekid
            }
            char c = ConsoleController::getFromInputBuffer();
            asm volatile("sd %0, 10 * 8(s0)" :: "r"(c));
        }else if (a0==0x42) {//putc
            ConsoleController::putInOutputBuffer((char)a1);
            ConsoleController::putcworker();
            plic_complete((int)CONSOLE_IRQ);
        }
        ret:   w_sstatus(sstatus);
        return;
    }
    else if (scause == 0x8000000000000001UL) {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
        mc_sip(SIP_SSIP);
        sstatus = r_sstatus();
        TCB::timeSliceCounter++;
        if (TCB::timeBlockedThreadHead) {
            while (TCB::timeBlockedThreadHead&& (TCB::timeBlockedThreadHead->timeBlocked == 0 || --TCB::timeBlockedThreadHead->timeBlocked == 0)) {
                TCB *toUnblock = TCB::timeBlockedThreadHead;
                toUnblock->timeBlocked = 0;
                TCB::timeBlockedThreadHead = TCB::timeBlockedThreadHead->next;
                if (TCB::timeBlockedThreadHead)
                    TCB::timeBlockedThreadHead->prev = nullptr;
                toUnblock->next = nullptr;
                toUnblock->sleeping = false;
                if (toUnblock->waitingon) {
                    //tounblock->waitingon->print_blocked(); //za modifikaciju ako bude bila ovo
                    if(!toUnblock->prevBlocked) {// leave in good place prev
                        toUnblock->waitingon->blockedTreadHead = toUnblock->nextBlocked;
                        if(!toUnblock->waitingon->blockedTreadHead)
                            toUnblock->waitingon->blockedTreadTail = nullptr;
                        else
                            toUnblock->waitingon->blockedTreadHead->prevBlocked= nullptr;
                    }
                    else if(!toUnblock->nextBlocked){ // leave in good place next
                        toUnblock->prevBlocked->nextBlocked = nullptr;
                        toUnblock->waitingon->blockedTreadTail= toUnblock->prevBlocked;
                    }
                    else{
                        toUnblock->prevBlocked->nextBlocked = toUnblock->nextBlocked;
                        toUnblock->nextBlocked->prevBlocked = toUnblock->prevBlocked;
                    }
                    toUnblock->nextBlocked = nullptr;
                    toUnblock->prevBlocked = nullptr;
                    toUnblock->waitstatus = -2;
                    toUnblock->waitingon->amount+=1;

                    toUnblock->waitingon = nullptr;
                }
                Scheduler::put(toUnblock);//put on scheduler
            }
        }

        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            TCB::timeSliceCounter = 0;
            TCB::yield();
        }
    }
    else if (scause == 0x8000000000000009UL)
    {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        ConsoleController::consoleHandler();
        return;
    }
    else
    {
                                                                                                                                    if (scause==1){printStr("TEST 5 (zadatak 4., thread_sleep test C API)\n"); HALT();return;}
        switch (scause) {
            case 2:
                printStr("\nIllegal instruction!\n");
                break;
            case 5:
                printStr("Unauthorized adress reading!\n");
                break;
            case 7:
                printStr("Unauhorized adress writing!\n");
                break;
            default:
                break;
        }
        printStr("Scause je: ");
        printInteger(scause);
        printStr("  sepc je:");
        printInteger(sepc);
        printStr("\n");
        Riscv::HALT();
    }

    w_sstatus(sstatus);
}
