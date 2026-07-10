#include "AllocationInputReader.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

static std::string trim(const std::string& value) {
    std::string result = value;
    while (!result.empty() && std::isspace(static_cast<unsigned char>(result.back()))) {
        result.pop_back();
    }
    while (!result.empty() && std::isspace(static_cast<unsigned char>(result.front()))) {
        result.erase(result.begin());
    }
    return result;
}

static std::string stripUtf8Bom(const std::string& value) {
    if (value.size() >= 3 && static_cast<unsigned char>(value[0]) == 0xEF &&
        static_cast<unsigned char>(value[1]) == 0xBB &&
        static_cast<unsigned char>(value[2]) == 0xBF) {
        return value.substr(3);
    }
    return value;
}

std::string AllocationInputReader::selectImplementation() {
    std::cout << "Seleccione la implementación:\n";
    std::cout << "1) Implementación 1: Asignación de memoria\n";
    std::cout << "2) Implementación 2: Traducción de direcciones virtuales\n";
    std::cout << "Ingrese 1 o 2 (por defecto 1): ";

    std::string line;
    std::getline(std::cin, line);
    line = trim(line);

    if (line == "2") {
        return "AddressTranslation";
    }
    return "MemoryAllocation";
}

int AllocationInputReader::readInteger(const std::string& prompt, int minValue) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            return minValue;
        }

        try {
            int value = std::stoi(trim(line));
            if (value >= minValue) {
                return value;
            }
            std::cout << "Por favor ingrese un valor mayor o igual a " << minValue << ".\n";
        } catch (const std::exception&) {
            std::cout << "Entrada no válida. Intente de nuevo.\n";
        }
    }
}

std::string AllocationInputReader::readMode() {
    std::cout << "Seleccione el modo de entrada:\n";
    std::cout << "1) Modo interactivo\n";
    std::cout << "2) Modo por archivo\n";
    std::cout << "Ingrese 1 o 2 (por defecto 1): ";

    std::string line;
    std::getline(std::cin, line);
    line = trim(line);

    if (line == "2") {
        return "file";
    }
    return "interactive";
}

static std::string toUpper(const std::string& value) {
    std::string result;
    result.reserve(value.size());
    for (char c : value) {
        result += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return result;
}

bool AllocationInputReader::parseCommandFile(
    const std::string& path,
    int& outTotalMemory,
    std::vector<FileCommand>& outCommands,
    std::vector<std::string>& outLog
) {
    outCommands.clear();
    outLog.clear();
    outTotalMemory = 0;

    std::ifstream file(path);
    if (!file.is_open()) {
        outLog.push_back("No se pudo abrir el archivo: " + path);
        return false;
    }

    std::string line;
    int lineNumber = 0;
    bool totalSet = false;
    bool seenCommand = false;

    while (std::getline(file, line)) {
        lineNumber++;
        std::string rawLine = line;
        line = stripUtf8Bom(line);
        line = trim(line);
        if (line.empty() || line.rfind("#", 0) == 0) {
            continue;
        }

        const std::string totalPrefix = "TOTAL_MEMORIA=";
        std::string upperLine = toUpper(line);

        if (upperLine.rfind(totalPrefix, 0) == 0) {
            if (seenCommand) {
                outLog.push_back("Línea " + std::to_string(lineNumber) + ": HEADER después de comandos no permitido");
                return false;
            }
            if (totalSet) {
                outLog.push_back("Línea " + std::to_string(lineNumber) + ": TOTAL_MEMORIA ya definido");
                return false;
            }
            std::string value = trim(line.substr(totalPrefix.size()));
            if (value.empty()) {
                outLog.push_back("Línea " + std::to_string(lineNumber) + ": TOTAL_MEMORIA sin valor");
                return false;
            }
            try {
                outTotalMemory = std::stoi(value);
            } catch (const std::exception&) {
                outLog.push_back("Línea " + std::to_string(lineNumber) + ": TOTAL_MEMORIA debe ser un entero");
                return false;
            }
            if (outTotalMemory <= 0) {
                outLog.push_back("Línea " + std::to_string(lineNumber) + ": TOTAL_MEMORIA debe ser mayor que 0");
                return false;
            }
            totalSet = true;
            continue;
        }

        // Ignorar líneas de ESTRATEGIA (ya no se leen del archivo)
        const std::string strategyPrefix = "ESTRATEGIA=";
        if (upperLine.rfind(strategyPrefix, 0) == 0) {
            outLog.push_back("Línea " + std::to_string(lineNumber) + ": ESTRATEGIA en archivo ignorada (se selecciona por consola)");
            continue;
        }

        seenCommand = true;
        std::string command;
        std::string argument;
        size_t commaPos = line.find(',');
        if (commaPos != std::string::npos) {
            command = trim(line.substr(0, commaPos));
            argument = trim(line.substr(commaPos + 1));
        } else {
            command = trim(line);
        }
        std::string commandUpper = toUpper(command);

        if (commandUpper == "CREAR") {
            if (argument.empty()) {
                outLog.push_back("Línea " + std::to_string(lineNumber) + ": CREAR requiere un tamaño");
                continue;
            }
            try {
                int size = std::stoi(argument);
                if (size <= 0) {
                    outLog.push_back("Línea " + std::to_string(lineNumber) + ": CREAR requiere tamaño mayor que 0");
                    continue;
                }
                outCommands.push_back({true, size, lineNumber, rawLine});
            } catch (const std::exception&) {
                outLog.push_back("Línea " + std::to_string(lineNumber) + ": CREAR tiene tamaño inválido");
            }
            continue;
        }

        if (commandUpper == "LIBERAR") {
            if (argument.empty()) {
                outLog.push_back("Línea " + std::to_string(lineNumber) + ": LIBERAR requiere PID");
                continue;
            }
            try {
                int pid = std::stoi(argument);
                if (pid <= 0) {
                    outLog.push_back("Línea " + std::to_string(lineNumber) + ": LIBERAR requiere PID mayor que 0");
                    continue;
                }
                outCommands.push_back({false, pid, lineNumber, rawLine});
            } catch (const std::exception&) {
                outLog.push_back("Línea " + std::to_string(lineNumber) + ": LIBERAR tiene PID inválido");
            }
            continue;
        }

        outLog.push_back("Línea " + std::to_string(lineNumber) + ": comando desconocido: " + rawLine);
    }

    if (!totalSet) {
        outLog.push_back("Falta TOTAL_MEMORIA en el archivo de comandos.");
        return false;
    }
    return true;
}

std::string AllocationInputReader::readStrategy() {
    std::cout << "Seleccione la estrategia de asignación:\n";
    std::cout << "1) First Fit\n";
    std::cout << "2) Best Fit\n";
    std::cout << "3) Worst Fit\n";
    std::cout << "Ingrese 1, 2 o 3 (por defecto 1): ";

    std::string line;
    std::getline(std::cin, line);
    line = trim(line);

    if (line == "2") {
        return "BestFit";
    }
    if (line == "3") {
        return "WorstFit";
    }
    return "FirstFit";
}
