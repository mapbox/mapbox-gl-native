# All dependencies in a single separate file so its checksum can be used in a CI cache key name

if(MBGL_PLATFORM STREQUAL "android")
    # noop
elseif(MBGL_PLATFORM STREQUAL "ios")
    # noop
elseif(MBGL_PLATFORM STREQUAL "linux")
    mason_use(libuv VERSION 1.9.1)
    mason_use(libpng VERSION 1.6.25)
    mason_use(libjpeg-turbo VERSION 1.5.0)
    mason_use(icu VERSION 63.1-min-static-data)

    if(WITH_EGL)
        mason_use(swiftshader VERSION 2018-05-31)
    endif()
elseif(MBGL_PLATFORM STREQUAL "macos")
    if(WITH_EGL)
        mason_use(swiftshader VERSION 2018-05-31)
    endif()
elseif(MBGL_PLATFORM STREQUAL "qt")
    if(NOT WITH_QT_DECODERS)
        mason_use(libjpeg-turbo VERSION 1.5.0)
        mason_use(libpng VERSION 1.6.25)
    endif()
endif()
