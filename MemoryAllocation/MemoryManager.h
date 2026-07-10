#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <vector>
#include <string>
#include "MemoryBlock.h"
#include "ProcessA.h"
#include "AllocationStrategy.h"

// Clase orquestadora: mantiene el estado de la memoria, usa una estrategia
// de asignación (inyectada) y coordina la creación/liberación de procesos.
class MemoryManager {
private:
    std::vector<MemoryBlock> blocks;       // Estado real de la memoria (bloques libres/ocupados)
    std::vector<ProcessA> processes;       // Procesos que han sido creados
    AllocationStrategy* strategy;          // Estrategia activa (First/Best/Worst Fit)
    int totalMemorySize;

public:
    // totalSize: tamaño total de la memoria simulada.
    // initialStrategy: qué estrategia usar al arrancar (se puede cambiar luego).
    MemoryManager(int totalSize, AllocationStrategy* initialStrategy);

    // Permite cambiar de estrategia en caliente (ej: el usuario elige otra en el menú)
    void setStrategy(AllocationStrategy* newStrategy);

    // Intenta crear un proceso y asignarle memoria usando la estrategia activa.
    // Devuelve true si se pudo asignar, false si no había espacio suficiente.
    bool createProcess(int pid, int requestedSize);

    // Libera la memoria ocupada por el proceso con ese pid.
    // Devuelve true si el proceso existía y estaba asignado, false si no.
    bool freeProcess(int pid);

    // Fragmentación externa: suma de todos los huecos libres que,
    // individualmente, son insuficientes para el proceso más pequeño pendiente
    // (para el informe, calculamos la versión simple: suma de todo el espacio
    // libre disperso en más de un bloque).
    int getExternalFragmentation() const;

    // Fragmentación interna: diferencia entre lo reservado en un bloque
    // y lo realmente usado por el proceso (aplica si no partimos bloques exactos).
    int getInternalFragmentation() const;

    // Reportes / estado
    void printMemoryState() const;
    void printProcesses() const;

    // Getters útiles para exportar a JSON/HTML más adelante
    const std::vector<MemoryBlock>& getBlocks() const;
    const std::vector<ProcessA>& getProcesses() const;
};

#endif
