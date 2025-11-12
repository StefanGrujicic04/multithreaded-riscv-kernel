//
// Created by stefan on 30.9.25..
//

#ifndef OS1_SYSCALL_CPP_H
#define OS1_SYSCALL_CPP_H
#include"syscall_c.hpp"


void* operator new (size_t);
void operator delete (void*);
void* operator new[] (size_t);
void operator delete[] (void*);


class Thread {
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t);
protected:
    Thread ();
    virtual void run () {}
private:
    thread_t myHandle;
    void (*body)(void*); void* arg;
    static void wrapper(void * arg);

};

class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
    int tryWait();
    int timedWait(size_t t);
private:
    sem_t myHandle;
};

class PeriodicThread : public Thread {
public:
    void terminate ();

protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation () {}
private:
    void run();
    time_t period;
};

class Console {
public:
    static char getc ();
    static void putc (char);
};



#endif //OS1_SYSCALL_CPP_H