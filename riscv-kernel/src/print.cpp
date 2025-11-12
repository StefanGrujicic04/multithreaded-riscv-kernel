//
// Created by stefan on 25.9.25..
//
#include "../h/print.hpp"

uint64 lckPrint = 0;

#define LOCK() while(copyAndSwap(lckPrint, 0, 1)) thread_dispatch()
#define UNLOCK() while(copyAndSwap(lckPrint, 1, 0))

void printStr(char const *string)
{
    LOCK();
    while (*string != '\0')
    {
        putc(*string);
        string++;
    }
    UNLOCK();
}

char* getStr(char *buf, int max) {
    LOCK();
    int i, cc;
    char c;

    for(i=0; i+1 < max; ){
        cc = getc();
        if(cc < 1)
            break;
        c = cc;
        buf[i++] = c;
        if(c == '\n' || c == '\r')
            break;
    }
    buf[i] = '\0';

    UNLOCK();
    return buf;
}

int strToInt(const char *s) {
    int n;

    n = 0;
    while ('0' <= *s && *s <= '9')
        n = n * 10 + *s++ - '0';
    return n;
}

char dgts[] = "0123456789ABCDEF";

void printInteger(int xx, int base, int sgn)
{
    LOCK();
    char buf[16];
    int i, neg;
    uint x;

    neg = 0;
    if(sgn && xx < 0){
        neg = 1;
        x = -xx;
    } else {
        x = xx;
    }

    i = 0;
    do{
        buf[i++] = dgts[x % base];
    }while((x /= base) != 0);
    if(neg)
        buf[i++] = '-';

    while(--i >= 0)
        putc(buf[i]);

    UNLOCK();
}