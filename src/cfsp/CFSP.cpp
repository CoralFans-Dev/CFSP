#include "cfsp/CFSP.h"
#include "cfsp/base/Mod.h"
#include "cfsp/commands/Command.h"
#include "cfsp/fix/FeatureFix.h"
#include "cfsp/simplayer/CFSP.h"
#include "cfsp/simplayer/Hooks.h"
#include "ll/api/Config.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/mod/RegisterHelper.h"


namespace coral_fans::cfsp {

CFSP& CFSP::getInstance() {
    static CFSP instance;
    return instance;
}

bool CFSP::load() {
    const auto& logger = getSelf().getLogger();
    auto&       mod    = coral_fans::cfsp::mod();

    // load config
    try {
        const auto& configFilePath = getSelf().getConfigDir() / "config.json";
        if (!ll::config::loadConfig(mod.getConfig(), configFilePath)) {
            logger.warn("Cannot load configurations from {}", configFilePath);
            logger.info("Saving default configurations");
            if (!ll::config::saveConfig(mod.getConfig(), configFilePath)) {
                logger.error("Cannot save default configurations to {}", configFilePath);
                return false;
            }
        }
    } catch (...) {
        logger.error("Failed to load config.json. Please check the file!");
        return false;
    }

    // load i18n
    logger.debug("Loading I18n");
    if (!ll::i18n::getInstance().load(getSelf().getLangDir())) logger.error("Failed to load I18n");
    return true;
}

bool CFSP::enable() {
    auto& mod = coral_fans::cfsp::mod();

    hookSimPlayer(true);
    fix::featureFix(true);

    if (mod.getConfig().command.sp.enabled) commands::registerSpCommand(mod.getConfig().command.sp.permission);

    // load simplayer data
    SimPlayerManager::getInstance().load();

    return true;
}

bool CFSP::disable() {
    hookSimPlayer(false);
    fix::featureFix(false);
    return true;
}

} // namespace coral_fans::cfsp

LL_REGISTER_MOD(coral_fans::cfsp::CFSP, coral_fans::cfsp::CFSP::getInstance());
