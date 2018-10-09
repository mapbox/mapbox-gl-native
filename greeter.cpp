#include "greeter.hpp"
#include "greeter_impl.hpp"

#include <utility>

Greeter::Greeter(std::string greeting)
: impl(std::make_unique<GreeterImpl>(std::move(greeting)))
{}

Greeter::~Greeter() = default;

std::string Greeter::greeting(const std::string& name) {
    return impl->greeting(name);
}
