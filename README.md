

# Build instructions

## Mac OS X

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


# Usage

- Press 'R' to reset the transform
- Press 'N' to reset north
