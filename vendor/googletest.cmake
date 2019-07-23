if(TARGET mbgl-vendor-googletest)
    return()
endif()

add_library(mbgl-vendor-googletest STATIC EXCLUDE_FROM_ALL
    ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest/src/gtest-all.cc
    ${CMAKE_SOURCE_DIR}/vendor/googletest/googlemock/src/gmock-all.cc
)

target_include_directories(mbgl-vendor-googletest PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest
    ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest/include
    ${CMAKE_SOURCE_DIR}/vendor/googletest/googlemock
    ${CMAKE_SOURCE_DIR}/vendor/googletest/googlemock/include
)

target_include_directories(mbgl-vendor-googletest SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest/include
    ${CMAKE_SOURCE_DIR}/vendor/googletest/googlemock/include
)

set_property(TARGET mbgl-vendor-googletest PROPERTY FOLDER Core)
