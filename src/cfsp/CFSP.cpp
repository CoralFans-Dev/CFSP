#include "cfsp/CFSP.h"
#include "cfsp/core/manager/CFSPManager.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/mod/RegisterHelper.h"

namespace coral_fans::cfsp {

CFSP& CFSP::getInstance() {
    static CFSP instance;
    return instance;
}

bool CFSP::load() {
    if (!manager::CFSPManager::getInstance().init()) return false;
    // load i18n
    if (!ll::i18n::getInstance().load(getSelf().getLangDir())) getSelf().getLogger().error("Failed to load I18n");
    return true;
}

bool CFSP::enable() {
    manager::CFSPManager::getInstance().load();
    return true;
}

bool CFSP::disable() { return true; }

} // namespace coral_fans::cfsp

LL_REGISTER_MOD(coral_fans::cfsp::CFSP, coral_fans::cfsp::CFSP::getInstance());
