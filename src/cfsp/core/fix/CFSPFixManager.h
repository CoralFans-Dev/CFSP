#pragma once

#include "mc/world/level/Tick.h"
#include <string>
#include <unordered_set>


namespace coral_fans::cfsp::fix {
class CFSPRemovingListRecorder {
public:
    std::unordered_set<std::string> mRemovingSpList;
    Tick                            mLastTick;
};

class CFSPFixManager {
public:
    bool createSpMutex = false;

public:
    static CFSPFixManager& getInstance() {
        static CFSPFixManager instance;
        return instance;
    }

private:
    void handItemFix(bool);
    void sapiFix(bool);

public:
    CFSPRemovingListRecorder mRemovingRecord;

public:
    void cfspBugFixHook(bool);
};
} // namespace coral_fans::cfsp::fix