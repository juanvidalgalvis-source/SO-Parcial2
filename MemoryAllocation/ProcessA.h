#ifndef PROCESSA_H
#define PROCESSA_H

class ProcessA {
private:
    int pid;
    int requestedSize;
    bool allocated;
    int assignedBlockBase;   // Base del bloque donde quedó asignado (-1 si no asignado)

public:
    ProcessA(int pid, int requestedSize);

    // Getters
    int getPid() const;
    int getRequestedSize() const;
    bool isAllocated() const;
    int getAssignedBlockBase() const;

    // Modificadores de estado
    void assignTo(int blockBaseAddress);
    void deallocate();

    void print() const;
};

#endif