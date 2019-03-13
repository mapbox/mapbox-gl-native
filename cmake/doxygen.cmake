find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NAME "Mapbox GL Native")
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/docs")
    set(DOXYGEN_GENERATE_XML TRUE)
    set(DOXYGEN_QUIET YES)
    set(DOXYGEN_WARN_LOGFILE ${CMAKE_BINARY_DIR}/doxygen_warn.log)

    doxygen_add_docs(generate_xml_doxygen
        ${CMAKE_SOURCE_DIR}/include
    )

    add_custom_target(doxygen_coverage
        COMMAND ${CMAKE_SOURCE_DIR}/scripts/doxy-coverage.py --quiet --noerror --json="${CMAKE_BINARY_DIR}/doxygen-coverage.json" "${CMAKE_BINARY_DIR}/docs/xml"
        COMMENT "Checking documentation coverage"
        DEPENDS generate_xml_doxygen
    )
endif()
