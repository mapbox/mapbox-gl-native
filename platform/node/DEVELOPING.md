# Developing the Mapbox GL Native Node.js module

This document explains how to build the [Node.js](https://nodejs.org/) bindings for [Mapbox GL Native](../../README.md) for contributing to the development of the bindings themselves. If you just want to use the module, you can simply install it via `npm`; see [README.md](README.md) for installation and usage instructions.

## Building

To develop these bindings, you’ll need to build them from source. Building requires the prerequisites listed in either
the [macOS](../macos/INSTALL.md#requirements) or [Linux](../linux/README.md#prerequisites) install documentation, depending
on the target platform.

To compile the Node.js bindings and install module dependencies, from the repository root directory, first run:

```
make distclean
```

If you are rebuilding after time has passed.

Then do:

```bash
make node
```

## Testing

To test the Node.js bindings:

```bash
npm test
```


## Merging your pull request

To clean up your pull request and prepare it for merging, update your local `master` branch, then run `git rebase -i master` from your pull request branch to squash/fixup commits as needed. When your work is ready to be merged, you can run `git merge --ff-only YOUR_BRANCH` from `master` or click the green merge button in the GitHub UI, which will automatically squash your branch down into a single commit before merging it.

## Publishing

We aim to publish the `@mapbox/mapbox-gl-native` package on the same four-week cadence as the iOS, Android, and JavaScript SDKs, including a beta release one week prior to the final release. Responsibility for publishing rotates between team members.

To publish a new version of the package:
- [ ] make a commit in the release branch which includes:
    - [ ] an updated version number in [`package.json`](../../package.json#L3)
    - [ ] an entry in [`platform/node/CHANGELOG.md`](CHANGELOG.md) describing the changes in the release
- [ ] run `git tag node-v{VERSION}` where `{VERSION}` matches the version in `package.json`, e.g. `git tag node-v3.3.2`
- [ ] run `git push && git push --tags`

The CI builds for tag pushes will check if the tag matches the version listed in `package.json`, and if so, will run with `BUILDTYPE=Release` and publish a binary with `node-pre-gyp`.

Once binaries have been published for Linux and macOS (which can be verified with `./node_modules/.bin/node-pre-gyp info`), you can run a quick final check to ensure they're being fetched properly by simply running `rm -rf lib && npm install`.

If everything looks good:
- [ ] run `mbx npm publish`

### Preleases

Publishing a prerelease binary can be useful for testing downstream integrations - the workflow is pretty much the same except that you'll be making your version number commit and `git tag node-v{VERSION}` (like `git tag node-v3.3.2-pre.1`) on a pull request branch before merging it rather than on `master`.
