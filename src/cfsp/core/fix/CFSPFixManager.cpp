#include "CFSPFixManager.h"


namespace coral_fans::cfsp::fix {
void CFSPFixManager::cfspBugFixHook(bool enable) {
    this->handItemFix(enable);
    this->sapiFix(enable);
}
} // namespace coral_fans::cfsp::fix