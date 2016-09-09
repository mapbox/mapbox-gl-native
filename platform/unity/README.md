# Unity plugin example

The Unity platform is not officialy supported by Mapbox. This plugin is just an example of how you can use Mapbox GL Native for rendering vector maps inside a Unity scene.

It derived from the [NativePluginInterface](https://docs.unity3d.com/Manual/NativePluginInterface.html) on the Unity official documentation and is known to run on Ubuntu 16.04 and OS X El Capitan.

### Build

Ensure you have git and other build essentials:

```
$ sudo apt-get install curl git build-essential zlib1g-dev cmake npm
```

Build with:

```
$ make unity
```

### Running

Build the original example as instructed on the Unity documentation. You will see a ball and a plasma effect like this:

![The original example](https://c1.staticflickr.com/9/8648/28568276835_ba94e7fbea_n.jpg)

Set the environment variable `MAPBOX_ACCESS_TOKEN` to your [Mapbox access token](ACCESS_TOKEN.md):

```
$ export MAPBOX_ACCESS_TOKEN=MYTOKEN
```

Replace the plugin `libRenderingPlugin.so` in the example with `libMapboxGLPlugin.so` and run it again. It should look like:

![Now rendering a map](https://c1.staticflickr.com/9/8839/27951118054_5fb1ec3920_n.jpg)

The plugin was written for OpenGL, you might need to force it when running the Unity runtime:

```
$ ./mygame -force-opengl
```
