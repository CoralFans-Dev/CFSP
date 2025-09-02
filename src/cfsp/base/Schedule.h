#include "TimeWheel.h"
#include <memory>

namespace coral_fans::cfsp::base {

class Schedule {
private:
    std::shared_ptr<timewheel::TimeWheel> mScheduler;

private:
    Schedule() { mScheduler = std::make_shared<timewheel::TimeWheel>(1200); };

public:
    static Schedule& getInstance() {
        static Schedule instance;
        return instance;
    }

public:
    inline std::shared_ptr<timewheel::TimeWheel> getSchedule() { return this->mScheduler; }
};
} // namespace coral_fans::cfsp::base