# Contributing to the Mapbox OS X SDK

This document explains how to build the Mapbox OS X SDK from source. It is intended for advanced developers who wish to contribute to Mapbox GL and the Mapbox iOS SDK.

### Requirements

The Mapbox OS X SDK and the osxapp demo application run on OS X 10.10.0 and above.

### Build

1. [Install core dependencies](../../INSTALL.md).
1. Run `make xproj`.
1. Switch to the osxsdk or osxapp scheme. The former builds just the Cocoa framework, while the latter also builds a Cocoa demo application based on it.

### Access tokens

The demo applications use Mapbox vector tiles, which require a Mapbox account and API access token. Obtain an access token on the [Mapbox account page](https://www.mapbox.com/studio/account/tokens/). You will be prompted for this access token the first time you launch the demo application.

### Usage

Through the OS X SDK, the demo application supports a variety of standard gestures and keyboard shortcuts. For more details, open Mapbox GL Help from the Help menu.

You can also [integrate the Mapbox OS X SDK into your own Cocoa application](INSTALL.md).
