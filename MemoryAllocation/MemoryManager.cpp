#include "MemoryManager.h"
#include <iostream>
#include <algorithm>

MemoryManager::MemoryManager(int totalSize, AllocationStrategy* initialStrategy)
    : strategy(initialStrategy), totalMemorySize(totalSize) {
    // Al arrancar, toda la memoria es UN solo bloque libre.
    blocks.push_back(MemoryBlock(0, totalSize));
}

void MemoryManager::setStrategy(AllocationStrategy* newStrategy) {
    strategy = newStrategy;
}

bool MemoryManager::createProcess(int pid, int requestedSize) {
    ProcessA proc(pid, requestedSize);

    int idx = strategy->findBlock(blocks, proc);
    if (idx == -1) {
        // No hay bloque libre suficientemente grande.
        // Igual guardamos el proceso (sin asignar) para que quede registrado
        // en los reportes que "se intentó" y falló.
        processes.push_back(proc);
        return false;
    }

    int base = blocks[idx].getBaseAddress();
    int availableSize = blocks[idx].getSize();

    // Quitamos el bloque libre original...
    blocks.erase(blocks.begin() + idx);

    // ...y lo reemplazamos por un bloque OCUPADO del tamaño EXACTO pedido.
    // Esto evita fragmentación interna (ver nota en el informe: es una
    // decisión de diseño válida, ya que el enunciado dice "si aplica").
    MemoryBlock occupied(base, requestedSize);
    occupied.occupy(pid);
    blocks.insert(blocks.begin() + idx, occupied);

    // Si sobró espacio, ese resto queda como un nuevo bloque LIBRE
    // justo después del que acabamos de ocupar.
    if (availableSize > requestedSize) {
        MemoryBlock leftover(base + requestedSize, availableSize - requestedSize);
        blocks.insert(blocks.begin() + idx + 1, leftover);
    }

    proc.assignTo(base);
    processes.push_back(proc);
    return true;
}

bool MemoryManager::freeProcess(int pid) {
    // 1. Buscamos el proceso en nuestro registro
    auto procIt = std::find_if(processes.begin(), processes.end(),
        [pid](const ProcessA& p) { return p.getPid() == pid; });

    if (procIt == processes.end() || !procIt->isAllocated()) {
        return false; // No existe o nunca fue asignado
    }

    // 2. Buscamos el bloque de memoria que le pertenece
    auto blockIt = std::find_if(blocks.begin(), blocks.end(),
        [pid](const MemoryBlock& b) { return !b.isFree() && b.getProcessId() == pid; });

    if (blockIt == blocks.end()) {
        return false; // No debería pasar, pero por seguridad
    }

    blockIt->release();
    procIt->deallocate();

    // 3. Coalescing: si el bloque vecino (izquierda o derecha) también
    // está libre, los fusionamos en uno solo. Esto reduce la fragmentación
    // externa que se iría acumulando con el tiempo.
    int idx = static_cast<int>(blockIt - blocks.begin());

    // Fusionar con el bloque de la derecha si también está libre
    if (idx + 1 < static_cast<int>(blocks.size()) && blocks[idx + 1].isFree()) {
        int mergedSize = blocks[idx].getSize() + blocks[idx + 1].getSize();
        MemoryBlock merged(blocks[idx].getBaseAddress(), mergedSize);
        blocks.erase(blocks.begin() + idx, blocks.begin() + idx + 2);
        blocks.insert(blocks.begin() + idx, merged);
    }

    // Fusionar con el bloque de la izquierda si también está libre
    if (idx - 1 >= 0 && blocks[idx - 1].isFree()) {
        int mergedSize = blocks[idx - 1].getSize() + blocks[idx].getSize();
        MemoryBlock merged(blocks[idx - 1].getBaseAddress(), mergedSize);
        blocks.erase(blocks.begin() + idx - 1, blocks.begin() + idx + 1);
        blocks.insert(blocks.begin() + idx - 1, merged);
    }

    return true;
}

int MemoryManager::getExternalFragmentation() const {
    // Definición usada: espacio libre total menos el bloque libre más grande.
    // Es la memoria que SÍ está libre pero que, por estar repartida en
    // huecos pequeños y no contiguos, no serviría para un proceso grande.
    int totalFree = 0;
    int largestFree = 0;

    for (const auto& b : blocks) {
        if (b.isFree()) {
            totalFree += b.getSize();
            largestFree = std::max(largestFree, b.getSize());
        }
    }
    return totalFree - largestFree;
}

int MemoryManager::getInternalFragmentation() const {
    // Con el diseño actual (partimos el bloque al tamaño exacto del proceso),
    // no queda desperdicio dentro de un bloque ocupado: internal frag = 0.
    // Se deja el método porque el enunciado lo pide explícitamente,
    // y en el informe se explica esta decisión de diseño.
    return 0;
}

void MemoryManager::printMemoryState() const {
    std::cout << "\n=== Estado de la memoria (total: " << totalMemorySize << ") ===\n";
    for (const auto& b : blocks) {
        b.print();
    }
    std::cout << "Fragmentacion externa: " << getExternalFragmentation() << "\n";
    std::cout << "Fragmentacion interna: " << getInternalFragmentation() << "\n";
}

void MemoryManager::printProcesses() const {
    std::cout << "\n=== Procesos ===\n";
    for (const auto& p : processes) {
        p.print();
    }
}

const std::vector<MemoryBlock>& MemoryManager::getBlocks() const {
    return blocks;
}

const std::vector<ProcessA>& MemoryManager::getProcesses() const {
    return processes;
}
