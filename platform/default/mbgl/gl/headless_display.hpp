#pragma once

#include <memory>

namespace mbgl {

class HeadlessDisplay {
public:
    static std::shared_ptr<HeadlessDisplay> create() {
        static std::weak_ptr<HeadlessDisplay> instance;
        
        auto shared = instance.lock();
        
        if (!shared) {
            instance = shared = std::shared_ptr<HeadlessDisplay>(new HeadlessDisplay());
        }
        
        return shared;
    }

    
    ~HeadlessDisplay();

    template <typename DisplayAttribute>
    DisplayAttribute attribute() const;

private:
    HeadlessDisplay();
    
    class Impl;
    std::unique_ptr<Impl>  impl;
};

} // namespace mbgl
