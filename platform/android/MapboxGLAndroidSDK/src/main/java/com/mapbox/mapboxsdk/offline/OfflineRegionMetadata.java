package com.mapbox.mapboxsdk.offline;

/**
 * Arbitrary binary region metadata. The contents are opaque to the SDK implementation;
 * it just stores and retrieves a byte[].
 */
public class OfflineRegionMetadata {

    private byte[] metadata;

    /*
     * Constructor
     */

    public OfflineRegionMetadata(byte[] metadata) {
        this.metadata = metadata;
    }

    /*
     * Getters and setters
     */

    public byte[] getMetadata() {
        return metadata;
    }

    public void setMetadata(byte[] metadata) {
        this.metadata = metadata;
    }

    /*
     * Overrides
     */

    @Override
    public String toString() {
        return "OfflineRegionMetadata{metadata=" + metadata + "}";
    }

}
