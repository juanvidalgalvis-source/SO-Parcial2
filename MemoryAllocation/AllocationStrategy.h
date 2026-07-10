#ifndef ALLOCATIONSTRATEGY_H
#define ALLOCATIONSTRATEGY_H

#include <vector>
#include <string>
#include "MemoryBlock.h"
#include "ProcessA.h"

// Clase base abstracta (interfaz) para las estrategias de asignación.
// No se puede instanciar directamente: solo sus hijas (FirstFit, BestFit, WorstFit).
class AllocationStrategy {
public:
    virtual ~AllocationStrategy() {}

    // Recibe la lista de bloques de memoria y el proceso que necesita espacio.
    // Devuelve el INDICE del bloque elegido dentro del vector, o -1 si no hay
    // ningún bloque libre suficientemente grande.
    // Es "pura virtual" (= 0): obliga a cada subclase a implementarla.
    virtual int findBlock(const std::vector<MemoryBlock>& blocks, const ProcessA& process) = 0;

    // Cada estrategia debe poder identificarse (útil para reportes/logs)
    virtual std::string getName() const = 0;
};

#endif
