#ifndef ADDRESSTRANSLATIONINPUTREADER_H
#define ADDRESSTRANSLATIONINPUTREADER_H

#include <string>
#include <vector>

// Tipo de comando leído desde el archivo de entrada de la Implementación 2.
enum class CommandType {
    CREAR,
    LIBERAR,
    TRADUCIR
};

// Representa un comando individual leído del archivo de entrada.
struct FileCommand {
    CommandType type;
    int pid;
    int value; // CREAR: tamaño virtual solicitado. TRADUCIR: dirección virtual. LIBERAR: no se usa (0).
};

// Lee y parsea el archivo de entrada de la Implementación 2 (traducción
// de direcciones virtuales a físicas). Formato esperado:
//
//   MEMORIA_FISICA=<entero mayor que 0>
//   MEMORIA_VIRTUAL=<entero mayor que 0>
//   TAMANO_PAGINA=<entero mayor que 0>
//   CREAR,<pid>,<tamano>
//   LIBERAR,<pid>
//   TRADUCIR,<pid>,<direccion>
//
// Las líneas que comienzan con # se ignoran (comentarios). Los tres
// encabezados deben aparecer antes que cualquier comando en el archivo.
class AddressTranslationInputReader {
public:
    // Parsea el archivo ubicado en filePath.
    // Devuelve true si el archivo se pudo abrir (incluso si su
    // contenido tuvo errores; el llamador debe revisar 'errors'
    // antes de ejecutar los comandos). Devuelve false solo si el
    // archivo no se pudo abrir.
    //
    // Salidas por referencia:
    //   physicalMemory, virtualMemory, pageSize: valores leídos de los encabezados
    //   commands: lista de comandos válidos, en el orden en que aparecen
    //   errors: problemas que impiden ejecutar con seguridad (encabezado
    //           faltante, comando mal formado, etc.)
    //   warnings: problemas menores que no detienen el parseo (línea o
    //             encabezado desconocido, etc.)
    static bool parseCommandFile(const std::string& filePath,
                                  int& physicalMemory,
                                  int& virtualMemory,
                                  int& pageSize,
                                  std::vector<FileCommand>& commands,
                                  std::vector<std::string>& errors,
                                  std::vector<std::string>& warnings);
};

#endif // ADDRESSTRANSLATIONINPUTREADER_H
