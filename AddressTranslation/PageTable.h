#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <vector>

// Tabla de páginas GLOBAL: una sola tabla compartida por todos los
// procesos del simulador (no una tabla por proceso). Cada entrada
// mapea la combinación (pid, pageNumber) -> frameNumber.
class PageTable {
private:
    // Entrada individual de la tabla de páginas.
    struct Entry {
        int pid;
        int pageNumber;
        int frameNumber;
        bool valid;
    };

    std::vector<Entry> entries;

public:
    PageTable();

    // Agrega una entrada que mapea la página pageNumber del proceso pid
    // al marco físico frameNumber.
    void mapPage(int pid, int pageNumber, int frameNumber);

    // Elimina todas las entradas pertenecientes al proceso pid
    // (se usa al liberar un proceso).
    void unmapProcess(int pid);

    // Busca el marco físico asignado a la página pageNumber del proceso pid.
    // Devuelve el frameNumber si existe una entrada válida, o -1 si no
    // se encuentra (página no asignada o proceso inexistente).
    int lookup(int pid, int pageNumber) const;

    // Devuelve todas las entradas actuales (solo lectura), útil para
    // que los reportes (HTML/TXT) impriman el contenido completo
    // de la tabla de páginas.
    const std::vector<Entry>& getEntries() const;

    // Imprime en consola el contenido completo de la tabla de páginas.
    void print() const;
};

#endif // PAGETABLE_H
