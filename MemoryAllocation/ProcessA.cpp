#include "ProcessA.h"
#include <iostream>

ProcessA::ProcessA(int pid, int requestedSize)
    : pid(pid), requestedSize(requestedSize), allocated(false), assignedBlockBase(-1) {}

int ProcessA::getPid() const { return pid; }
int ProcessA::getRequestedSize() const { return requestedSize; }
bool ProcessA::isAllocated() const { return allocated; }
int ProcessA::getAssignedBlockBase() const { return assignedBlockBase; }

void ProcessA::assignTo(int blockBaseAddress) {
    allocated = true;
    assignedBlockBase = blockBaseAddress;
}

void ProcessA::deallocate() {
    allocated = false;
    assignedBlockBase = -1;
}

void ProcessA::print() const {
    std::cout << "Proceso PID: " << pid
              << ", Tamaño solicitado: " << requestedSize
              << ", " << (allocated ? "ASIGNADO en base " + std::to_string(assignedBlockBase) : "NO ASIGNADO")
              << std::endl;
}