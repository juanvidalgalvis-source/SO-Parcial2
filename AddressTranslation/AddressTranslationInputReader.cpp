#include "AddressTranslationInputReader.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace {

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::vector<std::string> splitByComma(const std::string& s) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ',')) {
        tokens.push_back(trim(item));
    }
    return tokens;
}

std::string toUpper(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                    [](unsigned char c) { return std::toupper(c); });
    return result;
}

// Intenta convertir s a entero. Devuelve true solo si TODO el
// contenido de s es un número válido (evita aceptar cosas como "12x").
bool parseIntSafe(const std::string& s, int& out) {
    if (s.empty()) {
        return false;
    }
    try {
        size_t pos = 0;
        out = std::stoi(s, &pos);
        return pos == s.size();
    } catch (...) {
        return false;
    }
}

} // namespace

bool AddressTranslationInputReader::parseCommandFile(const std::string& filePath,
                                                       int& physicalMemory,
                                                       int& virtualMemory,
                                                       int& pageSize,
                                                       std::vector<FileCommand>& commands,
                                                       std::vector<std::string>& errors,
                                                       std::vector<std::string>& warnings) {
    commands.clear();
    errors.clear();
    warnings.clear();
    physicalMemory = 0;
    virtualMemory = 0;
    pageSize = 0;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        errors.push_back("No se pudo abrir el archivo: " + filePath);
        return false;
    }

    bool physicalSet = false;
    bool virtualSet = false;
    bool pageSizeSet = false;
    bool commandsStarted = false;
    int lineNumber = 0;
    std::string line;

    while (std::getline(file, line)) {
        lineNumber++;
        std::string trimmed = trim(line);

        if (trimmed.empty() || trimmed[0] == '#') {
            continue;
        }

        size_t eqPos = trimmed.find('=');
        size_t commaPos = trimmed.find(',');
        bool looksLikeHeader = (eqPos != std::string::npos) &&
                                (commaPos == std::string::npos || eqPos < commaPos);

        if (looksLikeHeader) {
            if (commandsStarted) {
                warnings.push_back("Linea " + std::to_string(lineNumber) +
                                    ": encabezado despues de comandos, se ignora: " + trimmed);
                continue;
            }

            std::string key = trim(trimmed.substr(0, eqPos));
            std::string value = trim(trimmed.substr(eqPos + 1));
            int intValue;

            if (key == "MEMORIA_FISICA") {
                if (parseIntSafe(value, intValue) && intValue > 0) {
                    physicalMemory = intValue;
                    physicalSet = true;
                } else {
                    errors.push_back("Linea " + std::to_string(lineNumber) +
                                      ": MEMORIA_FISICA invalida: " + value);
                }
            } else if (key == "MEMORIA_VIRTUAL") {
                if (parseIntSafe(value, intValue) && intValue > 0) {
                    virtualMemory = intValue;
                    virtualSet = true;
                } else {
                    errors.push_back("Linea " + std::to_string(lineNumber) +
                                      ": MEMORIA_VIRTUAL invalida: " + value);
                }
            } else if (key == "TAMANO_PAGINA") {
                if (parseIntSafe(value, intValue) && intValue > 0) {
                    pageSize = intValue;
                    pageSizeSet = true;
                } else {
                    errors.push_back("Linea " + std::to_string(lineNumber) +
                                      ": TAMANO_PAGINA invalida: " + value);
                }
            } else {
                warnings.push_back("Linea " + std::to_string(lineNumber) +
                                    ": encabezado desconocido, se ignora: " + key);
            }
            continue;
        }

        // A partir de aqui se asume que la linea es un comando.
        if (!physicalSet || !virtualSet || !pageSizeSet) {
            errors.push_back("Linea " + std::to_string(lineNumber) +
                              ": aparece un comando antes de definir MEMORIA_FISICA, "
                              "MEMORIA_VIRTUAL y TAMANO_PAGINA");
        }
        commandsStarted = true;

        std::vector<std::string> parts = splitByComma(trimmed);
        if (parts.empty()) {
            warnings.push_back("Linea " + std::to_string(lineNumber) +
                                ": linea vacia o mal formada, se ignora");
            continue;
        }

        std::string command = toUpper(parts[0]);

        if (command == "CREAR") {
            if (parts.size() != 3) {
                errors.push_back("Linea " + std::to_string(lineNumber) +
                                  ": CREAR requiere 2 parametros (pid,tamano)");
                continue;
            }
            int pid, size;
            if (!parseIntSafe(parts[1], pid) || !parseIntSafe(parts[2], size) || size <= 0) {
                errors.push_back("Linea " + std::to_string(lineNumber) +
                                  ": CREAR con parametros invalidos");
                continue;
            }
            commands.push_back(FileCommand{CommandType::CREAR, pid, size});

        } else if (command == "LIBERAR") {
            if (parts.size() != 2) {
                errors.push_back("Linea " + std::to_string(lineNumber) +
                                  ": LIBERAR requiere 1 parametro (pid)");
                continue;
            }
            int pid;
            if (!parseIntSafe(parts[1], pid)) {
                errors.push_back("Linea " + std::to_string(lineNumber) +
                                  ": LIBERAR con pid invalido");
                continue;
            }
            commands.push_back(FileCommand{CommandType::LIBERAR, pid, 0});

        } else if (command == "TRADUCIR") {
            if (parts.size() != 3) {
                errors.push_back("Linea " + std::to_string(lineNumber) +
                                  ": TRADUCIR requiere 2 parametros (pid,direccion)");
                continue;
            }
            int pid, address;
            if (!parseIntSafe(parts[1], pid) || !parseIntSafe(parts[2], address) || address < 0) {
                errors.push_back("Linea " + std::to_string(lineNumber) +
                                  ": TRADUCIR con parametros invalidos");
                continue;
            }
            commands.push_back(FileCommand{CommandType::TRADUCIR, pid, address});

        } else {
            warnings.push_back("Linea " + std::to_string(lineNumber) +
                                ": comando desconocido, se ignora: " + command);
        }
    }

    if (!physicalSet) {
        errors.push_back("Falta el encabezado MEMORIA_FISICA en el archivo");
    }
    if (!virtualSet) {
        errors.push_back("Falta el encabezado MEMORIA_VIRTUAL en el archivo");
    }
    if (!pageSizeSet) {
        errors.push_back("Falta el encabezado TAMANO_PAGINA en el archivo");
    }

    return true; // El archivo se pudo abrir; el llamador debe revisar 'errors'.
}
