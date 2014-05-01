#ifndef LLMR_UTIL_TIMER
#define LLMR_UTIL_TIMER

#include <string>

namespace llmr {
namespace util {

#ifndef DISABLE_TIMER
class timer {
public:
    timer();
    timer(const std::string &name);
    void report(const std::string &name);
    ~timer();

private:
    const std::string name;
    uint64_t start;
};
#else
class timer {
    inline timer() {}
    inline timer(const std::string &name) {}
    inline void report (const std::string &name) {}
    inline ~timer() {}
};
#endif

}
}

#endif
