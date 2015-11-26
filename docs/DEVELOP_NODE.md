# Developing the Mapbox GL Native Node Module

**Just want to use the module? You can install it via npm without doing any of
this:**
```
npm install mapbox-gl-native
```

This repository contains the bindings from the C++ core of Mapbox GL Native
to [Node.js](https://nodejs.org/). To develop these bindings,
you'll need to build them from source. Building requires installing all of the basic
dependencies needed for Mapbox GL Native, then running:

    npm install --build-from-source

From the root directory. This will compile the Node.js bindings and install module dependencies.

To recompile just the C++ code while developing, run `make node`.

To create an Xcode project and use a GUI debugger in the case of a crash, run `make xnode`.

## Testing

To test the Node.js bindings:

```
npm test
```

To run the visual render test suite:

```
npm run test-suite
```
