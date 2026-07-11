#include "PageTable.h"
#include <iostream>
#include <algorithm>

PageTable::PageTable() {
}

void PageTable::mapPage(int pid, int pageNumber, int frameNumber) {
    Entry entry;
    entry.pid = pid;
    entry.pageNumber = pageNumber;
    entry.frameNumber = frameNumber;
    entry.valid = true;
    entries.push_back(entry);
}

void PageTable::unmapProcess(int pid) {
    // Elimina (erase-remove) todas las entradas del proceso pid.
    entries.erase(
        std::remove_if(entries.begin(), entries.end(),
            [pid](const Entry& e) { return e.pid == pid; }),
        entries.end()
    );
}

int PageTable::lookup(int pid, int pageNumber) const {
    for (const Entry& e : entries) {
        if (e.pid == pid && e.pageNumber == pageNumber && e.valid) {
            return e.frameNumber;
        }
    }
    return -1;
}

const std::vector<PageTable::Entry>& PageTable::getEntries() const {
    return entries;
}

void PageTable::print() const {
    std::cout << "--- Tabla de paginas global ---" << std::endl;
    for (const Entry& e : entries) {
        std::cout << "PID=" << e.pid
                  << " | Pagina=" << e.pageNumber
                  << " | Marco=" << e.frameNumber << std::endl;
    }
}
