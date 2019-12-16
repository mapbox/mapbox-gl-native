//
//  ios_test_runner.hpp
//  RenderTestAPP
//
//  Created by Miao Zhao on 12/11/19.
//

#ifndef ios_test_runner_hpp
#define ios_test_runner_hpp

#include <string>

class TestRunner {
public:
    TestRunner()=default;
    ~TestRunner() =default;

    void startTest(const std::string& manifest);
};

#endif /* ios_test_runner_hpp */
