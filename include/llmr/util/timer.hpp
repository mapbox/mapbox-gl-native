#ifndef LLMR_UTIL_TIMER
#define LLMR_UTIL_TIMER

#include <iostream>
#include <string>
#include <chrono>

namespace llmr {
namespace util {

class timer {
    using hr = std::chrono::high_resolution_clock;
public:
    explicit timer(const std::string &name) : name(name), start(hr::now()) {}
    ~timer() {
        using namespace std::chrono;
        std::cerr << name << ": "
                  << (double)duration_cast<microseconds>(hr::now() - start).count() / 1000
                  << "ms" << std::endl;
    }

private:
    const std::string name;
    const hr::time_point start;
};
}
}

#endif
