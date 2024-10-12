#pragma once

#include "cfsp/Config.h"
#include "mc/dataloadhelper/DefaultDataLoadHelper.h"


namespace coral_fans::cfsp {

class CFSPMod {
private:
    config::Config         mConfig;
    DefaultDataLoadHelper* mDefaultDataLoadHelper;

public:
    inline config::Config&         getConfig() { return this->mConfig; }
    inline DefaultDataLoadHelper*& getDefaultDataLoadHelper() { return this->mDefaultDataLoadHelper; }
};

CFSPMod& mod();

} // namespace coral_fans::cfsp