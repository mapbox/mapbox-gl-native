# All dependencies in a single separate file so its checksum can be used in a CI cache key name

mason_use(geometry VERSION 0.9.3 HEADER_ONLY)
mason_use(variant VERSION 1.1.4 HEADER_ONLY)
mason_use(unique_resource VERSION cba309e HEADER_ONLY)
mason_use(rapidjson VERSION 1.1.0 HEADER_ONLY)
mason_use(boost VERSION 1.65.1 HEADER_ONLY)
mason_use(geojsonvt VERSION 6.6.0 HEADER_ONLY)
mason_use(supercluster VERSION 0.2.2 HEADER_ONLY)
mason_use(kdbush VERSION 0.1.1-1 HEADER_ONLY)
mason_use(earcut VERSION 0.12.4 HEADER_ONLY)
mason_use(protozero VERSION 1.5.2 HEADER_ONLY)
mason_use(pixelmatch VERSION 0.10.0 HEADER_ONLY)
mason_use(geojson VERSION 0.4.2 HEADER_ONLY)
mason_use(polylabel VERSION 1.0.3 HEADER_ONLY)
mason_use(wagyu VERSION 0.4.3 HEADER_ONLY)
mason_use(shelf-pack VERSION 2.1.1 HEADER_ONLY)
mason_use(cheap-ruler VERSION 2.5.3 HEADER_ONLY)
mason_use(vector-tile VERSION 1.0.2 HEADER_ONLY)

if(MBGL_PLATFORM STREQUAL "android")
    mason_use(jni.hpp VERSION 4.0.0-rc10 HEADER_ONLY)
    mason_use(sqlite VERSION 3.24.0-min-size)
    mason_use(icu VERSION 58.1-min-size)
elseif(MBGL_PLATFORM STREQUAL "ios")
    mason_use(icu VERSION 58.1-min-size)
elseif(MBGL_PLATFORM STREQUAL "linux")
    mason_use(glfw VERSION 2018-06-27-0be4f3f)
    mason_use(sqlite VERSION 3.24.0-min-size)
    mason_use(libuv VERSION 1.9.1)
    mason_use(libpng VERSION 1.6.25)
    mason_use(libjpeg-turbo VERSION 1.5.0)
    mason_use(webp VERSION 0.5.1)
    mason_use(benchmark VERSION 1.4.1${MASON_CXXABI_SUFFIX})
    mason_use(icu VERSION 58.1-min-size)
    mason_use(args VERSION 6.2.0 HEADER_ONLY)

    if(WITH_EGL)
        mason_use(swiftshader VERSION 2018-05-31)
    endif()
elseif(MBGL_PLATFORM STREQUAL "macos")
    mason_use(glfw VERSION 2018-06-27-0be4f3f)
    mason_use(benchmark VERSION 1.4.1)
    mason_use(icu VERSION 58.1-min-size)
    mason_use(args VERSION 6.2.0 HEADER_ONLY)

    if(WITH_EGL)
        mason_use(swiftshader VERSION 2018-05-31)
    endif()
elseif(MBGL_PLATFORM STREQUAL "qt")
    mason_use(optional VERSION f27e7908 HEADER_ONLY)
    mason_use(tao_tuple VERSION 28626e99 HEADER_ONLY)

    if(NOT WITH_QT_DECODERS)
        mason_use(libjpeg-turbo VERSION 1.5.0)
        mason_use(libpng VERSION 1.6.25)
        mason_use(webp VERSION 0.5.1)
    endif()

    if(NOT WITH_QT_I18N)
        mason_use(icu VERSION 58.1-min-size)
    endif()
endif()
