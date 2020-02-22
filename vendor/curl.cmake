if(TARGET mbgl-vendor-curl)
    return()
endif()

add_library(
    mbgl-vendor-curl INTERFACE
)

target_link_libraries(
    mbgl-vendor-curl
    INTERFACE $<$<PLATFORM_ID:Android>:${CMAKE_CURRENT_LIST_DIR}/curl-android-ios/prebuilt-with-ssl/android/${ANDROID_ABI}/libcurl.a>
)

target_include_directories(
    mbgl-vendor-curl SYSTEM
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/curl-android-ios/curl/include
)
