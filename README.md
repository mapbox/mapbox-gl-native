iOS
===

1. Make sure you have `xcode/llmr.xcodeproj`, which is a modified version of the OS X CMake-generated version. 

1. Download & unpack Boost into `ios/boost_1_55_0`. 

1. This does some nasty things with exposing `map.transform` and hardcoding the shader preamble, but be not afraid. 

1. Open, build, and run on simulator. Does not build for ARM yet. 

### Gestures

 * Pan to move. 

 * Pinch to zoom in or out (currently around map center, not gesture center). 

 * Double-tap to zoom in one level at gesture center. 

 * Two-finger single-tap to zoom out one level at gesture center. 

 * Rotate with two fingers to rotate map (currently around map center, not gesture center). 

 * Long-press to reset north as up. 