#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include "MemoryAllocation/AllocationInputReader.h"
#include "MemoryAllocation/AllocationReportWriter.h"
#include "MemoryAllocation/MemoryManager.h"
#include "MemoryAllocation/FirstFit.h"
#include "MemoryAllocation/BestFit.h"
#include "MemoryAllocation/WorstFit.h"

static std::unique_ptr<AllocationStrategy> createStrategy(const std::string& name) {
    if (name == "BestFit") {
        return std::make_unique<BestFit>();
    }
    if (name == "WorstFit") {
        return std::make_unique<WorstFit>();
    }
    return std::make_unique<FirstFit>();
}

static void printMenu(const std::string& currentStrategy, int nextPid) {
    std::cout << "\n=== Menú de simulación de memoria ===\n";
    std::cout << "Siguiente PID disponible: " << nextPid << "\n";
    std::cout << "Estrategia activa: " << currentStrategy << "\n";
    std::cout << "1) Crear proceso\n";
    std::cout << "2) Liberar proceso\n";
    std::cout << "3) Cambiar estrategia\n";
    std::cout << "4) Ver estado actual en consola\n";
    std::cout << "5) Salir\n";
}

static void openBrowser(const std::filesystem::path& filePath) {
    std::string command;
#ifdef _WIN32
    command = "start \"\" \"" + filePath.string() + "\"";
#else
    command = "xdg-open \"" + filePath.string() + "\"";
#endif
    if (std::system(command.c_str()) != 0) {
        std::cerr << "No se pudo abrir el navegador automáticamente.\n";
    }
}

static std::string formatCommandLog(const std::vector<std::string>& lines) {
    std::string output;
    for (const auto& line : lines) {
        output += line;
        output += '\n';
    }
    return output;
}

static std::string getFilenameWithoutExtension(const std::string& filepath) {
    std::filesystem::path p(filepath);
    std::string filename = p.filename().string();
    size_t lastDot = filename.find_last_of(".");
    if (lastDot != std::string::npos) {
        return filename.substr(0, lastDot);
    }
    return filename;
}

