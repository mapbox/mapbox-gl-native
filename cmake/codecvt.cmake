add_library(codecvt INTERFACE)

# Determine if the STL has codecvt
file(WRITE "${CMAKE_BINARY_DIR}/features/codecvt/main.cpp" "#include <codecvt>\nint main() {}")
try_compile(STL_SUPPORTS_CODECVT
    "${CMAKE_BINARY_DIR}/features/codecvt"
    SOURCES "${CMAKE_BINARY_DIR}/features/codecvt/main.cpp"
    CMAKE_FLAGS "-DCMAKE_MACOSX_BUNDLE:STRING=YES" "-DCMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED:STRING=NO"
    COMPILE_DEFINITIONS "-std=c++14" # CXX_STANDARD wasn't added to try_compile until CMake 3.8
    OUTPUT_VARIABLE CODECVT_TEST_OUTPUT
)

if (NOT STL_SUPPORTS_CODECVT)
    if ($ENV{V})
        message("codecvt support not detected: ${CODECVT_TEST_OUTPUT}")
    endif()
    target_include_directories(codecvt INTERFACE platform/default/codecvt)
    target_add_mason_package(codecvt INTERFACE boost)
endif()
