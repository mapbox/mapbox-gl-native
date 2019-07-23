if(TARGET mbgl-vendor-googletest)
    return()
endif()

add_library(mbgl-vendor-googletest STATIC EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_LIST_DIR}/googletest/googletest/src/gtest-all.cc
    ${CMAKE_CURRENT_LIST_DIR}/googletest/googlemock/src/gmock-all.cc
)

target_include_directories(mbgl-vendor-googletest PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/googletest/googletest
    ${CMAKE_CURRENT_LIST_DIR}/googletest/googletest/include
    ${CMAKE_CURRENT_LIST_DIR}/googletest/googlemock
    ${CMAKE_CURRENT_LIST_DIR}/googletest/googlemock/include
)

target_include_directories(mbgl-vendor-googletest SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/googletest/googletest/include
    ${CMAKE_CURRENT_LIST_DIR}/googletest/googlemock/include
)

set_property(TARGET mbgl-vendor-googletest PROPERTY FOLDER Core)
