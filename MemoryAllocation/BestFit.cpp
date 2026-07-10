#include "BestFit.h"
#include <climits>

int BestFit::findBlock(const std::vector<MemoryBlock>& blocks, const ProcessA& process) {
    int bestIndex = -1;
    int bestSize = INT_MAX;   // Vamos guardando el bloque más chico que sirva hasta ahora

    for (size_t i = 0; i < blocks.size(); i++) {
        if (blocks[i].isFree() && blocks[i].getSize() >= process.getRequestedSize()) {
            if (blocks[i].getSize() < bestSize) {
                bestSize = blocks[i].getSize();
                bestIndex = static_cast<int>(i);
            }
        }
    }
    return bestIndex;
}

std::string BestFit::getName() const {
    return "Best Fit";
}
