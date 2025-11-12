//
// Created by stefan on 24.9.25..
//

#include "../h/mem.hpp"
MemSegment* MemoryAllocator::segmentsHead = nullptr;
uint32 MemoryAllocator::segmentsNumber = 0;
uint32 MemoryAllocator::totalSize = 0;
void* MemoryAllocator::_mem_allocate(const size_t need) {
    size_t size=need+1;
    if (segmentsNumber==0 || totalSize<size) {
        return nullptr;
    }
    MemSegment* temp= segmentsHead;
    while (temp && temp->size<size ) {
        temp = temp->next;
    }
    if (!temp)
        return nullptr;

    if (temp->size==size||temp->size-size==1) {
        if (temp->size-size==1) {
            size++;

        }
        totalSize -= size;
        if (!temp->prev && !temp->next) {//just one segment
            segmentsNumber--;
            segmentsHead=nullptr;
            *((size_t*)temp) = size-1; //this is size in segments stored in metadata
            return (void*)((char*)temp + MEM_BLOCK_SIZE);
        }

        if (temp->prev)
            temp->prev->next = temp->next;
        else
            segmentsHead=temp->next;

        if (temp->next) temp->next->prev = temp->prev;

        segmentsNumber--;
        *(size_t*)temp=size-1;
        return (void*)((char*)temp +MEM_BLOCK_SIZE);

    }
    // when tren->size > size
    totalSize -= size;
    const size_t offset=size*MEM_BLOCK_SIZE;
    if (temp->prev) temp->prev->next = (MemSegment* )((char *)temp+offset);
    if (temp->next) temp->next->prev = (MemSegment* )((char *)temp+offset);

    ((MemSegment* )((char* )temp+offset))->next=temp->next;
    ((MemSegment* )((char* )temp+offset))->prev=temp->prev;
    ((MemSegment* )((char* )temp+offset))->size=temp->size- size;

    if (temp==segmentsHead) segmentsHead=((MemSegment* )((char *)temp+offset));

    auto tempMeta = (size_t*)temp;
    *tempMeta = size-1; //this is size segments stored in metadata
    tempMeta = (size_t*)((char*)tempMeta + MEM_BLOCK_SIZE); //shift pointer to account for metadata
    return (void*)tempMeta;

}
int MemoryAllocator::_mem_free(void* ptr) {
    if (!ptr) return -1;

    // free node reconstriciton
    auto seg   = (MemSegment*)((uint64)ptr - MEM_BLOCK_SIZE);
    size_t size = *(size_t*)seg;
    size++;
    seg->size = size;
    seg->prev = seg->next = nullptr;
    totalSize += size;

    // list of free memory is empty
    if (!segmentsHead) {
        segmentsHead = seg;
        segmentsNumber = 1;
        return 0;
    }

    // finds seg place in list
    MemSegment* it = segmentsHead;
    MemSegment* prev = nullptr;
    while (it && it < seg) { prev = it; it = it->next; }

    // connect lists
    seg->prev = prev;
    seg->next = it;
    if (it) it->prev = seg;
    if (prev) prev->next = seg;
    else segmentsHead = seg;

    segmentsNumber++;  // join in list

    // 4) try merge RIGHT side
    if (seg->next) {
        uint64 segEnd = (uint64)seg + seg->size * MEM_BLOCK_SIZE;
        if ((void*)seg->next == (void*)segEnd) {
            seg->size += seg->next->size;
            seg->next = seg->next->next;
            if (seg->next) seg->next->prev = seg;
            segmentsNumber--;  // seg and it in one
        }
    }

    // try merge LEFT side
    if (seg->prev) {
        auto prevEnd =(MemSegment*) ((uint64)seg->prev + seg->prev->size * MEM_BLOCK_SIZE);
        if (seg == prevEnd) {
            seg->prev->size += seg->size;
            seg->prev->next = seg->next;
            if (seg->next) seg->next->prev = seg->prev;
            segmentsNumber--;  // prev and seg in one
        }
    }


    return 0;
}
void MemoryAllocator::initialize() {
    segmentsHead =(MemSegment*)(HEAP_START_ADDR);
    segmentsHead->prev=nullptr;
    segmentsHead->next=nullptr;
    segmentsHead->size= size_t( uint64(HEAP_END_ADDR) - uint64(HEAP_START_ADDR) ) /MEM_BLOCK_SIZE;
    totalSize=segmentsHead->size;
    segmentsNumber = 1;
}
size_t MemoryAllocator::mem_get_largest_free_block() {
    size_t size=0;
    MemSegment * it=segmentsHead;
    while (it) {
        size_t newSize=it->size;
        if (newSize>size) size = newSize;
        it = it->next;
    }
    if (size == 0) return 0;           // no free space
    if (size == 1) return 0;           // just enough for header
    return (size - 1) * MEM_BLOCK_SIZE;
}
size_t MemoryAllocator::size_in_blocks(size_t size) {
    size_t blocks = size/MEM_BLOCK_SIZE;
    if(size%MEM_BLOCK_SIZE!=0)
        blocks+=1;
    return blocks;
}
