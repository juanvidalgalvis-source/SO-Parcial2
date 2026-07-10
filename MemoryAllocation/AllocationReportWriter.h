#ifndef ALLOCATIONREPORTWRITER_H
#define ALLOCATIONREPORTWRITER_H

#include <string>
#include "MemoryManager.h"

class AllocationReportWriter {
public:
    static std::string formatMemoryState(const MemoryManager& manager);
    static std::string formatProcessReport(const MemoryManager& manager);
    static std::string formatHtmlReport(const MemoryManager& manager, const std::string& commandLog = "");
    static std::string formatTextReport(const MemoryManager& manager, const std::string& commandLog = "");
    static bool writeHtmlReport(const MemoryManager& manager, const std::string& filename, const std::string& commandLog = "");
    static bool writeTextReport(const MemoryManager& manager, const std::string& filename, const std::string& commandLog = "");
};

#endif // ALLOCATIONREPORTWRITER_H
