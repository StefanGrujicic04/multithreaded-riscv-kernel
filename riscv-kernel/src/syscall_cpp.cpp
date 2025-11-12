//
// Created by stefan on 30.9.25..
//

#include "../h/syscall_cpp.hpp"
void* operator new(size_t sz) {
    return mem_alloc(sz);
}

void operator delete(void* ptr) {
    mem_free(ptr);
}

void* operator new[](size_t sz) {
    return mem_alloc(sz);
}

void operator delete[](void* ptr) {
    mem_free(ptr);
}

Thread::Thread(void (*body)(void *), void *arg) :myHandle(nullptr),body(body),arg(arg) {
}

Thread::~Thread() {
}

int Thread::start() {
    if(body){
        return thread_create(&myHandle,body,arg);
    }
    return thread_create(&myHandle, wrapper, this);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t t) {
    return time_sleep(t);
}

Thread::Thread(): myHandle(nullptr),body(nullptr),arg(nullptr){
}

void Thread::wrapper(void *arg) {
    if(arg){
        ((Thread*)arg)->run();
    }
}

Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle,init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

int Semaphore::timedWait(time_t t) {
    return sem_timedwait(myHandle,t);
}

int Semaphore::tryWait() {
    return sem_trywait(myHandle);
}

void PeriodicThread::terminate() {
    period = 0;
}

PeriodicThread::PeriodicThread(time_t period) : period(period){

}


void PeriodicThread::run() {
    while(period != 0){
        periodicActivation();
        time_sleep(period);
    }
    thread_exit();
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    return ::putc(c);
}