if(TARGET mbgl-vendor-zip-archive)
    return()
endif()

add_library(
    mbgl-vendor-zip-archive STATIC
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/SSZipArchive.h
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/SSZipArchive.m
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/SSZipCommon.h
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/ZipArchive.h
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz.h
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_compat.c
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_compat.h
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_crypt.c
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_crypt.h
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_crypt_apple.c
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_os.c
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_os.h
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_os_posix.c
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_strm.c
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_strm.h
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_strm_buf.c
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_strm_buf.h
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_strm_mem.c
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_strm_mem.h
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_strm_os.h
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_strm_os_posix.c
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_strm_pkcrypt.c
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_strm_pkcrypt.h
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_strm_split.c
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_strm_split.h
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_strm_wzaes.c
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_strm_wzaes.h
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_strm_zlib.c
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_strm_zlib.h
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_zip.c
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_zip.h
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_zip_rw.c
    ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive/minizip/mz_zip_rw.h
)

target_link_libraries(
    mbgl-vendor-zip-archive
    PRIVATE z
)

target_include_directories(
    mbgl-vendor-zip-archive SYSTEM
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/zip-archive/SSZipArchive
)

target_compile_definitions(
    mbgl-vendor-zip-archive
    PRIVATE
        HAVE_INTTYPES_H
        HAVE_PKCRYPT
        HAVE_STDINT_H
        HAVE_WZAES
        HAVE_ZLIB
)
set_property(TARGET mbgl-vendor-zip-archive PROPERTY FOLDER RenderTestApp)