int main() {
    std::cout << "Proyecto de simulación de asignación de memoria iniciado.\n";

    const std::filesystem::path reportPath = std::filesystem::current_path() / "MemoryAllocation" / "memory_report.html";
    const std::filesystem::path reportDir = reportPath.parent_path();
    if (!std::filesystem::exists(reportDir) && !std::filesystem::create_directories(reportDir)) {
        std::cerr << "No se pudo crear el directorio de reportes: " << reportDir.string() << "\n";
        return 1;
    }

    // Paso 1: Seleccionar implementación
    std::cout << "\n=== PASO 1: IMPLEMENTACIÓN ===\n";
    std::string implementationChoice;
    std::cout << "Seleccione la implementación a usar:\n";
    std::cout << "1) Asignación de memoria\n";
    std::cout << "2) Traducción de direcciones (no disponible todavía)\n";
    std::cout << "Ingrese 1 o 2 (por defecto 1): ";
    std::getline(std::cin, implementationChoice);
    implementationChoice = implementationChoice.empty() ? "1" : implementationChoice;
    if (implementationChoice != "1") {
        std::cout << "La implementación de Traducción de direcciones aún no está disponible. Se usará Asignación de memoria.\n";
    }

    // Paso 2: Seleccionar modo (interactivo o archivo)
    std::cout << "\n=== PASO 2: MODO DE ENTRADA ===\n";
    std::string mode = AllocationInputReader::readMode();

    // Paso 3: Seleccionar estrategia (siempre, para ambos modos)
    std::cout << "\n=== PASO 3: ESTRATEGIA ===\n";
    std::string selectedStrategy = AllocationInputReader::readStrategy();

    // Ahora procesamos según el modo elegido
    if (mode == "interactive") {
        std::cout << "\n=== INICIANDO MODO INTERACTIVO ===\n";
        std::cout << "Estrategia seleccionada: " << selectedStrategy << "\n";

        auto currentStrategy = createStrategy(selectedStrategy);
        MemoryManager manager(1024, currentStrategy.get());
        if (!AllocationReportWriter::writeHtmlReport(manager, reportPath.string())) {
            std::cerr << "No se pudo escribir el archivo de reporte: " << reportPath.string() << "\n";
            return 1;
        }

        std::cout << "Abriendo el reporte inicial en el navegador...\n";
        openBrowser(reportPath);
        std::cout << "Reporte generado en: " << reportPath.string() << "\n";
        std::cout << "Recargue el navegador después de cada acción para ver el estado actualizado.\n";

        int nextPid = 1;
        std::string currentStrategyName = selectedStrategy;

        while (true) {
            printMenu(currentStrategyName, nextPid);
            int option = AllocationInputReader::readInteger("Seleccione una opción: ", 1);

            switch (option) {
                case 1: {
                    int requestedSize = AllocationInputReader::readInteger("Tamaño solicitado para el nuevo proceso: ", 1);
                    bool created = manager.createProcess(nextPid, requestedSize);
                    if (created) {
                        std::cout << "Proceso " << nextPid << " creado correctamente.\n";
                    } else {
                        std::cout << "No se pudo asignar memoria para el proceso " << nextPid << ".\n";
                    }
                    nextPid++;
                    break;
                }
                case 2: {
                    int pidToFree = AllocationInputReader::readInteger("PID del proceso a liberar: ", 1);
                    if (manager.freeProcess(pidToFree)) {
                        std::cout << "Proceso " << pidToFree << " liberado correctamente.\n";
                    } else {
                        std::cout << "No se encontró un proceso asignado con PID " << pidToFree << ".\n";
                    }
                    break;
                }
                case 3: {
                    std::cout << "\n=== CAMBIAR ESTRATEGIA ===\n";
                    std::string newStrategy = AllocationInputReader::readStrategy();
                    currentStrategy = createStrategy(newStrategy);
                    currentStrategyName = newStrategy;
                    manager.setStrategy(currentStrategy.get());
                    std::cout << "Estrategia cambiada a " << currentStrategyName << ".\n";
                    break;
                }
                case 4: {
                    manager.printMemoryState();
                    manager.printProcesses();
                    break;
                }
                case 5: {
                    std::cout << "Saliendo de la simulación.\n";
                    return 0;
                }
                default: {
                    std::cout << "Opción no válida. Intente de nuevo.\n";
                    break;
                }
            }

            if (!AllocationReportWriter::writeHtmlReport(manager, reportPath.string())) {
                std::cerr << "Error al actualizar el reporte en: " << reportPath.string() << "\n";
                return 1;
            }
            std::cout << "Reporte actualizado. Recargue " << reportPath.string() << " en su navegador para ver los cambios.\n";
        }
    } else {
        // Modo por archivo
        std::cout << "\n=== INICIANDO MODO POR ARCHIVO ===\n";
        std::cout << "Estrategia seleccionada: " << selectedStrategy << "\n";

        std::cout << "Ingrese el nombre del archivo de comandos [procesos_ejemplo.txt]: ";
        std::string filePath;
        std::getline(std::cin, filePath);
        if (filePath.empty()) {
            filePath = "procesos_ejemplo.txt";
        }

        std::filesystem::path commandsPath(filePath);
        if (commandsPath.is_relative() && filePath.find_first_of("/\\") == std::string::npos) {
            commandsPath = std::filesystem::current_path() / "MemoryAllocation" / "data" / filePath;
        }

        int totalMemory = 0;
        std::vector<AllocationInputReader::FileCommand> commands;
        std::vector<std::string> parseLog;
        if (!AllocationInputReader::parseCommandFile(commandsPath.string(), totalMemory, commands, parseLog)) {
            std::cerr << "No se pudo procesar el archivo de comandos:\n";
            for (const auto& message : parseLog) {
                std::cerr << "  " << message << "\n";
            }
            return 1;
        }

        if (!parseLog.empty()) {
            std::cout << "Advertencias al leer el archivo:\n";
            for (const auto& message : parseLog) {
                std::cout << "  " << message << "\n";
            }
        }

        auto currentStrategy = createStrategy(selectedStrategy);
        MemoryManager manager(totalMemory, currentStrategy.get());
        int nextPid = 1;
        std::vector<std::string> commandLog;

        for (const auto& command : commands) {
            if (command.isCreate) {
                bool created = manager.createProcess(nextPid, command.value);
                if (created) {
                    commandLog.push_back("Línea " + std::to_string(command.lineNumber) + ": " + command.rawLine + " -> OK PID " + std::to_string(nextPid));
                } else {
                    commandLog.push_back("Línea " + std::to_string(command.lineNumber) + ": " + command.rawLine + " -> NO ASIGNADO PID " + std::to_string(nextPid));
                }
                nextPid++;
            } else {
                bool freed = manager.freeProcess(command.value);
                if (freed) {
                    commandLog.push_back("Línea " + std::to_string(command.lineNumber) + ": " + command.rawLine + " -> LIBERADO");
                } else {
                    commandLog.push_back("Línea " + std::to_string(command.lineNumber) + ": " + command.rawLine + " -> NO EXISTE/TIPO NO ASIGNADO");
                }
            }
        }

        if (!AllocationReportWriter::writeHtmlReport(manager, reportPath.string(), formatCommandLog(commandLog))) {
            std::cerr << "No se pudo escribir el archivo de reporte: " << reportPath.string() << "\n";
            return 1;
        }

        // Crear directorio output si no existe
        const std::filesystem::path outputDir = std::filesystem::current_path() / "output";
        if (!std::filesystem::exists(outputDir)) {
            if (!std::filesystem::create_directories(outputDir)) {
                std::cerr << "No se pudo crear el directorio output: " << outputDir.string() << "\n";
                return 1;
            }
        }

        // Generar archivo .txt con el nombre output_[nombre_archivo_sin_extension].txt
        std::string filenameBase = getFilenameWithoutExtension(filePath);
        const std::filesystem::path textReportPath = outputDir / ("output_" + filenameBase + ".txt");
        
        if (!AllocationReportWriter::writeTextReport(manager, textReportPath.string(), formatCommandLog(commandLog))) {
            std::cerr << "No se pudo escribir el archivo de reporte de texto: " << textReportPath.string() << "\n";
            return 1;
        }

        std::cout << "Procesamiento de archivo completado.\n";
        if (!commandLog.empty()) {
            std::cout << "Registro de comandos procesados:\n";
            for (const auto& line : commandLog) {
                std::cout << "  " << line << "\n";
            }
        }
        std::cout << "Reporte HTML generado en: " << reportPath.string() << "\n";
        std::cout << "Reporte TXT generado en: " << textReportPath.string() << "\n";
        std::cout << "Abra o recargue los archivos para ver el resultado.\n";
        openBrowser(reportPath);
        return 0;
    }
}
