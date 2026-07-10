#ifndef FIRSTFIT_H
#define FIRSTFIT_H

#include "AllocationStrategy.h"

// Elige el PRIMER bloque libre que sea suficientemente grande.
class FirstFit : public AllocationStrategy {
public:
    int findBlock(const std::vector<MemoryBlock>& blocks, const ProcessA& process) override;
    std::string getName() const override;
};

#endif
