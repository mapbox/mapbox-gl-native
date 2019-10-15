#include <mbgl/render_test.hpp>
#include "runtime.hpp"

#include <string>

int main(int argc, char *argv[]) {
    if (!mbgl::android::initRuntime(argc, argv)) {
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        const std::string arg{argv[i]};
        std::string kClassPathCommand("--class_path=");
        if (arg.compare(0, kClassPathCommand.length(), kClassPathCommand) == 0) {
            // Remove this item from the list
            argv[i][0] = 0;
        }
    }

    return mbgl::runRenderTests(argc, argv);
}
