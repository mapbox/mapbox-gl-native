package com.mapbox.mapboxsdk.testapp.layers;

public class ExampleCustomLayer {
    static {
        System.loadLibrary("example-custom-layer");
    }

    public native static long createContext();
    public static long InitializeFunction;
    public static long RenderFunction;
    public static long DeinitializeFunction;
}
