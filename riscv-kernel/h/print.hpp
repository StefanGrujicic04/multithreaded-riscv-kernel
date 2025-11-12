//
// Created by stefan on 25.9.25..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_1_SYNCHRONOUS_PRINT_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_1_SYNCHRONOUS_PRINT_HPP

#include "../h/syscall_c.hpp"

typedef unsigned long uint64;

extern "C" uint64 copyAndSwap(uint64 &lock, uint64 expected, uint64 desired);

void printStr(char const *string);

char* getStr(char *buf, int max);

int strToInt(const char *s);

void printInteger(int xx, int base=10, int sgn=0);
#endif // OS1_VEZBE07_RISCV_CONTEXT_SWITCH_1_SYNCHRONOUS_PRINT_HPP