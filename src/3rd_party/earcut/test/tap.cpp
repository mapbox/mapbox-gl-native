#include "tap.hpp"

#include <iostream>
#include <stdexcept>

int Tap::total = 0;
int Tap::errored = 0;
bool Tap::started = false;

Tap::Tap() {
    if (started) {
        throw std::runtime_error("Tap cannot be initialized more than once");
    }

    std::cout << "TAP version 13" << std::endl;

    atexit([]() {

    });
}

Tap::~Tap() {
    std::cout << std::endl;
    std::cout << "1.." << total << std::endl;
    std::cout << "# tests " << total << std::endl;
    std::cout << "# pass  " << (total - errored) << std::endl;
    std::cout << std::endl;
    if (!errored) {
        std::cout << "# ok" << std::endl << std::endl;
    } else {
        std::cout << "# not ok" << std::endl << std::endl;
        exit(1);
    }
}

Tap::Test::Test(const std::string &name) {
    std::cout << "# " << name << std::endl;
}

Tap::Test::~Test() {
    if (!finished) {
        fail("test exited without ending");
    }
    if (failed) {
        errored++;
    }
}

void Tap::Test::ok(bool status, const std::string &message) {
    if (!status) {
        fail(message);
    } else {
        std::cout << "ok " << ++total << " " << message << std::endl;
    }
}

void Tap::Test::fail(const std::string &message) {
    failed = true;
    std::cout << "not ok " << ++total << " " << message << std::endl;
}

void Tap::Test::end() {
    finished = true;
}
