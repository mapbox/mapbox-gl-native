package com.mapbox.mapboxsdk.testapp.model.customlayer;

public class ExampleCustomLayer {
    static {
        System.loadLibrary("example-custom-layer");
    }

    public native static long createContext();

    public native static void setColor(float red, float green, float blue, float alpha);

    public static long InitializeFunction;
    public static long RenderFunction;
    public static long DeinitializeFunction;
}
