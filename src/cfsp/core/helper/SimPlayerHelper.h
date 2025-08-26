

namespace coral_fans::cfsp::helper {
class SimPlayerHelperManager {
public:
    static SimPlayerHelperManager& getInstance();

private:
    void SimPlayerSaveHook();

public:
    void SimPlayerHelperHook();
};
} // namespace coral_fans::cfsp::helper