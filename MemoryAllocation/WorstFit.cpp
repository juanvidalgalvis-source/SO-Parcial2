#include "WorstFit.h"

int WorstFit::findBlock(const std::vector<MemoryBlock>& blocks, const ProcessA& process) {
    int worstIndex = -1;
    int worstSize = -1;   // Vamos guardando el bloque más grande que sirva hasta ahora
    for (size_t i = 0; i < blocks.size(); i++) {
        if (blocks[i].isFree() && blocks[i].getSize() >= process.getRequestedSize()) {
            if (blocks[i].getSize() > worstSize) {
                worstSize = blocks[i].getSize();
                worstIndex = static_cast<int>(i);
            }
        }
    }
    return worstIndex;
}

std::string WorstFit::getName() const {
    return "Worst Fit";
}
