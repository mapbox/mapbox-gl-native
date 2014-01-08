

# Build instructions

## Mac OS X

```
make
```


## Emscripten

```
emmake make emscripten
```

Note that mousewheel support requires a hacked emscripten:

In `src/library_glfw.js`, replace the first part of the mousewheel function with

```js
      GLFW.wheelPos = (event.detail || -event.deltaY);
```

In `system/include/GL/glfw.h`, replace the wheel callback typedef with

```c
typedef void (GLFWCALL * GLFWmousewheelfun)(float);
```



# Style protobuf

We're encoding the styling information as a protocol buffer, according to
proto/style.proto. The reason for doing so is that we don't have to ship with a
JSON/yaml parser. To test the conversion script, run

```
bin/convert-style.js | protoc --decode=style proto/style.proto
```

You should see a nicely formatted hierarchical output of the converted style.
