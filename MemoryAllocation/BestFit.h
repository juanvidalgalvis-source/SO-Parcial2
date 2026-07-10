#ifndef BESTFIT_H
#define BESTFIT_H

#include "AllocationStrategy.h"

// Elige el bloque libre MÁS PEQUEÑO que aún alcance (deja el menor desperdicio).
class BestFit : public AllocationStrategy {
public:
    int findBlock(const std::vector<MemoryBlock>& blocks, const ProcessA& process) override;
    std::string getName() const override;
};

#endif
