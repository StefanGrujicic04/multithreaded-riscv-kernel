//
// Created by stefan on 4.10.25..
//

#include "../h/ConsoleController.hpp"
ConsoleBuffer*  ConsoleController::inputBuffer = nullptr;
ConsoleBuffer*  ConsoleController::outputBuffer = nullptr;
void ConsoleController::consoleHandler() {
    Riscv::mc_sip(Riscv::SIP_SEIP);
    uint64 volatile sstatus = Riscv::r_sstatus();
    int pclaim = plic_claim();

    if(pclaim == CONSOLE_IRQ){
        ConsoleController::getcworker();
        ConsoleController::putcworker();
        plic_complete((int)CONSOLE_IRQ);
    }
    else{
        plic_complete(pclaim);
    }
    Riscv::w_sstatus(sstatus);
}
void ConsoleController::init() {
    ConsoleController::inputBuffer = new ConsoleBuffer();
    ConsoleController::outputBuffer = new ConsoleBuffer();
}

void ConsoleController::deinit() {
    delete inputBuffer;
    delete outputBuffer;
}
void ConsoleController::putInOutputBuffer(char c) {
    outputBuffer->push(c);
}

bool ConsoleController::canGetFromOutputBuffer() {
    return !outputBuffer->isempty();
}

char ConsoleController::getFromOutputBuffer() { //Firstly you need to check if it has elements
    return outputBuffer->pop();
}

void ConsoleController::putInInputBuffer(char c) {
    inputBuffer->push(c);
}

bool ConsoleController::canGetFromInputBuffer() {
    return !inputBuffer->isempty();
}

char ConsoleController::getFromInputBuffer() { //Firstly you need to check if it has elements
    return inputBuffer->pop();
}
void ConsoleController::putcworker(){
    bool volatile putcready =(*((char*)CONSOLE_STATUS))& CONSOLE_TX_STATUS_BIT;
    bool volatile haselements = !outputBuffer->isempty();
    while(putcready and haselements){
        *((char*)CONSOLE_TX_DATA) = outputBuffer->pop();
        putcready =*((char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT;
        haselements = !outputBuffer->isempty();
    }


}

void ConsoleController::getcworker() {
    bool volatile getcready = *((char*)CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT;
    bool volatile notfull = !inputBuffer->isfull();
    while(getcready && notfull){
        inputBuffer->push(*((char*)CONSOLE_RX_DATA));
        getcready = *((char*)CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT;
        notfull = !inputBuffer->isfull();
    }
}