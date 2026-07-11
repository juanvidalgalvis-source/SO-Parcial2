#ifndef ADDRESSTRANSLATIONREPORTWRITER_H
#define ADDRESSTRANSLATIONREPORTWRITER_H

#include <string>
#include <vector>
#include "MemoryTranslator.h"

// Genera los reportes (HTML y TXT) del simulador de la Implementación 2.
// Muestra memoria virtual, memoria física, tamaño de página, tabla de
// páginas, marcos disponibles y el registro de resultados de cada
// comando ejecutado (mismo espíritu que AllocationReportWriter en la
// Implementación 1).
class AddressTranslationReportWriter {
public:
    // Arma el contenido HTML completo a partir del estado actual del MMU
    // y el registro de resultados de cada comando ejecutado (por ejemplo:
    // "CREAR,1,12 -> OK", "TRADUCIR,1,20 -> ERROR: fuera de rango").
    static std::string formatHtmlReport(const MemoryTranslator& mmu,
                                         const std::vector<std::string>& commandLog);

    // Escribe el reporte HTML en outputPath. Devuelve true si se pudo escribir.
    static bool writeHtmlReport(const MemoryTranslator& mmu,
                                 const std::vector<std::string>& commandLog,
                                 const std::string& outputPath);

    // Arma el contenido del reporte de texto plano.
    static std::string formatTextReport(const MemoryTranslator& mmu,
                                         const std::vector<std::string>& commandLog);

    // Escribe el reporte de texto en outputPath. Devuelve true si se pudo escribir.
    static bool writeTextReport(const MemoryTranslator& mmu,
                                 const std::vector<std::string>& commandLog,
                                 const std::string& outputPath);

private:
    // Escapa caracteres especiales de HTML (<, >, &) para no romper el marcado.
    static std::string escapeHtml(const std::string& text);
};

#endif // ADDRESSTRANSLATIONREPORTWRITER_H
