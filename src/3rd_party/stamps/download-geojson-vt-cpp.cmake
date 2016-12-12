message(STATUS "downloading...
     src='https://github.com/mapbox/geojson-vt-cpp/archive/v6.1.3.zip'
     dst='/home/tmpsantos/Projects/mapbox-gl-native/build/geojson-vt-cpp-prefix/src/v6.1.3.zip'
     timeout='none'")




file(DOWNLOAD
  "https://github.com/mapbox/geojson-vt-cpp/archive/v6.1.3.zip"
  "/home/tmpsantos/Projects/mapbox-gl-native/build/geojson-vt-cpp-prefix/src/v6.1.3.zip"
  SHOW_PROGRESS
  # no TIMEOUT
  STATUS status
  LOG log)

list(GET status 0 status_code)
list(GET status 1 status_string)

if(NOT status_code EQUAL 0)
  message(FATAL_ERROR "error: downloading 'https://github.com/mapbox/geojson-vt-cpp/archive/v6.1.3.zip' failed
  status_code: ${status_code}
  status_string: ${status_string}
  log: ${log}
")
endif()

message(STATUS "downloading... done")
