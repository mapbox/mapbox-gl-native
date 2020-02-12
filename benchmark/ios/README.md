# iOS BenchmarkRunner App

This is a blank single-view-controller iOS app, linked against to a C++ static libraries, plus a a simple unit xctest. We use CMake to create an Xcode-friendly out-of-source build system. In another word, the build system is maintained by `CMakeLists` file,instead of a configured `.xcodeproj` file.

This CMake project can build the executable BenchmarkApp which is linked to the static C++ library 'mbgl-benchmark'. CMakeLists.txt files are the only build configuration kept in source control. This is in contrast to committing the `.xcodeproj` directory which includes the backing XML, which is nonsensically hard to edit by hand.

The test instantiates ObjC object of class `IosTestRunner` from the app, the class will instantiates a C++ object from the linked library `mbgl-benchmark` and calls running benchmark function on it. It subsequently deletes the C++ object pointer. In the end, the test will check the existence of test report, which is a json file that attach to xctest result.
