//
// Created by stefan on 26.9.25..
//
#include "../h/TCB.hpp"
#include"../h/syscall_c.hpp"
TCB * volatile TCB::running = nullptr;
TCB* volatile TCB::timeBlockedThreadHead = nullptr;
uint64 TCB::timeSliceCounter = 0;

TCB *TCB::createThread(TCB::Body body, void *arg,uint64* stack)
{
    return new TCB(body, TIME_SLICE,arg,stack);
}

TCB::TCB(TCB::Body body, uint64 timeSlice, void *arg, uint64* stack)  :
        body(body),
        stack(stack),
        context({(uint64) &threadWrapper,
                 body != nullptr ? (uint64) &stack[STACK_SIZE] : 0
                }),
        timeSlice(timeSlice),
        finished(false) ,
        arg(arg),
        next(nullptr)
{
    if (body!= nullptr) { Scheduler::put(this); }
}
void TCB::threadWrapper(){
    if(running->body)
        Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    else
        Riscv::ms_sstatus(Riscv::SSTATUS_SPP);
    Riscv::popSppSpie();
    running->body(running->arg);
    thread_exit();
}
void TCB::dispatch() {
    TCB* old = running;

    // 1) stavi staru nit nazad u ready SAMO ako stvarno treba da se planira
    //    (nije gotova, nije uspavana, nije na semaforu) I ima "body"
    if (old && !old->isFinished() && !old->sleeping && !old->waitingon && old->body) {
        Scheduler::put(old);
    }

    // 2) uzmi sledeću
    running = Scheduler::get();
    if (!running) {
        // fallback: ako nema spremnih niti, ostani na staroj
        running = old;
    }

    // 3) preklopi kontekste (čak i kad je old==running je ok; ret je brz)
    contextSwitch(&old->context, &running->context);

    // 4) POSLE preklopa: ako je stara nit završila i imala svoj stack, bezbedno je obriši
    if (old && old->isFinished() && old->body) {
        delete old;
    }
    /*
    TCB* old = running;

    if (!old->isFinished() && !old->sleeping && !old->waitingon) {
        Scheduler::put(old);
    }
    running = Scheduler::get();
    contextSwitch(&old->context, &running->context);

    if (old->isFinished()) {
        delete old;
    }*/
}

int TCB::sleep(time_t time) {
    insert_in_sleeping(time);
    yield();
    return 0;
}
void TCB::insert_in_sleeping(time_t time) {

    TCB* t = TCB::running;
    t->next = t->prev = nullptr;
    t->sleeping = true;

    if (!timeBlockedThreadHead) {
        t->timeBlocked = time;
        timeBlockedThreadHead = t;
        return;
    }

    TCB* prev = nullptr;
    TCB* curr = timeBlockedThreadHead;


    while (curr && time >= curr->timeBlocked) {
        time -= curr->timeBlocked;
        prev = curr;
        curr = curr->next;
    }

    t->timeBlocked = time;

    if (!prev) {
        t->next = timeBlockedThreadHead;
        timeBlockedThreadHead->prev = t;
        timeBlockedThreadHead = t;
    } else {
        t->next = curr;
        t->prev = prev;
        prev->next = t;
        if (curr) curr->prev = t;
    }

    if (curr) curr->timeBlocked -= time;

    /*
    TCB* old =running;
    old->next=nullptr;
    old->prev=nullptr;
    old->sleeping=true;
    if (!timeBlockedThreadHead) {
        timeBlockedThreadHead = old;
        old->timeBlocked = time;
        return;
    }
    TCB* prev=nullptr;
    TCB* curr=timeBlockedThreadHead;
    while (curr&& curr->timeBlocked <= old->timeBlocked) {
        old->timeBlocked-= curr->timeBlocked;
        prev=curr;
        curr=curr->next;
    }
    old->timeBlocked = time;
    if (!prev) {
        old->next=timeBlockedThreadHead;
        timeBlockedThreadHead->prev=old;
        timeBlockedThreadHead=old;
    }
    else if (!curr) {
        prev->next=old;
        old->prev=prev;
    }
    else {
        prev->next=old;
        curr->prev=old;
        old->next=curr;
        old->prev=prev;
    }
    if (curr) {
        curr->timeBlocked-=time;
    }
    old->timeBlocked=time;
    old->sleeping=true;

*/
}