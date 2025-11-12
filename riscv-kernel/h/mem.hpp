//
// Created by stefan on 24.9.25..
//

#ifndef OS1_MEM_H
#define OS1_MEM_H
#include "../lib/hw.h"
struct MemSegment {
    MemSegment* next;
    MemSegment* prev;
    size_t size;
};
class MemoryAllocator {
public:

    static void* _mem_allocate( size_t size);

    static int _mem_free(void* ptr);

    static void initialize();

    static uint32 mem_get_free_space(){return totalSize;}

    static size_t mem_get_largest_free_block();

    static size_t size_in_blocks(size_t size);

private:
    static MemSegment* segmentsHead;
    static uint32 segmentsNumber;
    static uint32 totalSize;
};


#endif //OS1_MEM_H