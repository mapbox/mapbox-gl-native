#ifndef ios_test_runner_hpp
#define ios_test_runner_hpp

#include <string>

class TestRunner {
public:
    TestRunner() = default;
    ~TestRunner() = default;

    bool startTest(const std::string& basePath);
};

#endif /* ios_test_runner_hpp */
