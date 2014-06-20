#ifndef LLMR_STYLE_CLASS_DESCRIPTION
#define LLMR_STYLE_CLASS_DESCRIPTION

#include <string>
#include <unordered_map>
#include "properties.hpp"

namespace llmr {

class ClassDescription {
public:
    inline const ClassProperties &operator[](const std::string &name) const {
        auto it = styles.find(name);
        if (it != styles.end()) {
            return it->second;
        } else {
            return defaultProperties;
        }
    }

    void addClass(const std::string &name, ClassProperties &&properties);

    inline std::unordered_map<std::string, ClassProperties>::const_iterator begin() const {
        return styles.begin();
    }

    inline std::unordered_map<std::string, ClassProperties>::const_iterator end() const {
        return styles.end();
    }

private:
    std::unordered_map<std::string, ClassProperties> styles;
    static const ClassProperties defaultProperties;
};


}

#endif
