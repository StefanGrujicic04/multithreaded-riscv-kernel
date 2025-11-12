//
// Created by stefan on 4.10.25..
//

#ifndef OS1_CONSOLECONTROLLER_H
#define OS1_CONSOLECONTROLLER_H

#include "ConsoleBuffer.hpp"
#include "../lib/hw.h"
#include "riscv.hpp"

class ConsoleController {
public:

    static void consoleHandler();

    static void init();
    static void deinit();

    static void putInOutputBuffer(char c);
    static bool canGetFromOutputBuffer();
    static char getFromOutputBuffer();
    static void putInInputBuffer(char c);
    static bool canGetFromInputBuffer();
    static char getFromInputBuffer();

    static void putcworker();
    static void getcworker();


    static ConsoleBuffer*  inputBuffer ; //for getc
    static  ConsoleBuffer*  outputBuffer; //for putc

};


#endif //OS1_CONSOLECONTROLLER_H