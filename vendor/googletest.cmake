add_library(googletest STATIC
    ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest/src/gtest-all.cc
    ${CMAKE_SOURCE_DIR}/vendor/googletest/googlemock/src/gmock-all.cc
)

target_include_directories(googletest PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest
    ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest/include
    ${CMAKE_SOURCE_DIR}/vendor/googletest/googlemock
    ${CMAKE_SOURCE_DIR}/vendor/googletest/googlemock/include
)

target_include_directories(googletest SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest/include
    ${CMAKE_SOURCE_DIR}/vendor/googletest/googlemock/include
)
