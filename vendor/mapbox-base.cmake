# License helper for mapbox-base, should be upstreamed.

if(NOT TARGET mapbox-base)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/mapbox-base)
endif()
