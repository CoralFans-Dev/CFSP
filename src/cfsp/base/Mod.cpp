#include "cfsp/base/Mod.h"

namespace coral_fans::cfsp {

CFSPMod& mod() {
    static CFSPMod mod;
    return mod;
}

} // namespace coral_fans::cfsp