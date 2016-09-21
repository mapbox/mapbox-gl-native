package com.mapbox.mapboxsdk.style.sources;

import java.util.HashMap;

/**
 * Base Peer class for sources. see source.hpp for the other half of the peer.
 */
public abstract class Source {
    private long nativePtr;
    private boolean invalidated;

    public Source(long nativePtr) {
        this.nativePtr = nativePtr;
    }

    public Source() {
    }

    public String getId() {
        checkValidity();
        return nativeGetId();
    }

    public long getNativePtr() {
        return nativePtr;
    }

    protected native String nativeGetId();

    protected void checkValidity() {
        if (invalidated) {
            throw new RuntimeException("Layer has been invalidated. Request a new reference after adding");
        }
    }

    public final void invalidate() {
        this.invalidated = true;
    }
}
