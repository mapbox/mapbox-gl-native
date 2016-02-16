#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>

#include "../fixtures/util.hpp"

using namespace mbgl::util;

TEST(RunLoop, Stop) {
    RunLoop loop(RunLoop::Type::New);

    Timer timer;
    timer.start(mbgl::Duration::zero(), mbgl::Duration::zero(), [&] {
        loop.stop();
    });

    loop.run();
}

TEST(RunLoop, MultipleStop) {
    RunLoop loop(RunLoop::Type::New);

    Timer timer;
    timer.start(mbgl::Duration::zero(), mbgl::Duration::zero(), [&] {
        loop.stop();
        loop.stop();
        loop.stop();
        loop.stop();
    });

    loop.run();
}
