package com.mapbox.mapboxsdk.offline;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

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

    /*
     * byte[] utils
     */

    public static byte[] serialize(Object obj) throws IOException {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        ObjectOutputStream os = new ObjectOutputStream(out);
        os.writeObject(obj);
        return out.toByteArray();
    }

    public static Object deserialize(byte[] data) throws IOException, ClassNotFoundException {
        ByteArrayInputStream in = new ByteArrayInputStream(data);
        ObjectInputStream is = new ObjectInputStream(in);
        return is.readObject();
    }

}
