# multithreaded-riscv-kernel
Minimal C++ kernel for the RISC-V architecture with basic memory management, interrupts, and task scheduling.

# RISC-V Kernel (C++)

An educational **kernel for the RISC-V architecture** implemented in **C++**.  
Built as part of an Operating Systems course at the University of Belgrade (ETF).  
Implements low-level system components including memory management, interrupt handling, and task scheduling.

---

##  Features
- **Memory Management**: Basic paging and allocation mechanisms.
- **Interrupt Handling**: Custom interrupt table and dispatcher.
- **Task Scheduling**: Cooperative and preemptive scheduler.
- **Context Switching**: User/system mode transitions simulated in RISC-V.
- **Simulation**: Built and tested on a RISC-V emulator (spike / QEMU).
