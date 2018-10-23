add_library(rapidjson INTERFACE)

target_include_directories(rapidjson SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/rapidjson/include
)

target_compile_definitions(rapidjson INTERFACE
    RAPIDJSON_HAS_STDSTRING=1
)

if(WIN32)
    target_compile_definitions(rapidjson INTERFACE
        RAPIDJSON_HAS_CXX11_RVALUE_REFS
    )
endif()
