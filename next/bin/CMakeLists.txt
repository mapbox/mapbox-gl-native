add_executable(
    mbgl-offline
    ${MBGL_ROOT}/bin/offline.cpp
)

target_link_libraries(
    mbgl-offline
    PRIVATE Mapbox::Base::Extras::args mbgl-core
)

add_executable(
    mbgl-render
    ${MBGL_ROOT}/bin/render.cpp
)

target_link_libraries(
    mbgl-render
    PRIVATE Mapbox::Base::Extras::args mbgl-core
)

install(TARGETS mbgl-offline mbgl-render RUNTIME DESTINATION bin)

# FIXME: CI must have a valid token
#
# add_test(NAME mbgl-offline-tool-test COMMAND mbgl-offline -s mapbox://styles/mapbox/satellite-v9 --maxZoom=0 WORKING_DIRECTORY
# ${MBGL_ROOT} )
#
# add_test(NAME mbgl-render-tool-test COMMAND mbgl-render WORKING_DIRECTORY ${MBGL_ROOT} )
