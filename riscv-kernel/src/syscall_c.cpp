//
// Created by stefan on 30.9.25..
//

#include "../h/syscall_c.hpp"
#include"../h/ConsoleController.hpp"
#include "../h/TCB.hpp"

void* mem_alloc(size_t size) {
    size_t blocks = size/MEM_BLOCK_SIZE;
    if(size%MEM_BLOCK_SIZE!=0)
        blocks+=1;
    return (void*) syscall(0x01,(uint64)blocks,0,0,0);
}
int mem_free(void* ptr) {
    return (int)syscall(0x02,(uint64)ptr,0,0,0);
}
size_t mem_get_free_space() {
    return syscall(0x03,0,0,0,0);
}
size_t mem_get_largest_free_block() {
    return syscall(0x04,0,0,0,0);
}

int thread_create(thread_t *handle,void(*start_routine)(void *),void *arg) {
    uint64* stack=nullptr;
    if (start_routine) {
        stack=new uint64[TCB::STACK_SIZE];
    }
    return (int)syscall(0x11,(uint64)handle,(uint64) start_routine,(uint64)arg,(uint64)stack);
}
int thread_exit() {
    return (int)syscall(0x12,(uint64)0,0,0,0);
}
void thread_dispatch() {
     syscall(0x13,0,0,0,0);
}
int sem_open(sem_t *handle, unsigned int init) {
    return (int)syscall(0x21,(uint64)handle,init,0,0);
}

int sem_close(sem_t handle) {
    return (int)syscall(0x22,(uint64)handle,0,0,0);
}

int sem_wait(sem_t id) {
    return (int)syscall(0x23,(uint64)id,0,0,0);

}

int sem_signal(sem_t id) {
    return (int)syscall(0x24,(uint64)id,0,0,0);
}

int sem_timedwait(sem_t id, time_t timeout) {
    return (int)syscall(0x25,(uint64)id,(uint64)timeout,0,0);
}

int sem_trywait(sem_t id) {
    return (int)syscall(0x26,(uint64)id,0,0,0);
}

int time_sleep(time_t t) {
    return (int)syscall(0x31,(uint64)t,0,0,0);
}
char getc() {
    /*
    bool volatile wait = true;
    while(wait){
        wait = !ConsoleController::canGetFromInputBuffer();
    }
    */
    char c = syscall(0x41,0,0,0,0);
    return c;
}

void putc(char c) {
    syscall(0x42,c,0,0,0);
}

uint64 syscall(uint64 noSystemCall, uint64 arg1, uint64 arg2, uint64 arg3, uint64 arg4) {

    asm volatile("ecall");
    uint64 ret;
    asm volatile("mv %0, a0" : "=r" (ret));// moves result in ret
    return ret;
}


