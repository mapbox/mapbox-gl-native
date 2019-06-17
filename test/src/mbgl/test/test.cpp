#include <mbgl/actor/scheduler.hpp>
#include <mbgl/gfx/backend.hpp>
#include <mbgl/test.hpp>
#include <mbgl/test/util.hpp>

#include <gtest/gtest.h>

#include <args.hxx>

#include <cstdlib>

namespace mbgl {

int runTests(int argc, char *argv[]) {
#if TEST_HAS_SERVER
    auto server = std::make_unique<test::Server>("test/storage/server.js");
#endif

    testing::InitGoogleTest(&argc, argv);

    args::ArgumentParser argumentParser("Unit tests");
    args::ValueFlag<std::string> backendValue(argumentParser, "Backend", "Rendering backend", {"backend"});

    argumentParser.ParseCLI(argc, argv);

    if (backendValue) {
        mbgl::gfx::Backend::SetType(args::get(backendValue));
    } else if (const char* backendEnv = std::getenv("MAPBOX_RENDER_BACKEND")) {
        mbgl::gfx::Backend::SetType(backendEnv);
    }

    return RUN_ALL_TESTS();
}

} // namespace mbgl
