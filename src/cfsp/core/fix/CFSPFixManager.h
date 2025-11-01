#pragma once


namespace coral_fans::cfsp::fix {
class CFSPFixManager {
public:
    static CFSPFixManager& getInstance() {
        static CFSPFixManager instance;
        return instance;
    }

private:
    void featureFix();
    void handItemFix();

public:
    void cfspBugFixHook();
};
} // namespace coral_fans::cfsp::fix