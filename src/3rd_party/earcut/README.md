## Earcut

A C++ port of [earcut.js](https://github.com/mapbox/earcut), a fast, [header-only](https://github.com/mapbox/earcut.hpp/blob/master/include/earcut.hpp) polygon triangulation library.

The library implements a modified ear slicing algorithm, optimized by [z-order curve](http://en.wikipedia.org/wiki/Z-order_curve) hashing and extended to handle holes, twisted polygons, degeneracies and self-intersections in a way that doesn't _guarantee_ correctness of triangulation, but attempts to always produce acceptable results for practical data like geographical shapes.

It's based on ideas from [FIST: Fast Industrial-Strength Triangulation of Polygons](http://www.cosy.sbg.ac.at/~held/projects/triang/triang.html) by Martin Held and [Triangulation by Ear Clipping](http://www.geometrictools.com/Documentation/TriangulationByEarClipping.pdf) by David Eberly.


## Usage

```cpp
#include <earcut.hpp>
```
```cpp
// The number type to use for tessellation
using Coord = double;

// The index type. Defaults to uint32_t, but you can also pass uint16_t if you know that your
// data won't have more than 65536 vertices.
using N = uint32_t;

// Create array
using Point = std::array<Coord, 2>;
std::vector<std::vector<Point>> polygon;
// ... fill polygon structure with actual data

// Run tessellation
// Returns array of indices that refer to the vertices of the input polygon.
// Three subsequent indices form a triangle.
std::vector<N> indices = mapbox::earcut<N>(polygon);
```

It is also possible to use your custom point type as input. There are default accessors defined for `std::tuple`, `std::pair`, and `std::array`. For a custom type (like Clipper's `IntPoint` type), do this:

```cpp
struct IntPoint {
    int64_t X, Y;
};

namespace mapbox {
namespace util {

template <>
struct nth<0, IntPoint> {
    inline static int64_t get(const IntPoint &t) {
        return t.X;
    };
};
template <>
struct nth<1, IntPoint> {
    inline static int64_t get(const IntPoint &t) {
        return t.Y;
    };
};

} // namespace util
} // namespace mapbox
```

<details><summary>Additional build instructions for test, benchmark and visualization:</summary>
In case you just want to use the earcut triangulation library; copy and include the header file [```<earcut.hpp>```](https://github.com/mapbox/earcut.hpp/blob/master/include/earcut.hpp) in your project and follow the steps documented in the section [Usage](#usage).

If you want to build the test, benchmark and visualization programs instead, follow these instructions:

## Dependencies

Before you continue, make sure to have the following tools and libraries installed:
 * git ([Ubuntu](https://help.ubuntu.com/lts/serverguide/git.html)/[Windows/Other](http://git-scm.com/downloads))
 * cmake 3.2+ ([Ubuntu](https://launchpad.net/~george-edison55/+archive/ubuntu/cmake-3.x)/[Windows/Other](https://cmake.org/download/))
 * OpenGL SDK ([Ubuntu](http://packages.ubuntu.com/de/trusty/libgl1-mesa-dev)/[Windows](https://dev.windows.com/en-us/downloads/windows-10-sdk)/[Apple](https://developer.apple.com/opengl/))
 * Compiler such as [GCC 4.9+, Clang 3.4+](https://launchpad.net/~ubuntu-toolchain-r/+archive/ubuntu/test), [MSVC12+](https://www.visualstudio.com/)

Note: On some operating systems such as Windows, manual steps are required to add cmake and [git](http://blog.countableset.ch/2012/06/07/adding-git-to-windows-7-path/) to your PATH environment variable.

## Manual compilation

```bash
git clone --recursive https://github.com/mapbox/earcut.hpp.git
cd ./earcut.hpp
mkdir build
cd ./build
cmake ..
make
# ./tests
# ./bench
# ./viz
```

## [Visual Studio](https://www.visualstudio.com/), [Eclipse](https://eclipse.org/), [XCode](https://developer.apple.com/xcode/), ...

```batch
git clone --recursive https://github.com/mapbox/earcut.hpp.git
cd ./earcut.hpp
mkdir project
cd ./project
cmake .. -G "Visual Studio 14 2015"
::you can also generate projects for "Visual Studio 12 2013", "XCode", "Eclipse CDT4 - Unix Makefiles"
```
After completion, open the generated project with your IDE.

## [CLion](https://www.jetbrains.com/clion/)

Import the project from https://github.com/mapbox/earcut.hpp.git and you should be good to go!
</details>

## Status

This is currently based on [earcut 2.1.1](https://github.com/mapbox/earcut#211-mar-17-2016).
