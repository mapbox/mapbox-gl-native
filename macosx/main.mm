
#include "settings.hpp"
#include "../common/glfw_view.hpp"
#include "settings.hpp"

#include <cstdio>

int main() {
    llmr::Settings_MacOSX settings;
    llmr::Map map(settings);
    llmr::GLFWView view(map);
    view.show();
    settings.sync();
    return 0;
}

