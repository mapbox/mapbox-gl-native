An OpenGL renderer for [Mapbox vector tiles](https://www.mapbox.com/blog/vector-tiles),
implemented in C++, targeting iOS & OSX.

# Build instructions

## Mac OS X

You may need to install [glfw3](http://www.glfw.org/docs/latest/):

```
brew install homebrew/versions/glfw3
```

```
mkdir xcode
cd xcode
cmake .. -G Xcode
```

Then open the xcode project and build.


# Style protobuf

We're encoding the styling information as a protocol buffer, according to
proto/style.proto. The reason for doing so is that we don't have to ship with a
JSON/yaml parser. To test the conversion script, run

```
bin/convert-style.js | protoc --decode=style proto/style.proto
```

You should see a nicely formatted hierarchical output of the converted style.
