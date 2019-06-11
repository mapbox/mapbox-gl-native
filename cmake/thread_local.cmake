include(WriteCompilerDetectionHeader)

write_compiler_detection_header(
  FILE "${CMAKE_CURRENT_BINARY_DIR}/compilerchecks/thread_local_compiler_detection.h"
  PREFIX MB
  COMPILERS GNU Clang AppleClang MSVC Intel
  FEATURES cxx_thread_local
)

add_definitions(-DDETECT_THREAD_LOCAL)
