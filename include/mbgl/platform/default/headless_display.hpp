#pragma once

#include <memory>

namespace mbgl {

class HeadlessDisplay {
public:
    HeadlessDisplay();
    ~HeadlessDisplay();

    template <typename DisplayAttribute>
    DisplayAttribute attribute() const;

private:
    class Impl;
    std::unique_ptr<Impl>  impl;
};

} // namespace mbgl
