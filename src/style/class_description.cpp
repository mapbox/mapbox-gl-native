#include <llmr/style/class_description.hpp>

namespace llmr {

void ClassDescription::addClass(const std::string &name, ClassProperties &&properties) {
    styles.emplace(name, std::move(properties));
}

}
