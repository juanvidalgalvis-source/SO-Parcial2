#ifndef WORSTFIT_H
#define WORSTFIT_H

#include "AllocationStrategy.h"

// Elige el bloque libre MÁS GRANDE disponible.
class WorstFit : public AllocationStrategy {
public:
    int findBlock(const std::vector<MemoryBlock>& blocks, const ProcessA& process) override;
    std::string getName() const override;
};

#endif
