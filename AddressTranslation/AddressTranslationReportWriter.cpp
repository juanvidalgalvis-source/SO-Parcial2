#include "AddressTranslationReportWriter.h"
#include <fstream>
#include <sstream>

std::string AddressTranslationReportWriter::escapeHtml(const std::string& text) {
    std::string result;
    result.reserve(text.size());
    for (char c : text) {
        switch (c) {
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '&': result += "&amp;"; break;
            default: result += c;
        }
    }
    return result;
}

std::string AddressTranslationReportWriter::formatHtmlReport(const MemoryTranslator& mmu,
                                                               const std::vector<std::string>& commandLog) {
    std::ostringstream html;

    html << "<!DOCTYPE html>\n<html lang=\"es\">\n<head>\n";
    html << "<meta charset=\"UTF-8\">\n<title>Reporte - Traduccion de Direcciones</title>\n";
    html << "<style>\n"
         << "body { font-family: Arial, sans-serif; margin: 20px; background:#fafafa; }\n"
         << "h1 { color:#2c3e50; }\n"
         << "table { border-collapse: collapse; width: 100%; margin-bottom: 24px; background:#fff; }\n"
         << "th, td { border: 1px solid #ccc; padding: 6px 10px; text-align: left; font-size: 14px; }\n"
         << "th { background-color: #34495e; color: #fff; }\n"
         << ".free { color: #27ae60; font-weight: bold; }\n"
         << ".occupied { color: #c0392b; font-weight: bold; }\n"
         << ".summary-box { background: #eef2f7; padding: 14px 18px; border-radius: 6px; margin-bottom: 24px; }\n"
         << ".summary-box p { margin: 4px 0; }\n"
         << "</style>\n</head>\n<body>\n";

    html << "<h1>Reporte de Traduccion de Direcciones (Paginacion de un nivel)</h1>\n";

    // --- Resumen: memoria virtual, memoria fisica, tamano de pagina, marcos disponibles ---
    html << "<div class=\"summary-box\">\n";
    html << "<p><b>Memoria virtual total:</b> " << mmu.getVirtualMemorySize()
         << " | <b>Usada:</b> " << mmu.getUsedVirtualMemory() << "</p>\n";
    html << "<p><b>Memoria fisica total:</b> " << mmu.getPhysicalMemorySize() << "</p>\n";
    html << "<p><b>Tamano de pagina:</b> " << mmu.getPageSize() << "</p>\n";
    html << "<p><b>Marcos disponibles:</b> " << mmu.getFreeFrameCount()
         << " / " << mmu.getFrames().size() << "</p>\n";
    html << "</div>\n";

    // --- Tabla de marcos (memoria fisica) ---
    html << "<h2>Marcos de memoria fisica</h2>\n<table>\n";
    html << "<tr><th>Marco</th><th>Estado</th><th>PID</th><th>Pagina</th></tr>\n";
    for (const Frame& f : mmu.getFrames()) {
        html << "<tr><td>" << f.getFrameNumber() << "</td>";
        if (f.isFree()) {
            html << "<td class=\"free\">Libre</td><td>-</td><td>-</td>";
        } else {
            html << "<td class=\"occupied\">Ocupado</td><td>" << f.getPid()
                 << "</td><td>" << f.getPageNumber() << "</td>";
        }
        html << "</tr>\n";
    }
    html << "</table>\n";

    // --- Tabla de paginas global ---
    html << "<h2>Tabla de paginas</h2>\n<table>\n";
    html << "<tr><th>PID</th><th>Pagina</th><th>Marco</th></tr>\n";
    for (const auto& entry : mmu.getPageTable().getEntries()) {
        html << "<tr><td>" << entry.pid << "</td><td>" << entry.pageNumber
             << "</td><td>" << entry.frameNumber << "</td></tr>\n";
    }
    html << "</table>\n";

    // --- Procesos ---
    html << "<h2>Procesos</h2>\n<table>\n";
    html << "<tr><th>PID</th><th>Tamano virtual</th><th>Paginas</th><th>Asignado</th></tr>\n";
    for (const ProcessP& p : mmu.getProcesses()) {
        html << "<tr><td>" << p.getPid() << "</td><td>" << p.getVirtualSize()
             << "</td><td>" << p.getPages().size() << "</td><td>"
             << (p.isAllocated() ? "Si" : "No") << "</td></tr>\n";
    }
    html << "</table>\n";

    // --- Registro de comandos ---
    html << "<h2>Registro de comandos</h2>\n<table>\n";
    html << "<tr><th>#</th><th>Resultado</th></tr>\n";
    for (size_t i = 0; i < commandLog.size(); i++) {
        html << "<tr><td>" << (i + 1) << "</td><td>" << escapeHtml(commandLog[i]) << "</td></tr>\n";
    }
    html << "</table>\n";

    html << "</body>\n</html>\n";
    return html.str();
}

bool AddressTranslationReportWriter::writeHtmlReport(const MemoryTranslator& mmu,
                                                       const std::vector<std::string>& commandLog,
                                                       const std::string& outputPath) {
    std::ofstream out(outputPath);
    if (!out.is_open()) {
        return false;
    }
    out << formatHtmlReport(mmu, commandLog);
    return true;
}

std::string AddressTranslationReportWriter::formatTextReport(const MemoryTranslator& mmu,
                                                               const std::vector<std::string>& commandLog) {
    std::ostringstream out;

    out << "===== Reporte de Traduccion de Direcciones (Paginacion de un nivel) =====\n\n";

    out << "Memoria virtual total : " << mmu.getVirtualMemorySize()
        << " | Usada: " << mmu.getUsedVirtualMemory() << "\n";
    out << "Memoria fisica total  : " << mmu.getPhysicalMemorySize() << "\n";
    out << "Tamano de pagina      : " << mmu.getPageSize() << "\n";
    out << "Marcos disponibles    : " << mmu.getFreeFrameCount()
        << " / " << mmu.getFrames().size() << "\n\n";

    out << "--- Marcos de memoria fisica ---\n";
    for (const Frame& f : mmu.getFrames()) {
        out << "Marco " << f.getFrameNumber() << ": ";
        if (f.isFree()) {
            out << "libre\n";
        } else {
            out << "ocupado por PID=" << f.getPid()
                << " (pagina " << f.getPageNumber() << ")\n";
        }
    }

    out << "\n--- Tabla de paginas ---\n";
    for (const auto& entry : mmu.getPageTable().getEntries()) {
        out << "PID=" << entry.pid << " | Pagina=" << entry.pageNumber
            << " | Marco=" << entry.frameNumber << "\n";
    }

    out << "\n--- Procesos ---\n";
    for (const ProcessP& p : mmu.getProcesses()) {
        out << "PID=" << p.getPid() << " | Tamano virtual=" << p.getVirtualSize()
            << " | Paginas=" << p.getPages().size()
            << " | Asignado=" << (p.isAllocated() ? "Si" : "No") << "\n";
    }

    out << "\n--- Registro de comandos ---\n";
    for (size_t i = 0; i < commandLog.size(); i++) {
        out << (i + 1) << ". " << commandLog[i] << "\n";
    }

    return out.str();
}

bool AddressTranslationReportWriter::writeTextReport(const MemoryTranslator& mmu,
                                                       const std::vector<std::string>& commandLog,
                                                       const std::string& outputPath) {
    std::ofstream out(outputPath);
    if (!out.is_open()) {
        return false;
    }
    out << formatTextReport(mmu, commandLog);
    return true;
}
