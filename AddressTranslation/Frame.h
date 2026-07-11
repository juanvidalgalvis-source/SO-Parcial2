#ifndef FRAME_H
#define FRAME_H

// Representa un marco (frame) de memoria física.
// Cada marco puede estar libre u ocupado por la página de un proceso.
class Frame {
private:
    int frameNumber;   // Número identificador del marco (posición en memoria física)
    bool free;         // true si el marco está libre, false si está ocupado
    int pid;           // Proceso dueño del marco (-1 si está libre)
    int pageNumber;    // Número de página que ocupa este marco (-1 si está libre)

public:
    // Crea un marco libre identificado por frameNumber
    explicit Frame(int frameNumber);

    // Ocupa el marco asignándolo a la página pageNumber del proceso pid
    void occupy(int pid, int pageNumber);

    // Libera el marco, dejándolo disponible nuevamente
    void release();

    int getFrameNumber() const;
    bool isFree() const;
    int getPid() const;
    int getPageNumber() const;
};

#endif // FRAME_H
