#ifndef PROCESSP_H
#define PROCESSP_H

#include <vector>
#include "Page.h"

// Representa un proceso que solicita memoria virtual mediante paginación.
// El nombre de la clase es ProcessP para diferenciarla de ProcessA
// (proceso de la Implementación 1: asignación contigua de memoria).
class ProcessP {
private:
    int pid;                    // Identificador del proceso
    int virtualSize;            // Tamaño de memoria virtual solicitado
    std::vector<Page> pages;    // Páginas en las que se divide el proceso
    bool allocated;             // true si todas sus páginas fueron asignadas a marcos

public:
    // Crea un proceso con pid y tamaño virtual solicitado.
    // Todavía no calcula sus páginas (ver calculateNumPages).
    ProcessP(int pid, int virtualSize);

    // Calcula cuántas páginas necesita el proceso según pageSize
    // (redondeando hacia arriba) y llena el vector pages con
    // objetos Page numerados desde 0.
    void calculateNumPages(int pageSize);

    void setAllocated(bool allocated);
    bool isAllocated() const;

    int getPid() const;
    int getVirtualSize() const;

    // Devuelve una referencia al vector de páginas para poder
    // modificarlas (asignarles marco) desde el MemoryTranslator.
    std::vector<Page>& getPages();

    // Overload de solo lectura, para código que solo necesita
    // consultar las páginas (por ejemplo, los reportes) sin poder
    // modificarlas, y que recibe el ProcessP como const.
    const std::vector<Page>& getPages() const;

    // Imprime en consola el pid, tamaño, número de páginas y,
    // por cada página, si es válida y a qué marco está asignada.
    void print() const;
};

#endif // PROCESSP_H
