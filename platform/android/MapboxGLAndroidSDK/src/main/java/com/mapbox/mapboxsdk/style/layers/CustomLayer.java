package com.mapbox.mapboxsdk.style.layers;

/**
 * Custom layer.
 * <p/>
 * Experimental feature. Do not use.
 */
public class CustomLayer extends Layer {

    public CustomLayer(String id,
                       long context,
                       long initializeFunction,
                       long renderFunction,
                       long deinitializeFunction) {
        initialize(id, initializeFunction, renderFunction, deinitializeFunction, context);
    }

    public CustomLayer(long nativePtr) {
        super(nativePtr);
    }

    public void invalidate() {
        nativeUpdate();
    }

    protected native void initialize(String id, long initializeFunction, long renderFunction, long deinitializeFunction, long context);

    protected native void nativeUpdate();

}
