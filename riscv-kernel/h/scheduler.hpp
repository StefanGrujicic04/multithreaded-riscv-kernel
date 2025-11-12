//
// Created by stefan on 30.9.25..
//

#ifndef OS1_SCHEDULER_H
#define OS1_SCHEDULER_H
#include "List.hpp"

class TCB;

class Scheduler
{
private:
    static List<TCB> readyThreadQueue;

public:
    static TCB *get();

    static void put(TCB *ccb);

};


#endif //OS1_SCHEDULER_H