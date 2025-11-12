//
// Created by stefan on 26.9.25..
//

#ifndef OS1_TCB_H
#define OS1_TCB_H
#include "../lib/hw.h"
#include "../h/scheduler.hpp"
#include "../h/riscv.hpp"
#include "mem.hpp"
#include "../h/_semaphore.hpp"
class _semaphore;


class TCB
{
public:

    bool isFinished() const { return finished; }

    void setFinished(const bool value) { finished = value; }

    uint64 getTimeSlice() const { return timeSlice; }

    using Body = void (*)(void *);

    static TCB *createThread(Body body, void * arg,uint64* stack);

    static void yield(){
        uint64 volatile sepc = Riscv::r_sepc();
        uint64 volatile sstatus = Riscv::r_sstatus();

        TCB::dispatch();

        Riscv::w_sstatus(sstatus);
        Riscv::w_sepc(sepc);
    }

    static TCB* volatile running;

    static int sleep(time_t time);

    static uint64 constexpr STACK_SIZE = 2048;

    void* operator new(size_t size) {
        return MemoryAllocator::_mem_allocate(MemoryAllocator::size_in_blocks(size));
    }
    void operator delete(void* p) {
        MemoryAllocator::_mem_free(p);
    }
    void* operator new[](size_t size) {
        return MemoryAllocator::_mem_allocate(MemoryAllocator::size_in_blocks(size));
    }
    void operator delete[](void* p) {
        MemoryAllocator::_mem_free(p);
    }
private:
    friend class Scheduler;
    friend class _semaphore;
    friend class Riscv;
    TCB(Body body, uint64 timeSlice,void *arg, uint64* stack);
    ~TCB() {
        MemoryAllocator::_mem_free(stack);
    }
    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    bool finished;
    void *arg;

    static void insert_in_sleeping(time_t time);
    static void threadWrapper();

    TCB* next = nullptr; //It is used for chaining in the scheduler and in sleep threads
    TCB* prev = nullptr; //Only for previus in the sleeping threads
    bool sleeping = false;
    time_t timeBlocked = 0;


    _semaphore* waitingon = nullptr;
    int waitstatus = 0;
    TCB* nextBlocked = nullptr; //Next in the blocked threads at the semaphores
    TCB* prevBlocked = nullptr; //Prev in the blocked threads at the semaphores

    static TCB* volatile timeBlockedThreadHead;

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();


    static uint64 timeSliceCounter;

    static uint64 constexpr TIME_SLICE = 2;
};

#endif //OS1_TCB_H