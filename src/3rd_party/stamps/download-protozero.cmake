message(STATUS "downloading...
     src='https://github.com/mapbox/protozero/archive/v1.4.2.zip'
     dst='/home/tmpsantos/Projects/mapbox-gl-native/build/protozero-prefix/src/v1.4.2.zip'
     timeout='none'")




file(DOWNLOAD
  "https://github.com/mapbox/protozero/archive/v1.4.2.zip"
  "/home/tmpsantos/Projects/mapbox-gl-native/build/protozero-prefix/src/v1.4.2.zip"
  SHOW_PROGRESS
  # no TIMEOUT
  STATUS status
  LOG log)

list(GET status 0 status_code)
list(GET status 1 status_string)

if(NOT status_code EQUAL 0)
  message(FATAL_ERROR "error: downloading 'https://github.com/mapbox/protozero/archive/v1.4.2.zip' failed
  status_code: ${status_code}
  status_string: ${status_string}
  log: ${log}
")
endif()

message(STATUS "downloading... done")
