# Setting up Android emulator

If you want to run the test app in the emulator, we recommend the x86 build because it will run a lot faster.

First ensure you have an `MAPBOX_ACCESS_TOKEN` environment variable set, as described below. Then, create an x86 build:

    make android-lib-x86

In Android Studio, create an x86 AVD (Android Virtual Device):

- Open AVD Manager via the Tools menu -> Android -> AVD Manager
- Click "Create Virtual Device" at the bottom on AVD Manager window
- Select one of the device profiles, for example the Nexus 4
- Click "Next"
- Select a Lollipop or Kitkat release with ABI of x86. If the line is greyed out click Download to download the OS files.
- Click "Next"
- Under "Emulated Performance" check "Host GPU" and uncheck "Store a snapshot for faster startup"
- Click "Finish"
- Close the AVD Manager

Now when you run or debug the Android project you will see a window "Choose Device". Select your new AVD from drop down under "Launch emulator". If you select "Use same device for future launches" Android Studio will remember the selection and not ask again.
