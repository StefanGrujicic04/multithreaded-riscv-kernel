//
// Created by stefan on 4.10.25..
//

#ifndef OS1_CONSOLEBUFFER_H
#define OS1_CONSOLEBUFFER_H
#include "../lib/hw.h"
#include "../h/_semaphore.hpp"

class ConsoleBuffer {
public:
    ConsoleBuffer();
    void push(char c);
    char pop();
    bool isfull()const {
        return sz==BUFFER_SIZE;
    }
    bool isempty()const {
        return sz==0;
    }
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
private:
    _semaphore* space_available,*item_available; //ne treba mutex jer ne moze u isto vreme da bude u prekidnoj rutini
    static constexpr uint64 BUFFER_SIZE = 256;
    char* buf;
    int head,tail;
    int sz = 0;
};


#endif //OS1_CONSOLEBUFFER_H