This document aims to outline at a high level the various parts that make up mapbox-gl-native and how they work together.

# Repository structure

mapbox-gl-native uses a monolithic that houses both core C++ code and code that wraps the C++ core with SDKs for Android, iOS, macOS, Node.js, and Qt. A "monorepo" allows us to:

 * Make changes to the core API and SDKs simultaneously, ensuring no platform falls behind.
 * Ensure that core changes do not inadvertently break SDK tests.
 * Centralize discussions about features and defects that affect multiple platforms.

In the repository, core C++ code is contained in the `include` and `src` directories. The former includes headers that are considered to make up the "public" core C++ API, while the latter includes `.cpp` implementation files and headers that are private to the implementation. Within both directories, files are nested under an `mbgl` directory, which has various subdirectories based on areas of functionality. Both public and private headers therefore can (and should) always be included with the form `#include <mbgl/___/___.hpp>`.

Code and build scripts belonging to platform SDKs are contained in the `platform` directory, which has subdirectories for each platform. The `platform/darwin` and `platform/default` directories contain code shared by multiple platform SDKs.

# Build system

The mapbox-gl-native build system uses a variety of tools.

## Make

GNU Make and a master `Makefile` serves as the coordinating tool for all other build system tools. Most of your interaction with the build system will be via `make` commands. We aim to have a consistent pattern for make targets across platforms: for a given platform, e.g. `ios`, `make ios` builds a reasonable set of end products (though perhaps not all permutations of build options, which can be prohibitively expensive), and `make ios-test` runs a complete test suite for the platform.

## Git submodules

Git submodules are used to pull in several dependencies: mason (see below) and several iOS dependencies (though we have plans to phase these out). Initializing these submodules is handled automatically by the necessary `make` targets.

## npm

npm is a package manager for Node.js. mapbox-gl-native uses it to pull in several development dependencies that happen to be packaged as node modules -- mainly for testing needs.

## Mason

[Mason](https://github.com/mapbox/mason) is Mapbox's own cross platform C/C++ package manager. mapbox-gl-native uses mason packages as a source of precompiled binaries for third-party dependencies such as Boost, RapidJSON, and SQLite, and Mapbox's own C++ modules such as [earcut.hpp](https://github.com/mapbox/earcut.hpp) and [geojson-vt-cpp](https://github.com/mapbox/geojson-vt-cpp). It is also used to obtain a toolchain for Android platform cross-compiliation.

We track mason dependencies for a given platform in the file `platform/<platform>/scripts/configure.sh`. The `configure` script at the root handles sourcing this file during the build, running the appropriate mason commands, and writing configuration settings for the subsequent build to a `config.gypi` in the build output directory.

## gyp

[gyp](https://gyp.gsrc.io/) is a build system originally created for Chromium and since adopted by Node.js. In mapbox-gl-native it's used to build the core C++ static library, the Node.js platform module, and the shared JNI module for Android.

## Platform-specific subsystems

Outside of the core C++ static library, platform SDKs typically rely on platform-native build tooling to complete the job.

* For iOS and macOS this means Xcode and the xcodebuild command line tool.
* For Android, Gradle and Android Studio.
* For Qt, `qmake`.

See the relevant platform-specific `README.md` / `INSTALL.md` for details.

# Major functional components

## Map
## Style

The "Style" component of mapbox-gl-native contains an implementation of the [Mapbox Style Specification](https://www.mapbox.com/mapbox-gl-style-spec/), defining what data to draw, the order to draw it in, and how to style the data when drawing it.

In addition to supporting styles loaded from a URL, mapbox-gl-native includes a runtime styling API, which allows users to dynamically modify the current style: add and remove layers, modify layer properties, and so on. As appropriate for a C++ API, the runtime styling API API is _strongly typed_: there are subclasses for each layer type, with correctly-typed accessors for each style property. This results in a large API surface area. Fortunately, this is automated, by generating the API – and the regular portion of the implementation – from the style specification.

The layers API makes a distinction between public API and internal implementation using [the `Impl` idiom](https://github.com/mapbox/mapbox-gl-native/issues/3254) seen elsewhere in the codebase. Here, it takes the form of two parallel class hierarchies:

* `Layer` and its subclasses form the public API.
* `Layer::Impl` and its subclasses form the internal API.

As well as forming the boundary between public and internal, these two class hierarchies form the boundary between generated code and handwritten code. Except for `CustomLayer` and `CustomLayer::Impl`:

* `Layer` subclasses are entirely generated. (`Layer` itself is handwritten.)
* `Layer::Impl` and its subclasses are entirely handwritten.

## FileSource
## Layout
## Rendering
## Annotations

# Threading

At runtime, mapbox-gl-native uses the following threads:

* The "main thread" (or other thread on which a `Map` object is created) handles direct `Map` API requests, owns the active `Style` and its associated objects, and renders the map. Since this thread is usually dispatching events triggered by user input, it's important that these duties not require significant computation or perform blocking I/O that would cause UI jank or hangs.
* Many of the tasks that require significant computation are associated with layout and styling of map features: parsing vector tiles, computing text layout, and generating data buffers to be consumed by OpenGL. This work happens on "worker threads" that are spawned by the main thread, four per `Style` object. The `Style` and its associated objects handle dispatching tasks to the workers, typically on a tile-by-tile basis.
* A "FileSource" thread handles network requests for styles, tiles, and other resources, and I/O on the SQLite database used for offline maps and caching. Requests originate from the main thread, are dispatched by a FileSource internally to its thread, and results are returned to the main thread via an asynchronous callback function. (This is implemented with platform-specific hooks into the native message pump for the requesting thread.)

To minimize data races and invalid memory access, we aim for zero shared memory state between any two threads. Cross-thread communication occurs via one thread requesting the "invocation" of a task on another thread. The parameters for this task should be passed as value objects, so that they are copied, or as ownership-transferring values such as `std::unique_ptr`. (We're [not entirely there yet](https://github.com/mapbox/mapbox-gl-native/issues/667), but that's the goal.) The result (if any) is likewise passed as a value or ownership-transferring parameter to the callback function.

Invoking a task on another thread itself creates an ownership obligation: the responsibility for the work happening on the other thread. This ownership is represented by the invocation method returning `std::unique_ptr<AsyncTask>`. Destroying this object indicates that the result of the task is no longer required: the callback is guaranteed not to be called, and the work on the other thread may be aborted (if doing so is convenient).

All this is implemented by [Thread](https://github.com/mapbox/mapbox-gl-native/blob/master/src/mbgl/util/thread.hpp) and [RunLoop](https://github.com/mapbox/mapbox-gl-native/blob/master/include/mbgl/util/run_loop.hpp).
