// #import <Foundation/Foundation.h>
// #import <AppKit/AppKit.h>

#include "settings.hpp"
#include "../common/glfw_view.hpp"

int main() {
    llmr::Settings_MacOSX settings;
    MapView view(settings);
    view.init();
    int ret = view.run();
    settings.sync();
    return ret;
}
