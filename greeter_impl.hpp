#pragma once
      
#include <string>
      
class GreeterImpl {
public:
    GreeterImpl(std::string greeting_)
        : greetingString(std::move(greeting_)) {
    }
      
    std::string greeting(const std::string& name) {
        return greetingString + ", " + name + "!";
    }
      
private:
    std::string greetingString;
};
