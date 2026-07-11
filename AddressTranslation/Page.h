#ifndef PAGE_H
#define PAGE_H

// Representa una página virtual perteneciente a un proceso.
// Una página es válida cuando tiene un marco físico asignado.
class Page {
private:
    int pageNumber;   // Número de página dentro del espacio virtual del proceso
    int frameNumber;  // Marco físico asignado (-1 si no está asignada)
    bool valid;        // true si la página está mapeada a un marco físico

public:
    // Crea una página no asignada (inválida) identificada por pageNumber
    explicit Page(int pageNumber);

    // Asigna la página al marco físico frameNumber y la marca como válida
    void assignFrame(int frameNumber);

    // Invalida la página, quitando su asignación de marco
    void invalidate();

    int getPageNumber() const;
    int getFrameNumber() const;
    bool isValid() const;
};

#endif // PAGE_H
