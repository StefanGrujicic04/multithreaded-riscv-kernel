//
// Created by stefan on 4.10.25..
//

#include "../h/ConsoleBuffer.hpp"
ConsoleBuffer::ConsoleBuffer() {
    buf = new char[BUFFER_SIZE];
    space_available = new _semaphore(BUFFER_SIZE);
    item_available = new _semaphore(0);
    head = tail = 0;
}
void ConsoleBuffer::push(char c) {
    space_available->wait();
    buf[tail] = c;
    tail = (tail+1)%BUFFER_SIZE;
    sz++;
    item_available->signal();
}

char ConsoleBuffer::pop() {
    item_available->wait();
    char c = buf[head];
    head=(head+1)%BUFFER_SIZE;
    sz--;
    space_available->signal();
    return c;
}