package com.mapbox.mapboxsdk.layers;

public class CustomLayer {
    public CustomLayer(String id,
                       long context,
                       long initializeFunction,
                       long renderFunction,
                       long deinitializeFunction) {
        this.mID = id;
        this.mContext = context;
        this.mInitializeFunction = initializeFunction;
        this.mRenderFunction = renderFunction;
        this.mDeinitializeFunction = deinitializeFunction;
    }

    public String mID;
    public long mContext;
    public long mInitializeFunction;
    public long mRenderFunction;
    public long mDeinitializeFunction;
}
