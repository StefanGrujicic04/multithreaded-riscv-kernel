//
// Created by stefan on 22.9.25..

#include "../lib/hw.h"
#include "../h/mem.hpp"
#include "../h/riscv.hpp"
#include "../h/TCB.hpp"
#include "../h/ConsoleController.hpp"
#include "../h/syscall_c.hpp"
/*
void afk(void* arg){
	while(1){
		thread_dispatch();
	}
}
extern void userMain();

void userMainWrapper(void* arg){
	userMain();
}

int main() {

	Riscv::w_stvec((uint64)&Riscv::supervisorTrap);

	//allow SSIP and SEIP in S-mode
	__asm__ volatile ("csrs sie, %0" :: "r"(Riscv::SIP_SSIP));
	__asm__ volatile ("csrs sie, %0" :: "r"(Riscv::SIP_SEIP));

	MemoryAllocator::initialize();
	ConsoleController::init();

	TCB *mainThread;
	thread_create(&mainThread, nullptr, nullptr);
	TCB::running = mainThread;

	Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
	//TCB* idle;
	//thread_create(&idle,afk, nullptr);
	//userMain();

	TCB* usermain;
	thread_create(&usermain,userMainWrapper, nullptr);
	while(!usermain->isFinished()){
		thread_dispatch();
	}


	ConsoleController::deinit();
	Riscv::mc_sstatus(Riscv::SSTATUS_SIE);
	//idle->setFinished(true);
	MemoryAllocator::_mem_free(mainThread);


	Riscv::HALT();
	return 0;
}
*/

void afk(void* arg){
	while(1){
		thread_dispatch();
	}
}
extern void userMain();

void userMainWrapper(void* arg){
	userMain();
}

int main() {

	Riscv::w_stvec((uint64)&Riscv::supervisorTrap);
	MemoryAllocator::initialize();
	ConsoleController::init();

	TCB *mainT;
	thread_create(&mainT, nullptr, nullptr);
	TCB::running = mainT;

	Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
	TCB* idle;
	thread_create(&idle,afk, nullptr);
	//userMain();

	TCB* usermain;
	thread_create(&usermain,userMainWrapper, nullptr);
	while(!usermain->isFinished()){
		thread_dispatch();
	}


	ConsoleController::deinit();
	Riscv::mc_sstatus(Riscv::SSTATUS_SIE);
	//idle->setFinished(true);
	MemoryAllocator::_mem_free(mainT);


	Riscv::HALT();
	return 0;
}