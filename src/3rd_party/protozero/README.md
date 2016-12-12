# protozero

Minimalistic protocol buffer decoder and encoder in C++.

Designed for high performance. Suitable for writing zero copy parsers and
encoders with minimal need for run-time allocation of memory.

Low-level: this is designed to be a building block for writing a very
customized decoder for a stable protobuf schema. If your protobuf schema
is changing frequently or lazy decoding is not critical for your application
then this approach offers no value: just use the decoding API available via the
C++ API that can be generated via the Google Protobufs `protoc` program.

[![Travis Build Status](https://travis-ci.org/mapbox/protozero.svg?branch=master)](https://travis-ci.org/mapbox/protozero)
[![Appveyor Build Status](https://ci.appveyor.com/api/projects/status/o354pq10y96mnr6d?svg=true)](https://ci.appveyor.com/project/Mapbox/protozero)
[![Coverage Status](https://coveralls.io/repos/mapbox/protozero/badge.svg?branch=master&service=github)](https://coveralls.io/github/mapbox/protozero?branch=master)

## Depends

 - C++11 compiler


## How it works

The protozero code does **not** read `.proto` files used by the usual Protobuf
implementations. The developer using protozero has to manually "translate" the
`.proto` description into code. This means there is no way to access any of the
information from the `.proto` description. This results in a few restrictions:

* The names of the fields are not available.
* Enum names are not available, you'll have to use the values they are defined
  with.
* Default values are not available.
* Field types have to be hardcoded. The library does not know which types to
  expect, so the user of the library has to supply the right types. Some checks
  are made using `assert()`, but mostly the user has to take care of that.

The library will make sure not to overrun the buffer it was given, but
basically all other checks have to be made in user code!


## Documentation

You have to have a working knowledge of how
[protocol buffer encoding works](https://developers.google.com/protocol-buffers/docs/encoding).

* Read the [tutorial](doc/tutorial.md) for an introduction on how to use
  Protozero.
* There is a table of all types and functions in the
  [cheat sheet](doc/cheatsheet.md).
* [Macros defined or used by Protozero](doc/macros.md).
* Read the [upgrading instructions](UPGRADING.md) if you are upgrading from
  an older version of Protozero.

Call `make doc` to build the Doxygen-based reference documentation. (You'll
need [Doxygen](http://www.stack.nl/~dimitri/doxygen/) installed.) Then open
`doc/html/index.html` in your browser to read it.


## Installation

Call `make install` to install include files in `/usr/include/protozero`. Call
`make install DESTDIR=/usr/local` or similar to change install directory.


## Limitations

* A protobuf message has to fit into memory completely, otherwise it can not
  be parsed with this library. There is no streaming support.
* The length of a string, bytes, or submessage can't be more than 2^31-1.
* The Google Protobuf spec documents that a non-repeated field can actually
  appear several times in a message and the implementation is required to
  return the value of the last version of that field in this case.
  `pbf_reader.hpp` does not enforce this. If this feature is needed in your
  case, you have to do this yourself.
* There is no specific support for maps but they can be used as described in
  the "Backwards compatibility" section of
  https://developers.google.com/protocol-buffers/docs/proto3#maps.


## Memory Alignment Issues and Endianness

Protobuf-encoded data is not necessarily properly aligned for the machine we
are using. For single values this isn't a problem, because we copy those into
a properly aligned variable and return that one. But for repeated packed values
it can be a problem, because we give users access to them through an iterator.
To get the best performance this iterator is usually just a raw pointer. This
works fine on Intel processors, non-aligned access is just slower than aligned
access. On ARM this is not necessarily the case (depends on machine type and
compile options), so we need to go through a special iterator there
which makes sure to return aligned data on member access. Basically the same
iterator is used on big endian architectures to put the bytes in the correct
order before handing them back to the application.

Detection of endianess and those architectures which have problems with
non-aligned data is not perfect. If tests fail for you, this might be a problem
in your setup. Please open an issue on Github in this case and tell us about
your system.

See also the discussion on https://github.com/mapbox/protozero/issues/33 .


## Tests

Extensive tests are included. Call

    make test

to build all tests and run them.

See `test/README.md` for more details about the test.

You can also use `gyp` to build the reader tests:

    gyp gyp/protozero.gyp --depth=. --build=Release
    ./out/Release/tests

This will clobber the `Makefile` from the repository! Instead of `Release` you
can use `Debug` for a debug build.


## Coverage report

To get a coverage report compile and link with `--coverage`:

    CXXFLAGS="--coverage" LDFLAGS="--coverage" make test

If you are using `g++` use `gcov` to generate a report (results are in `*.gcov`
files):

    gcov -lp test/*tests.o test/t/*/*test_cases.o

If you are using `clang++` use `llvm-cov` instead:

    llvm-cov gcov -lp test/*tests.o test/t/*/*test_cases.o

If you are using `g++` you can use `gcovr` to generate nice HTML output:

    mkdir -p coverage
    gcovr -r . --html --html-details -o coverage/index.html

Open `coverage/index.html` in your browser to see the report.


## Cppcheck

For extra checks with [Cppcheck](http://cppcheck.sourceforge.net/) you can call

    make check


## Who is using Protozero?

* [Carmen](https://github.com/mapbox/carmen-cache)
* [Libosmium](https://github.com/osmcode/libosmium)
* [Mapnik](https://github.com/mapbox/mapnik-vector-tile)
* [Mapbox GL Native](https://github.com/mapbox/mapbox-gl-native)
* [OSRM](https://github.com/Project-OSRM/osrm-backend)
* [Tippecanoe](https://github.com/mapbox/tippecanoe)

Are you using Protozero? Tell us! Send a pull request with changes to this
README.


