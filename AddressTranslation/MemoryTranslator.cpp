#include "MemoryTranslator.h"
#include <iostream>
#include <algorithm>

MemoryTranslator::MemoryTranslator(int physicalMemorySize, int virtualMemorySize, int pageSize)
    : pageSize(pageSize),
      physicalMemorySize(physicalMemorySize),
      virtualMemorySize(virtualMemorySize) {

    // La memoria física se divide en marcos de tamaño pageSize.
    int numFrames = physicalMemorySize / pageSize;
    for (int i = 0; i < numFrames; i++) {
        frames.push_back(Frame(i));
    }
}

int MemoryTranslator::countFreeFrames() const {
    int count = 0;
    for (const Frame& f : frames) {
        if (f.isFree()) {
            count++;
        }
    }
    return count;
}

int MemoryTranslator::findProcessIndex(int pid) const {
    for (size_t i = 0; i < processes.size(); i++) {
        if (processes[i].getPid() == pid) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int MemoryTranslator::getUsedVirtualMemory() const {
    int used = 0;
    for (const ProcessP& p : processes) {
        if (p.isAllocated()) {
            used += p.getVirtualSize();
        }
    }
    return used;
}

bool MemoryTranslator::createProcess(int pid, int virtualSize) {
    // Validacion 1: la memoria virtual total no debe excederse.
    // Si este proceso hace que la suma de memoria virtual usada supere
    // virtualMemorySize, falla de una vez sin tocar marcos ni paginas.
    if (getUsedVirtualMemory() + virtualSize > virtualMemorySize) {
        ProcessP failed(pid, virtualSize);
        failed.calculateNumPages(pageSize);
        failed.setAllocated(false);
        processes.push_back(failed);
        return false;
    }

    ProcessP process(pid, virtualSize);
    process.calculateNumPages(pageSize);

    int numPagesNeeded = static_cast<int>(process.getPages().size());

    // Validacion 2 (regla acordada): si no hay marcos suficientes para
    // TODAS las paginas, la creacion falla completo (sin asignaciones
    // parciales), igual que en la Implementacion 1 cuando findBlock
    // devuelve -1.
    if (countFreeFrames() < numPagesNeeded) {
        process.setAllocated(false);
        processes.push_back(process);
        return false;
    }

    // Hay marcos suficientes: se asigna cada pagina al primer marco
    // libre disponible (analogo a First Fit sobre los marcos).
    std::vector<Page>& pages = process.getPages();
    for (Page& page : pages) {
        for (Frame& frame : frames) {
            if (frame.isFree()) {
                frame.occupy(pid, page.getPageNumber());
                page.assignFrame(frame.getFrameNumber());
                pageTable.mapPage(pid, page.getPageNumber(), frame.getFrameNumber());
                break;
            }
        }
    }

    process.setAllocated(true);
    processes.push_back(process);
    return true;
}

bool MemoryTranslator::freeProcess(int pid) {
    int idx = findProcessIndex(pid);
    if (idx == -1) {
        return false;
    }

    // Libera todos los marcos que pertenecen a este proceso.
    for (Frame& frame : frames) {
        if (!frame.isFree() && frame.getPid() == pid) {
            frame.release();
        }
    }

    // Elimina las entradas de este proceso en la tabla de paginas.
    pageTable.unmapProcess(pid);

    // Quita el proceso de la lista de procesos activos.
    processes.erase(processes.begin() + idx);

    return true;
}

int MemoryTranslator::translateAddress(int pid, int virtualAddress) const {
    int idx = findProcessIndex(pid);
    if (idx == -1) {
        return -1; // Proceso no existe
    }

    const ProcessP& process = processes[idx];

    if (virtualAddress < 0 || virtualAddress >= process.getVirtualSize()) {
        return -1; // Direccion fuera del rango virtual del proceso
    }

    int pageNumber = virtualAddress / pageSize;
    int offset = virtualAddress % pageSize;

    int frameNumber = pageTable.lookup(pid, pageNumber);
    if (frameNumber == -1) {
        return -1; // Pagina no mapeada (proceso no asignado o fallo)
    }

    return frameNumber * pageSize + offset;
}

int MemoryTranslator::getFreeFrameCount() const {
    return countFreeFrames();
}

void MemoryTranslator::printMemoryState() const {
    std::cout << "--- Estado de la memoria fisica (marcos) ---" << std::endl;
    for (const Frame& f : frames) {
        std::cout << "Marco " << f.getFrameNumber() << ": ";
        if (f.isFree()) {
            std::cout << "libre" << std::endl;
        } else {
            std::cout << "ocupado por PID=" << f.getPid()
                      << " (pagina " << f.getPageNumber() << ")" << std::endl;
        }
    }
    std::cout << "Marcos libres: " << countFreeFrames() << "/" << frames.size() << std::endl;
}

void MemoryTranslator::printProcesses() const {
    std::cout << "--- Procesos registrados ---" << std::endl;
    for (const ProcessP& p : processes) {
        p.print();
    }
}

void MemoryTranslator::printSummary() const {
    std::cout << "===== Resumen del simulador (Implementacion 2) =====" << std::endl;
    std::cout << "Memoria virtual total : " << virtualMemorySize
              << " | usada: " << getUsedVirtualMemory() << std::endl;
    std::cout << "Memoria fisica total  : " << physicalMemorySize << std::endl;
    std::cout << "Tamano de pagina      : " << pageSize << std::endl;
    std::cout << "Marcos disponibles    : " << countFreeFrames() << "/" << frames.size() << std::endl;
    std::cout << std::endl;
    printMemoryState();
    std::cout << std::endl;
    pageTable.print();
    std::cout << "=====================================================" << std::endl;
}

int MemoryTranslator::getPageSize() const {
    return pageSize;
}

int MemoryTranslator::getPhysicalMemorySize() const {
    return physicalMemorySize;
}

int MemoryTranslator::getVirtualMemorySize() const {
    return virtualMemorySize;
}

const std::vector<Frame>& MemoryTranslator::getFrames() const {
    return frames;
}

const std::vector<ProcessP>& MemoryTranslator::getProcesses() const {
    return processes;
}

const PageTable& MemoryTranslator::getPageTable() const {
    return pageTable;
}
