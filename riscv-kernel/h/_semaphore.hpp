//
// Created by stefan on 4.10.25..
//

#ifndef OS1__SEMAPHORE_H
#define OS1__SEMAPHORE_H
#include"../h/TCB.hpp"
#include "../h/riscv.hpp"
class _semaphore {
public:
    enum SEMCONST{SEMDEAD=-1,TIMEOUT=-2};
    int wait();
    int signal();
    int timedwait(time_t time);
    int trywait();
    int value() const {return amount;}
    _semaphore(int init): amount(init){}
    ~_semaphore();


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
protected:
    void block();
    void blocktimed(time_t time);
    void unblock(int status);
private:
    static void unlinkFromSemaphoreBlockedList(_semaphore* sem, TCB* tcb);
    static void unlinkFromSleepDeltaList(TCB* volatile & head, TCB* tcb);
    friend class Riscv;
    TCB* volatile blockedTreadHead=nullptr;
    TCB* volatile blockedTreadTail=nullptr;
    void insert_in_blocked();
    int amount;
};


#endif //OS1__SEMAPHORE_H