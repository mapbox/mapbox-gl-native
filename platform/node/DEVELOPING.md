# Developing the Mapbox GL Native Node.js module

This document explains how to build the [Node.js](https://nodejs.org/) bindings for [Mapbox GL Native](../../README.md) for contributing to the development of the bindings themselves. If you just want to use the module, you can simply install it via `npm`; see [README.md](README.md) for installation and usage instructions.

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

## Merging your pull request

To clean up your pull request and prepare it for merging, update your local `master` branch, then run `git rebase -i master` from your pull request branch to squash/fixup commits as needed. When your work is ready to be merged, you can run `git merge --ff-only YOUR_BRANCH` from `master` or click the green merge button in the GitHub UI, which will automatically squash your branch down into a single commit before merging it.

## Publishing

After your pull request has been merged (and you've fetched the latest `master` if merged from the GitHub UI), make a new commit in the `master` branch that updates the version number in [`package.json`](../../package.json#L3) and adds an entry in [`platform/node/CHANGELOG.md`](CHANGELOG.md) describing the changes in your release.

Tag this commit with `git tag node-v{VERSION}` where `{VERSION}` matches the version in `package.json`, like `git tag node-v3.3.2`, then run `git push && git push --gs`. Travis CI integration runs on tag publishes and will check if the `$TAG` env var [matches the version listed in `package.json`](https://github.com/mapbox/mapbox-gl-native/blob/94a58691e24cd0760f9a3c2ac1a9322aa7854367/.travis.yml#L62-L64), and if so, will run with `BUILDTYPE=Release` and [publish a binary with `node-pre-gyp`](https://github.com/mapbox/mapbox-gl-native/blob/94a58691e24cd0760f9a3c2ac1a9322aa7854367/platform/node/scripts/after_script.sh#L9-L15).

This autopublish on git tag workflow [broke for macOS binaries when we switched over to Bitrise](https://github.com/mapbox/mapbox-gl-native/issues/4854), so the temporary workaround has been to manually publish macOS binaries after pushing the new tag. Make sure the `mason` submodule is up to date with `git submodule update --init` then run `make distclean && BUILDTYPE=Release npm install --build-from-source && ./node_modules/.bin/node-pre-gyp package publish info`.

Once binaries have been published for Linux and macOS (which can be verified with `./node_modules/.bin/node-pre-gyp info`), you can run a quick final check to ensure they're being fetched properly by simply running `rm -rf lib && npm install`. If everything looks good, run `npm publish`!

### Preleases

Publishing a prerelease binary can be useful for testing downstream integrations - the workflow is pretty much the same except that you'll be making your version number commit and `git tag node-v{VERSION}` (like `git tag node-v3.3.2-pre.1`) on a pull request branch before merging it rather than on `master`.
