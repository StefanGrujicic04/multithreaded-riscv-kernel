//
// Created by stefan on 4.10.25..
//

#include "../h/_semaphore.hpp"
int _semaphore::wait() {

    if(--amount<0) {
        block();
    }
    return TCB::running->waitstatus;
}
int _semaphore::timedwait(time_t time) {
    if(--amount<0) {
        blocktimed(time);
    }
    return TCB::running->waitstatus;
}
int _semaphore::signal() {
    if(++amount<=0)
        unblock(0);


    return 0;

}

int _semaphore::trywait() {
    if(amount<=0) {
        return 1;
    }
    --amount;
    return 0;
}
void _semaphore::block() {
    insert_in_blocked();
    //print_blocked();
    TCB::yield();
}
void _semaphore::blocktimed(time_t time) {
    //ubacivanje u listu blokiranih na semaforu
    insert_in_blocked();
    TCB::insert_in_sleeping(time);
    TCB::yield();

}
void _semaphore::unblock(int status) {
    TCB* tounblock = blockedTreadHead;
    blockedTreadHead=blockedTreadHead->nextBlocked;
    if(blockedTreadHead)
        blockedTreadHead->prevBlocked = nullptr;
    else
        blockedTreadTail = nullptr;
    tounblock->waitingon = nullptr;
    tounblock->waitstatus = status;
    tounblock->nextBlocked = nullptr;
    tounblock->prevBlocked = nullptr;

    if(tounblock->sleeping){
        if(!tounblock->prev) {
            TCB::timeBlockedThreadHead = tounblock->next;
            if(TCB::timeBlockedThreadHead) {
                TCB::timeBlockedThreadHead->timeBlocked += tounblock->timeBlocked;
                TCB::timeBlockedThreadHead->prev = nullptr;
            }
        }
        else if(!tounblock->next){
            tounblock->prev->next = nullptr;
        }
        else{
            tounblock->prev->next = tounblock->next;
            tounblock->next->prev = tounblock->prev;
            tounblock->next->timeBlocked+=tounblock->timeBlocked;
        }
        tounblock->next = nullptr;
        tounblock->prev = nullptr;
        tounblock->sleeping = false;
    }
    Scheduler::put(tounblock);
}
/*
void _semaphore::unblock(int status) {

    if (!blockedTreadHead) return; //no one to unblock

    TCB* t = blockedTreadHead;

    // 1) take from semaphore list
    unlinkFromSemaphoreBlockedList(this, t);

    // 2) take from sleepy list
    unlinkFromSleepDeltaList(TCB::timeBlockedThreadHead, t);

    // 3) write status
    t->waitstatus = status;

    // 4) push in ready
    Scheduler::put(t);
}
 inline void _semaphore::unlinkFromSemaphoreBlockedList(_semaphore* sem, TCB* tcb) {
    //  tcb == sem->blockedThreadsHead will always be true
    sem->blockedTreadHead = sem->blockedTreadHead->nextBlocked;
    if (sem->blockedTreadHead) sem->blockedTreadHead->prevBlocked = nullptr;
    else                         sem->blockedTreadTail = nullptr;

    tcb->nextBlocked = tcb->prevBlocked = nullptr;
    tcb->waitingon   = nullptr;
}

inline void _semaphore::unlinkFromSleepDeltaList(TCB* volatile & head, TCB* tcb) {
    if (!tcb->sleeping) return;

    if (!tcb->prev) {
        // tcb is head
        head = tcb->next;
        if (head) {
            head->timeBlocked += tcb->timeBlocked;
            head->prev = nullptr;
        }
    } else if (!tcb->next) {
        // tcb is tail
        tcb->prev->next = nullptr;
    } else {
        // tcb middle
        tcb->prev->next = tcb->next;
        tcb->next->prev = tcb->prev;
        tcb->next->timeBlocked += tcb->timeBlocked;
    }

    tcb->next = tcb->prev = nullptr;
    tcb->sleeping = false;
}
*/
void _semaphore::insert_in_blocked(){
    TCB::running->waitingon = this;
    if(blockedTreadHead) {
        blockedTreadTail->nextBlocked = TCB::running;
        TCB::running->prevBlocked = blockedTreadTail;
        blockedTreadTail = TCB::running;
    }
    else {
        blockedTreadHead = blockedTreadTail = TCB::running;
    }
}

_semaphore::~_semaphore() {
    while(blockedTreadHead) {
        unblock(-1);
    }
}