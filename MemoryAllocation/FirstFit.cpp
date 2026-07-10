#include "FirstFit.h"

int FirstFit::findBlock(const std::vector<MemoryBlock>& blocks, const ProcessA& process) {
    for (size_t i = 0; i < blocks.size(); i++) {
        if (blocks[i].isFree() && blocks[i].getSize() >= process.getRequestedSize()) {
            return static_cast<int>(i);   // Se queda con el PRIMERO que sirva
        }
    }
    return -1;   // No hay ningún bloque que alcance
}

std::string FirstFit::getName() const {
    return "First Fit";
}
