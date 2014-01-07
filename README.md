

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
