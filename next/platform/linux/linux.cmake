find_package(CURL REQUIRED)
find_package(ICU REQUIRED i18n)
find_package(ICU REQUIRED uc)
find_package(JPEG REQUIRED)
find_package(OpenGL REQUIRED GLX)
find_package(PNG REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(X11 REQUIRED)

pkg_search_module(LIBUV libuv REQUIRED)

target_sources(
    mbgl-core
    PRIVATE
        ${MBGL_ROOT}/platform/default/src/mbgl/gfx/headless_backend.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/gfx/headless_frontend.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/gl/headless_backend.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/i18n/collator.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/i18n/number_format.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/layermanager/layer_manager.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/asset_file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/default_file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/file_source_request.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/http_file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/local_file_request.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/local_file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/offline.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/offline_database.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/offline_download.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/online_file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/sqlite3.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/text/bidi.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/text/local_glyph_rasterizer.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/async_task.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/compression.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/image.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/jpeg_reader.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/logging_stderr.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/monotonic_timer.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/png_reader.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/png_writer.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/run_loop.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/string_stdlib.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/thread.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/thread_local.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/timer.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/utf.cpp
        ${MBGL_ROOT}/platform/linux/src/gl_functions.cpp
        ${MBGL_ROOT}/platform/linux/src/headless_backend_glx.cpp
)

# FIXME: Should not be needed, but now needed by node because of the headless frontend.
target_include_directories(
    mbgl-core
    PUBLIC ${MBGL_ROOT}/platform/default/include
    PRIVATE
        ${CURL_INCLUDE_DIRS}
        ${JPEG_INCLUDE_DIRS}
        ${LIBUV_INCLUDE_DIRS}
        ${X11_INCLUDE_DIRS}
)

include(${PROJECT_SOURCE_DIR}/vendor/nunicode.cmake)
include(${PROJECT_SOURCE_DIR}/vendor/sqlite.cmake)

target_link_libraries(
    mbgl-core
    PRIVATE
        ${CURL_LIBRARIES}
        ${JPEG_LIBRARIES}
        ${LIBUV_LIBRARIES}
        ${X11_LIBRARIES}
        ICU::i18n
        ICU::uc
        OpenGL::GLX
        PNG::PNG
        mbgl-vendor-nunicode
        mbgl-vendor-sqlite
)

add_subdirectory(${PROJECT_SOURCE_DIR}/bin)
add_subdirectory(${PROJECT_SOURCE_DIR}/expression-test)
add_subdirectory(${PROJECT_SOURCE_DIR}/platform/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/platform/node)

add_executable(
    mbgl-test-runner
    ${MBGL_ROOT}/platform/default/src/mbgl/test/main.cpp
)

target_compile_definitions(
    mbgl-test-runner
    PRIVATE WORK_DIRECTORY=${MBGL_ROOT}
)

target_link_libraries(
    mbgl-test-runner
    PRIVATE mbgl-test
)

add_executable(
    mbgl-benchmark-runner
    ${MBGL_ROOT}/platform/default/src/mbgl/benchmark/main.cpp
)

target_link_libraries(
    mbgl-benchmark-runner
    PRIVATE mbgl-benchmark
)

add_executable(
    mbgl-render-test-runner
    ${MBGL_ROOT}/platform/default/src/mbgl/render-test/main.cpp
)

target_link_libraries(
    mbgl-render-test-runner
    PRIVATE mbgl-render-test
)

add_test(NAME mbgl-benchmark-runner COMMAND mbgl-benchmark-runner WORKING_DIRECTORY ${MBGL_ROOT})
add_test(NAME mbgl-test-runner COMMAND mbgl-test-runner WORKING_DIRECTORY ${MBGL_ROOT})

string(RANDOM LENGTH 5 ALPHABET 0123456789 MBGL_RENDER_TEST_SEED)

add_test(
    NAME mbgl-render-test
    COMMAND
        mbgl-render-test-runner
        render-tests
        --recycle-map
        --shuffle
        --manifestPath=${MBGL_ROOT}/render-test/linux-manifest.json
        --seed=${MBGL_RENDER_TEST_SEED}
)

if(CMAKE_C_COMPILER MATCHES clang OR CMAKE_BUILD_TYPE MATCHES Debug)
    add_test(
        NAME mbgl-render-test-probe-unit-tests
        COMMAND mbgl-render-test-runner tests --manifestPath=${MBGL_ROOT}/render-test/linux-probe-manifest.json
    )
else()
    add_test(
        NAME mbgl-render-test-probe-gfx-network
        COMMAND mbgl-render-test-runner render-tests --manifestPath=${MBGL_ROOT}/render-test/linux-manifest-probe-gfx-network.json
    )
    add_test(
        NAME mbgl-render-test-probe-memory
        COMMAND mbgl-render-test-runner render-tests --manifestPath=${MBGL_ROOT}/render-test/linux-manifest-probe-memory.json
    )
endif()

add_test(NAME mbgl-query-test COMMAND mbgl-render-test-runner query-tests --manifestPath=${MBGL_ROOT}/render-test/linux-query-manifest.json)
