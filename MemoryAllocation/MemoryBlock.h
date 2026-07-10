#ifndef MEMORYBLOCK_H
#define MEMORYBLOCK_H

class MemoryBlock {
private:
    int baseAddress;   // Direccion donde inicia el bloque
    int size;           // Tamano del bloque en KB (o unidad que definas)
    bool free;           // true = libre, false = ocupado
    int processId;      // PID del proceso que lo ocupa (-1 si está libre)

public:
    MemoryBlock(int baseAddress, int size);

    // Getters
    int getBaseAddress() const;
    int getSize() const;
    bool isFree() const;
    int getProcessId() const;

    // Modificadores de estado
    void occupy(int processId);
    void release();

    // Utilidad para debug / reportes
    void print() const;
};

#endif