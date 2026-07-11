#include "ProcessP.h"
#include <iostream>

ProcessP::ProcessP(int pid, int virtualSize)
    : pid(pid), virtualSize(virtualSize), allocated(false) {
}

void ProcessP::calculateNumPages(int pageSize) {
    pages.clear();

    // Redondeo hacia arriba: (virtualSize + pageSize - 1) / pageSize
    int numPages = (virtualSize + pageSize - 1) / pageSize;

    for (int i = 0; i < numPages; i++) {
        pages.push_back(Page(i));
    }
}

void ProcessP::setAllocated(bool allocated) {
    this->allocated = allocated;
}

bool ProcessP::isAllocated() const {
    return allocated;
}

int ProcessP::getPid() const {
    return pid;
}

int ProcessP::getVirtualSize() const {
    return virtualSize;
}

std::vector<Page>& ProcessP::getPages() {
    return pages;
}

const std::vector<Page>& ProcessP::getPages() const {
    return pages;
}

void ProcessP::print() const {
    std::cout << "Proceso PID=" << pid
              << " | Tamano virtual=" << virtualSize
              << " | Paginas=" << pages.size()
              << " | Asignado=" << (allocated ? "Si" : "No") << std::endl;

    for (const Page& page : pages) {
        std::cout << "  Pagina " << page.getPageNumber() << ": ";
        if (page.isValid()) {
            std::cout << "valida -> marco " << page.getFrameNumber() << std::endl;
        } else {
            std::cout << "invalida (sin marco asignado)" << std::endl;
        }
    }
}
