#pragma once

#include <mapbox/bindgen/peer.hpp>
#include <memory>
#include <string>

class GreeterImpl;

class Greeter {
public:
    Greeter(std::string greeting);
    ~Greeter();

    std::string greeting(const std::string& name);

private:
    std::unique_ptr<GreeterImpl> impl;

public:
    // For use only by generated bindings.
    mapbox::bindgen::peer peer;
};
