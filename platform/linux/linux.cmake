find_package(CURL REQUIRED)
find_package(ICU REQUIRED i18n uc)
find_package(JPEG REQUIRED)
find_package(OpenGL REQUIRED GLX)
find_package(PNG REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(X11 REQUIRED)

pkg_search_module(LIBUV libuv REQUIRED)

include(${PROJECT_SOURCE_DIR}/platform/gfx/gl/gl.cmake)

target_sources(mbgl-core PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/gl_functions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/headless_backend_glx.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gfx/headless_backend.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gfx/headless_frontend.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gl/headless_backend.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/layermanager/layer_manager.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/asset_file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/default_file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/file_source_request.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/http_file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/local_file_request.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/local_file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/offline.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/offline_database.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/offline_download.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/online_file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/sqlite3.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/text/bidi.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/text/collator.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/text/local_glyph_rasterizer.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/text/unaccent.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/async_task.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/format_number.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/image.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/jpeg_reader.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/logging_stderr.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/png_reader.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/png_writer.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/run_loop.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/string_stdlib.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/thread.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/thread_local.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/timer.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/utf.cpp
)

# FIXME: Should not be needed, but now needed by
# node because of the headless frontend.
target_include_directories(mbgl-core PUBLIC
    ${PROJECT_SOURCE_DIR}/platform/default/include
)

target_include_directories(mbgl-core PRIVATE
    ${CURL_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
    ${GLES_INCLUDE_DIRS}
    ${JPEG_INCLUDE_DIRS}
    ${LIBUV_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
)

include(${PROJECT_SOURCE_DIR}/vendor/nunicode.cmake)
include(${PROJECT_SOURCE_DIR}/vendor/sqlite.cmake)

target_link_libraries(mbgl-core PRIVATE
    ${CURL_LIBRARIES}
    ${EGL_LIBRARIES}
    ${GLES_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${LIBUV_LIBRARIES}
    ${X11_LIBRARIES}
    ICU::i18n
    ICU::uc
    PNG::PNG
    OpenGL::GL
    mbgl-vendor-nunicode
    mbgl-vendor-sqlite
)

add_custom_target(mbgl-ca-bundle)
add_dependencies(mbgl-core mbgl-ca-bundle)

add_custom_command(TARGET mbgl-ca-bundle PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/misc/ca-bundle.crt ${CMAKE_BINARY_DIR}
)

add_subdirectory(${PROJECT_SOURCE_DIR}/bin)
add_subdirectory(${PROJECT_SOURCE_DIR}/expression-test)
add_subdirectory(${PROJECT_SOURCE_DIR}/platform/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/platform/node)
add_subdirectory(${PROJECT_SOURCE_DIR}/render-test)

add_executable(mbgl-test-runner
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/test/main.cpp
)

target_compile_definitions(mbgl-test-runner PRIVATE
    WORK_DIRECTORY=${PROJECT_SOURCE_DIR}
)

target_link_libraries(mbgl-test-runner PRIVATE
    mbgl-test
)

add_executable(mbgl-benchmark-runner
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/benchmark/main.cpp
)

target_link_libraries(mbgl-benchmark-runner PRIVATE
    mbgl-benchmark
)

add_test(NAME mbgl-benchmark-runner COMMAND mbgl-benchmark-runner WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_test(NAME mbgl-test-runner COMMAND mbgl-test-runner WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
