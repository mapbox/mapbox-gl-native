An OpenGL renderer for [Mapbox vector tiles](https://www.mapbox.com/blog/vector-tiles),
implemented in C++, targeting iOS & OSX.

# Build instructions

## Mac OS X

Install boost and [glfw3](http://www.glfw.org/docs/latest/):

```
brew install boost
brew install homebrew/versions/glfw3
```

Then configure the project:

    ./configure

Options include:

 - --boost=/usr/local
 - --glfw3=/usr/local (by default pkg-config will be used)

Then you can build the Mac OS X app with make:

    make app

Or generate a duel iOS/Mac compatible XCode project for libllmr to include as a subproject:

    make xcode # then open llmr.xcodeproj


# Style protobuf

We're encoding the styling information as a protocol buffer, according to
proto/style.proto. The reason for doing so is that we don't have to ship with a
JSON/yaml parser. To test the conversion script, run

```
bin/convert-style.js | protoc --decode=style proto/style.proto
```

You should see a nicely formatted hierarchical output of the converted style.


To update the included default style, use

```
bin/convert-style.js > resources/style.pbf && bin/build-style.js
```


# Usage

- Press 'R' to reset the transform
- Press 'N' to reset north
- Press Tab to toggle debug information
- Press Esc to quit
