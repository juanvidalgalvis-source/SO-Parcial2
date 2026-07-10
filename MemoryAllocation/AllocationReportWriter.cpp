#include "AllocationReportWriter.h"
#include "MemoryManager.h"
#include <fstream>
#include <sstream>

namespace {
    std::string escapeHtml(const std::string& value) {
        std::string result;
        for (char c : value) {
            switch (c) {
                case '&': result += "&amp;"; break;
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                case '"': result += "&quot;"; break;
                case '\'': result += "&#39;"; break;
                default: result += c; break;
            }
        }
        return result;
    }
}

std::string AllocationReportWriter::formatMemoryState(const MemoryManager& manager) {
    std::ostringstream output;
    output << "Estado de memoria:\n";
    int index = 0;
    for (const auto& block : manager.getBlocks()) {
        output << "Bloque " << index++ << ": base=" << block.getBaseAddress()
               << ", tamaño=" << block.getSize()
               << ", " << (block.isFree() ? "LIBRE" : "OCUPADO")
               << (block.isFree() ? "" : ", PID=" + std::to_string(block.getProcessId()))
               << "\n";
    }
    output << "\n";
    output << "Fragmentación externa: " << manager.getExternalFragmentation() << "\n";
    output << "Fragmentación interna: " << manager.getInternalFragmentation() << "\n";
    return output.str();
}

std::string AllocationReportWriter::formatProcessReport(const MemoryManager& manager) {
    std::ostringstream output;
    output << "Procesos:\n";
    for (const auto& process : manager.getProcesses()) {
        output << "PID=" << process.getPid()
               << ", solicitado=" << process.getRequestedSize()
               << ", " << (process.isAllocated() ? "ASIGNADO" : "NO ASIGNADO");
        if (process.isAllocated()) {
            output << ", base=" << process.getAssignedBlockBase();
        }
        output << "\n";
    }
    return output.str();
}

std::string AllocationReportWriter::formatHtmlReport(const MemoryManager& manager, const std::string& commandLog) {
    std::ostringstream html;
    html << "<html><head><meta charset=\"UTF-8\"><title>Reporte de Memoria</title>"
         << "<style>body{font-family:Arial,sans-serif;}table{border-collapse:collapse;width:100%;}"
         << "th,td{border:1px solid #ccc;padding:8px;text-align:left;}th{background:#f3f3f3;}</style>"
         << "</head><body>"
         << "<h1>Reporte de Memoria</h1>"
         << "<h2>Bloques</h2>"
         << "<table><thead><tr><th>Bloque</th><th>Base</th><th>Tamaño</th><th>Estado</th><th>PID</th></tr></thead><tbody>";
    int index = 0;
    for (const auto& block : manager.getBlocks()) {
        html << "<tr><td>" << index++ << "</td>"
             << "<td>" << block.getBaseAddress() << "</td>"
             << "<td>" << block.getSize() << "</td>"
             << "<td>" << (block.isFree() ? "LIBRE" : "OCUPADO") << "</td>"
             << "<td>" << (block.isFree() ? "-" : std::to_string(block.getProcessId())) << "</td></tr>";
    }
    html << "</tbody></table>";
    html << "<h2>Resumen</h2>"
         << "<p>Fragmentación externa: " << manager.getExternalFragmentation() << "</p>"
         << "<p>Fragmentación interna: " << manager.getInternalFragmentation() << "</p>";
    html << "<h2>Procesos</h2><pre>";
    html << escapeHtml(formatProcessReport(manager));
    html << "</pre>";
    if (!commandLog.empty()) {
        html << "<h2>Registro de comandos</h2><pre>";
        html << escapeHtml(commandLog);
        html << "</pre>";
    }
    html << "</body></html>";
    return html.str();
}

bool AllocationReportWriter::writeHtmlReport(const MemoryManager& manager, const std::string& filename, const std::string& commandLog) {
    std::ofstream file(filename, std::ofstream::trunc);
    if (!file.is_open()) {
        return false;
    }
    file << formatHtmlReport(manager, commandLog);
    return file.good();
}

std::string AllocationReportWriter::formatTextReport(const MemoryManager& manager, const std::string& commandLog) {
    std::ostringstream output;
    
    output << "================================================================================\n";
    output << "REPORTE DE MEMORIA\n";
    output << "================================================================================\n\n";
    
    output << "BLOQUES DE MEMORIA\n";
    output << "--------------------------------------------------------------------------------\n";
    int index = 0;
    for (const auto& block : manager.getBlocks()) {
        output << "Bloque " << index << ":\n";
        output << "  Base: " << block.getBaseAddress() << "\n";
        output << "  Tamaño: " << block.getSize() << "\n";
        output << "  Estado: " << (block.isFree() ? "LIBRE" : "OCUPADO") << "\n";
        if (!block.isFree()) {
            output << "  PID: " << block.getProcessId() << "\n";
        }
        output << "\n";
        index++;
    }
    
    output << "RESUMEN\n";
    output << "--------------------------------------------------------------------------------\n";
    output << "Fragmentación externa: " << manager.getExternalFragmentation() << "\n";
    output << "Fragmentación interna: " << manager.getInternalFragmentation() << "\n\n";
    
    output << "PROCESOS\n";
    output << "--------------------------------------------------------------------------------\n";
    output << formatProcessReport(manager);
    output << "\n";
    
    if (!commandLog.empty()) {
        output << "REGISTRO DE COMANDOS\n";
        output << "--------------------------------------------------------------------------------\n";
        output << commandLog;
        output << "\n";
    }
    
    output << "================================================================================\n";
    output << "FIN DEL REPORTE\n";
    output << "================================================================================\n";
    
    return output.str();
}

bool AllocationReportWriter::writeTextReport(const MemoryManager& manager, const std::string& filename, const std::string& commandLog) {
    std::ofstream file(filename, std::ofstream::trunc);
    if (!file.is_open()) {
        return false;
    }
    file << formatTextReport(manager, commandLog);
    return file.good();
}
