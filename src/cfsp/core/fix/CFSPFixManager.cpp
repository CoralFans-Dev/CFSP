#include "CFSPFixManager.h"


namespace coral_fans::cfsp::fix {
void CFSPFixManager::cfspBugFixHook() {
    this->featureFix();
    this->handItemFix();
}
} // namespace coral_fans::cfsp::fix