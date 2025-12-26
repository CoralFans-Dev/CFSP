#pragma once


namespace coral_fans::cfsp::fix {
class CFSPFixManager {
public:
    bool createSpMutex = false;

public:
    static CFSPFixManager& getInstance() {
        static CFSPFixManager instance;
        return instance;
    }

private:
    void featureFix();
    void handItemFix();
    void sapiFix();

public:
    void cfspBugFixHook();
};
} // namespace coral_fans::cfsp::fix