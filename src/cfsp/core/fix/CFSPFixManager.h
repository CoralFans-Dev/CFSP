namespace coral_fans::cfsp::fix {
class CFSPFixManager {
public:
    static CFSPFixManager& getInstance() {
        static CFSPFixManager instance;
        return instance;
    }

public:
    void featureFix();
};
} // namespace coral_fans::cfsp::fix