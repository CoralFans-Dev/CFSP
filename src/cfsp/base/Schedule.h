#include "TimeWheel.h"
#include <memory>

namespace coral_fans::cfsp::base {
class Schedule {
private:
    std::shared_ptr<timewheel::TimeWheel> mScheduler;

public:
    static Schedule& getInstance() {
        static Schedule instance;
        return instance;
    }

public:
    inline std::shared_ptr<timewheel::TimeWheel> getSchedule() { return this->mScheduler; }
};
} // namespace coral_fans::cfsp::base