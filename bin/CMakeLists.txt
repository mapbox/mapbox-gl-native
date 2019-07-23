add_executable(mbgl-offline
    ${CMAKE_CURRENT_LIST_DIR}/offline.cpp
)

target_link_libraries(mbgl-offline PRIVATE
    Mapbox::Base::Extras::args
    mbgl-core
)

add_executable(mbgl-render
    ${CMAKE_CURRENT_LIST_DIR}/render.cpp
)

target_link_libraries(mbgl-render PRIVATE
    Mapbox::Base::Extras::args
    mbgl-core
)

add_test(NAME mbgl-offline-tool-test
    COMMAND mbgl-offline -s mapbox://styles/mapbox/satellite-v9 --maxZoom=0
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_test(NAME mbgl-render-tool-test
    COMMAND mbgl-render
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
