#include "CFSPFixManager.h"


namespace coral_fans::cfsp::fix {
void CFSPFixManager::cfspBugFixHook() {
    this->featureFix();
    this->handItemFix();
    this->sapiFix();
}
} // namespace coral_fans::cfsp::fix