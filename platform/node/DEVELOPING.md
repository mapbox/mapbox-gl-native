# Developing the Mapbox GL Native Node.js module

This document explains how to build the [Node.js](https://nodejs.org/) bindings for [../../README.md](Mapbox GL Native) for contributing to the development of the bindings themselves. If you just want to use the module, you can simply install it via `npm`; see [README.md](README.md) for installation and usage instructions.

## Building

To develop these bindings, you’ll need to build them from source. Building requires [installing all of the basic dependencies needed for Mapbox GL Native](../../INSTALL.md), then running:

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
