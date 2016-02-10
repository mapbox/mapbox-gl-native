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

TEST(RunLoop, UnrefShouldStop) {
    RunLoop loop(RunLoop::Type::New);

    Timer timer;
    timer.start(mbgl::Duration::zero(), mbgl::Duration::zero(), [&] {
        loop.unref();
    });

    loop.run();
}

TEST(RunLoop, RefUnref) {
    RunLoop loop(RunLoop::Type::New);

    Timer timer;
    auto zero = mbgl::Duration::zero();

    auto cb3 = [&] {
        loop.stop();
    };

    auto cb2 = [&] {
        loop.unref();
        loop.unref();

        loop.ref();

        timer.start(zero, zero, cb3);
    };

    auto cb1 = [&] {
        loop.ref();
        loop.ref();

        loop.unref();

        timer.start(zero, zero, cb2);
    };

    timer.start(zero, zero, cb1);

    loop.run();
}
