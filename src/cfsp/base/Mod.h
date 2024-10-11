#pragma once

#include "cfsp/Config.h"
#include "cfsp/simplayer/CFSP.h"
#include "mc/dataloadhelper/DefaultDataLoadHelper.h"


namespace coral_fans::cfsp {

class CFSPMod {
private:
    config::Config         mConfig;
    SimPlayerManager       mSimPlayerManager;
    DefaultDataLoadHelper* mDefaultDataLoadHelper;

public:
    inline config::Config&         getConfig() { return this->mConfig; }
    inline SimPlayerManager&       getSimPlayerManager() { return this->mSimPlayerManager; }
    inline DefaultDataLoadHelper*& getDefaultDataLoadHelper() { return this->mDefaultDataLoadHelper; }

public:
    inline void tick() { mSimPlayerManager.tick(); }

public:
    const std::string VERSION = "1.0.0";
};

CFSPMod& mod();

} // namespace coral_fans::cfsp