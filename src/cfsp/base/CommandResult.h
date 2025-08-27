#include <string>
namespace coral_fans::cfsp::base {
class CommandResult {
public:
    enum class Type : int {
        none    = 0,
        success = 1,
        warn    = 2,
        error   = 3,
    };

public:
    Type        mType;
    std::string info;

public:
    CommandResult(Type mType, std::string info) : mType(mType), info(info) {}
    CommandResult() { mType = Type::none; }
};
} // namespace coral_fans::cfsp::base