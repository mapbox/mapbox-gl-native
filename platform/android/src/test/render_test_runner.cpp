#include "runtime.hpp"
#include <mbgl/render_test_runner.hpp>

int main(int argc, char *argv[]) {
    if (!mbgl::android::initRuntime(argc, argv)) {
        return 1;
    }

    return mbgl::runRenderTests(argc, argv);
}
