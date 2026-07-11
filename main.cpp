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
#include "AddressTranslation/AddressTranslationInputReader.h"
#include "AddressTranslation/AddressTranslationReportWriter.h"
#include "AddressTranslation/MemoryTranslator.h"

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

static int readImplementationChoice() {
    while (true) {
        int choice = AllocationInputReader::readInteger(
            "\n=== PASO 1: IMPLEMENTACIÓN ===\n"
            "Seleccione la implementación a usar:\n"
            "1) Asignación de memoria\n"
            "2) Traducción de direcciones\n"
            "Ingrese 1 o 2: ",
            1);

        if (choice == 1 || choice == 2) {
            return choice;
        }
        std::cout << "Opción no válida. Intente de nuevo." << std::endl;
    }
}

static void printTranslationMenu() {
    std::cout << "\n=== Menú de traducción de direcciones ===\n";
    std::cout << "1) Crear proceso\n";
    std::cout << "2) Liberar proceso\n";
    std::cout << "3) Traducir dirección\n";
    std::cout << "4) Ver estado actual en consola\n";
    std::cout << "5) Salir\n";
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
    std::cout << "Proyecto de simulación iniciado.\n";

    int implementationChoice = readImplementationChoice();

    const std::filesystem::path reportPath = std::filesystem::current_path() /
        (implementationChoice == 1 ? "MemoryAllocation" : "AddressTranslation") /
        "memory_report.html";
    const std::filesystem::path reportDir = reportPath.parent_path();
    if (!std::filesystem::exists(reportDir) && !std::filesystem::create_directories(reportDir)) {
        std::cerr << "No se pudo crear el directorio de reportes: " << reportDir.string() << "\n";
        return 1;
    }

    // Paso 2: Seleccionar modo (interactivo o archivo)
    std::cout << "\n=== PASO 2: MODO DE ENTRADA ===\n";
    std::string mode = AllocationInputReader::readMode();

    std::string selectedStrategy;
    if (implementationChoice == 1) {
        // Paso 3: Seleccionar estrategia solo para la Implementación 1
        std::cout << "\n=== PASO 3: ESTRATEGIA ===\n";
        selectedStrategy = AllocationInputReader::readStrategy();
    }

    // Ahora procesamos según el modo elegido
    if (implementationChoice == 1) {
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
    } else {
        if (mode == "interactive") {
            std::cout << "\n=== INICIANDO MODO INTERACTIVO ===\n";
            int physicalMemory = AllocationInputReader::readInteger("Tamaño de memoria física: ", 1);
            int virtualMemory = AllocationInputReader::readInteger("Tamaño de memoria virtual: ", 1);
            int pageSize = AllocationInputReader::readInteger("Tamaño de página: ", 1);

            MemoryTranslator mmu(physicalMemory, virtualMemory, pageSize);
            std::vector<std::string> commandLog;
            if (!AddressTranslationReportWriter::writeHtmlReport(mmu, commandLog, reportPath.string())) {
                std::cerr << "No se pudo escribir el archivo de reporte: " << reportPath.string() << "\n";
                return 1;
            }

            std::cout << "Abriendo el reporte inicial en el navegador...\n";
            openBrowser(reportPath);
            std::cout << "Reporte generado en: " << reportPath.string() << "\n";
            std::cout << "Recargue el navegador después de cada acción para ver el estado actualizado.\n";

            while (true) {
                printTranslationMenu();
                int option = AllocationInputReader::readInteger("Seleccione una opción: ", 1);

                switch (option) {
                    case 1: {
                        int pid = AllocationInputReader::readInteger("PID del proceso: ", 1);
                        int virtualSize = AllocationInputReader::readInteger("Tamaño virtual solicitado: ", 1);
                        bool created = mmu.createProcess(pid, virtualSize);
                        if (created) {
                            std::cout << "Proceso " << pid << " creado correctamente.\n";
                            commandLog.push_back("CREAR," + std::to_string(pid) + "," + std::to_string(virtualSize) + " -> OK");
                        } else {
                            std::string reason;
                            if (mmu.getUsedVirtualMemory() + virtualSize > mmu.getVirtualMemorySize()) {
                                reason = "No hay suficiente memoria virtual disponible.";
                            } else {
                                int pagesNeeded = (virtualSize + mmu.getPageSize() - 1) / mmu.getPageSize();
                                if (mmu.getFreeFrameCount() < pagesNeeded) {
                                    reason = "No hay suficientes marcos físicos libres.";
                                } else {
                                    reason = "No se pudo crear el proceso.";
                                }
                            }
                            std::cout << reason << "\n";
                            commandLog.push_back("CREAR," + std::to_string(pid) + "," + std::to_string(virtualSize) + " -> ERROR: " + reason);
                        }
                        break;
                    }
                    case 2: {
                        int pid = AllocationInputReader::readInteger("PID del proceso a liberar: ", 1);
                        if (mmu.freeProcess(pid)) {
                            std::cout << "Proceso " << pid << " liberado correctamente.\n";
                            commandLog.push_back("LIBERAR," + std::to_string(pid) + " -> OK");
                        } else {
                            std::cout << "No se encontró un proceso con PID " << pid << ".\n";
                            commandLog.push_back("LIBERAR," + std::to_string(pid) + " -> ERROR: PID no encontrado");
                        }
                        break;
                    }
                    case 3: {
                        int pid = AllocationInputReader::readInteger("PID del proceso: ", 1);
                        int virtualAddress = AllocationInputReader::readInteger("Dirección virtual: ", 0);
                        int physicalAddress = mmu.translateAddress(pid, virtualAddress);
                        if (physicalAddress >= 0) {
                            std::cout << "Dirección física: " << physicalAddress << "\n";
                            commandLog.push_back("TRADUCIR," + std::to_string(pid) + "," + std::to_string(virtualAddress) + " -> OK: " + std::to_string(physicalAddress));
                        } else {
                            std::string reason = "PID no existe o la dirección virtual no es válida.";
                            const auto& processes = mmu.getProcesses();
                            bool pidFound = false;
                            for (const auto& process : processes) {
                                if (process.getPid() == pid) {
                                    pidFound = true;
                                    if (virtualAddress < 0 || virtualAddress >= process.getVirtualSize()) {
                                        reason = "Dirección virtual fuera de rango.";
                                    } else if (!process.isAllocated()) {
                                        reason = "Proceso no tiene páginas asignadas.";
                                    } else {
                                        reason = "Página no mapeada o proceso sin paginación válida.";
                                    }
                                    break;
                                }
                            }
                            std::cout << reason << "\n";
                            commandLog.push_back("TRADUCIR," + std::to_string(pid) + "," + std::to_string(virtualAddress) + " -> ERROR: " + reason);
                        }
                        break;
                    }
                    case 4: {
                        mmu.printSummary();
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

                if (!AddressTranslationReportWriter::writeHtmlReport(mmu, commandLog, reportPath.string())) {
                    std::cerr << "Error al actualizar el reporte en: " << reportPath.string() << "\n";
                    return 1;
                }
                std::cout << "Reporte actualizado. Recargue " << reportPath.string() << " en su navegador para ver los cambios.\n";
            }
        } else {
            // Modo por archivo
            std::cout << "\n=== INICIANDO MODO POR ARCHIVO ===\n";
            std::cout << "Ingrese el nombre del archivo de comandos [procesos_ejemplo.txt]: ";
            std::string filePath;
            std::getline(std::cin, filePath);
            if (filePath.empty()) {
                filePath = "procesos_ejemplo.txt";
            }

            std::filesystem::path commandsPath(filePath);
            if (commandsPath.is_relative() && filePath.find_first_of("/\\") == std::string::npos) {
                commandsPath = std::filesystem::current_path() / "AddressTranslation" / "data" / filePath;
            }

            int physicalMemory = 0;
            int virtualMemory = 0;
            int pageSize = 0;
            std::vector<FileCommand> commands;
            std::vector<std::string> errors;
            std::vector<std::string> warnings;

            bool parseResult = AddressTranslationInputReader::parseCommandFile(
                commandsPath.string(), physicalMemory, virtualMemory, pageSize,
                commands, errors, warnings);

            if (!parseResult || !errors.empty()) {
                std::cerr << "No se pudo procesar el archivo de comandos:\n";
                for (const auto& message : errors) {
                    std::cerr << "  " << message << "\n";
                }
                return 1;
            }

            if (!warnings.empty()) {
                std::cout << "Advertencias al leer el archivo:\n";
                for (const auto& message : warnings) {
                    std::cout << "  " << message << "\n";
                }
            }

            MemoryTranslator mmu(physicalMemory, virtualMemory, pageSize);
            std::vector<std::string> commandLog;

            for (const auto& command : commands) {
                if (command.type == CommandType::CREAR) {
                    bool created = mmu.createProcess(command.pid, command.value);
                    if (created) {
                        commandLog.push_back("Línea: CREAR," + std::to_string(command.pid) + "," + std::to_string(command.value) + " -> OK");
                    } else {
                        commandLog.push_back("Línea: CREAR," + std::to_string(command.pid) + "," + std::to_string(command.value) + " -> ERROR");
                    }
                } else if (command.type == CommandType::LIBERAR) {
                    bool freed = mmu.freeProcess(command.pid);
                    if (freed) {
                        commandLog.push_back("Línea: LIBERAR," + std::to_string(command.pid) + " -> OK");
                    } else {
                        commandLog.push_back("Línea: LIBERAR," + std::to_string(command.pid) + " -> ERROR: PID no encontrado");
                    }
                } else if (command.type == CommandType::TRADUCIR) {
                    int physicalAddress = mmu.translateAddress(command.pid, command.value);
                    if (physicalAddress >= 0) {
                        commandLog.push_back("Línea: TRADUCIR," + std::to_string(command.pid) + "," + std::to_string(command.value) + " -> OK: " + std::to_string(physicalAddress));
                    } else {
                        commandLog.push_back("Línea: TRADUCIR," + std::to_string(command.pid) + "," + std::to_string(command.value) + " -> ERROR: no se pudo traducir");
                    }
                }
            }

            if (!AddressTranslationReportWriter::writeHtmlReport(mmu, commandLog, reportPath.string())) {
                std::cerr << "No se pudo escribir el archivo de reporte: " << reportPath.string() << "\n";
                return 1;
            }

            const std::filesystem::path outputDir = std::filesystem::current_path() / "output";
            if (!std::filesystem::exists(outputDir)) {
                if (!std::filesystem::create_directories(outputDir)) {
                    std::cerr << "No se pudo crear el directorio output: " << outputDir.string() << "\n";
                    return 1;
                }
            }

            std::string filenameBase = getFilenameWithoutExtension(filePath);
            const std::filesystem::path textReportPath = outputDir / ("output_at_" + filenameBase + ".txt");

            if (!AddressTranslationReportWriter::writeTextReport(mmu, commandLog, textReportPath.string())) {
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
}
