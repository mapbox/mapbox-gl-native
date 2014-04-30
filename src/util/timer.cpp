#ifndef DISABLE_TIMER
#include <llmr/util/timer.hpp>
#include <llmr/util/time.hpp>

#include <iostream>
#include <atomic>

using namespace llmr::util;

std::atomic<int> indent;

timer::timer() : start(now()) {
    indent++;
}

timer::timer(const std::string &name) : name(name), start(now()) {
    indent++;
}

void timer::report(const std::string &name) {
    time duration = now() - start;
    std::cerr << std::string((indent - 1) * 4, ' ')
              << name << ": "
              << (double)(duration) / 1_millisecond
              << "ms" << std::endl;
    start += duration;
}

timer::~timer() {
    if (name.size()) {
        report(name);
    }
    indent--;
}
#endif
