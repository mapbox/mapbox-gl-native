{
  "includes": [
      "common.gypi"
  ],
  "targets": [
    {
      "target_name": "tests",
      "type": "executable",
      "sources": [
        "<!@(find ../test/ -name '*.cpp' ! -name 'writer_tests.cpp' ! -name 'writer_test_cases.cpp' ! -name 'testcase.cpp')"
      ],
      "include_dirs": [
        "../include/",
        "../test/include/"
      ]
    },
    {
      "target_name": "writer_tests",
      "type": "executable",
      "sources": [
        "../test/writer_tests.cpp",
        "<!@(find ../test/ -name 'writer_test_cases.cpp')",
        "<!@(find ../include/protozero/ -name '*.hpp' ! -name 'config.hpp' ! -name 'pbf_builder.hpp' ! -name 'pbf_message.hpp' ! -name 'version.hpp')",
        "<!@(find ../test/ -name '*.pb.cc')",
      ],
      "include_dirs": [
        "../",
        "../include/",
        "../test/include/",
        "../deps/protobuf/include"
      ],
      "libraries" : [
        "../deps/protobuf/$(PlatformTarget)/$(Configuration)/libprotobuf-lite.lib"
      ]
    }
  ]
}
