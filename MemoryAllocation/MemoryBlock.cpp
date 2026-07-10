#include "MemoryBlock.h"
#include <iostream>

MemoryBlock::MemoryBlock(int baseAddress, int size)
    : baseAddress(baseAddress), size(size), free(true), processId(-1) {}

int MemoryBlock::getBaseAddress() const { return baseAddress; }
int MemoryBlock::getSize() const { return size; }
bool MemoryBlock::isFree() const { return free; }
int MemoryBlock::getProcessId() const { return processId; }

void MemoryBlock::occupy(int pid) {
    free = false;
    processId = pid;
}

void MemoryBlock::release() {
    free = true;
    processId = -1;
}

void MemoryBlock::print() const {
    std::cout << "[Base: " << baseAddress
              << ", Size: " << size
              << ", " << (free ? "LIBRE" : "OCUPADO")
              << (free ? "" : (", PID: " + std::to_string(processId)))
              << "]" << std::endl;
}