#ifndef MEMORYTRANSLATOR_H
#define MEMORYTRANSLATOR_H

#include <vector>
#include "Frame.h"
#include "ProcessP.h"
#include "PageTable.h"

// MemoryTranslator (MMU): clase orquestadora de la Implementación 2.
// Administra la memoria física (marcos), la memoria virtual (procesos
// con sus páginas) y la tabla de páginas global, y resuelve la
// traducción de direcciones virtuales a físicas.
class MemoryTranslator {
private:
    int pageSize;                  // Tamaño de página (mismas unidades que las memorias)
    int physicalMemorySize;        // Tamaño total de memoria física
    int virtualMemorySize;         // Tamaño total de memoria virtual disponible para procesos
    std::vector<Frame> frames;     // Marcos de memoria física
    std::vector<ProcessP> processes; // Procesos registrados
    PageTable pageTable;           // Tabla de páginas global

    // Cuenta cuántos marcos libres hay actualmente.
    int countFreeFrames() const;

    // Busca el índice de un proceso por pid dentro de processes.
    // Devuelve -1 si no existe.
    int findProcessIndex(int pid) const;

public:
    // Crea el MMU con memoria física y virtual de los tamaños dados,
    // divididas en páginas/marcos de tamaño pageSize.
    MemoryTranslator(int physicalMemorySize, int virtualMemorySize, int pageSize);

    // Crea un proceso con el pid y tamaño virtual solicitados.
    // Primero valida que sumar este proceso no exceda la memoria
    // virtual total declarada (getUsedVirtualMemory() + virtualSize
    // <= virtualMemorySize); si la excede, falla sin tocar marcos.
    // Si pasa esa validación, calcula cuántas páginas necesita y,
    // si hay suficientes marcos libres para todas, las asigna y
    // mapea en la tabla de páginas (proceso queda allocated=true).
    // Si no alcanzan los marcos, NO se asigna ninguna página (falla
    // completo, sin asignaciones parciales) y el proceso queda
    // registrado con allocated=false.
    // Devuelve true si la creación fue exitosa, false si falló
    // (por cualquiera de los dos motivos anteriores).
    bool createProcess(int pid, int virtualSize);

    // Libera todos los marcos ocupados por el proceso pid, elimina
    // sus entradas de la tabla de páginas y quita el proceso de la
    // lista de procesos activos. Devuelve true si el proceso existía.
    bool freeProcess(int pid);

    // Traduce una dirección virtual del proceso pid a su dirección
    // física correspondiente. Devuelve la dirección física si la
    // traducción es válida, o -1 si el proceso no existe, la
    // dirección está fuera de rango, o la página no está mapeada.
    int translateAddress(int pid, int virtualAddress) const;

    // Devuelve cuántos marcos libres quedan.
    int getFreeFrameCount() const;

    // Imprime en consola el estado de los marcos, procesos y tabla
    // de páginas.
    void printMemoryState() const;
    void printProcesses() const;

    // Imprime en consola un resumen con los 5 elementos que pide el
    // enunciado: memoria virtual (total y usada), memoria física,
    // tamaño de página, tabla de páginas y marcos disponibles.
    void printSummary() const;

    int getPageSize() const;
    int getPhysicalMemorySize() const;
    int getVirtualMemorySize() const;

    // Suma el tamaño virtual de todos los procesos que sí lograron
    // ser asignados (allocated=true). Los procesos fallidos no
    // consumen memoria virtual. Público porque los reportes
    // (HTML/TXT) necesitan mostrar cuánta memoria virtual está en uso.
    int getUsedVirtualMemory() const;

    const std::vector<Frame>& getFrames() const;
    const std::vector<ProcessP>& getProcesses() const;
    const PageTable& getPageTable() const;
};

#endif // MEMORYTRANSLATOR_H
