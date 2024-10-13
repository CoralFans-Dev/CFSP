#pragma once

#include "cfsp/Config.h"

namespace coral_fans::cfsp {

class CFSPMod {
private:
    config::Config mConfig;

public:
    inline config::Config& getConfig() { return this->mConfig; }

private:
    CFSPMod() {}
    ~CFSPMod() {}
    CFSPMod(const CFSPMod&);
    CFSPMod& operator=(const CFSPMod&);

public:
    friend CFSPMod& mod();
    friend class CFSP;
};

CFSPMod& mod();

} // namespace coral_fans::cfsp