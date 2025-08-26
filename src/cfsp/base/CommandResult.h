#include <string>
namespace coral_fans::cfsp::base {
class CommandResult {
public:
    enum class Type : int {
        success = 0,
        warn    = 1,
        error   = 2,
    };

public:
    Type        mType;
    std::string info;

public:
    CommandResult(Type mType, std::string info) : mType(mType), info(info) {}
};
} // namespace coral_fans::cfsp::base