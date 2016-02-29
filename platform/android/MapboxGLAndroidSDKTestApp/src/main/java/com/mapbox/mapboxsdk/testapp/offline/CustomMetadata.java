package com.mapbox.mapboxsdk.testapp.offline;

import com.google.gson.Gson;

import java.io.UnsupportedEncodingException;

/**
 * A custom metadata class
 */
public class CustomMetadata {

    public final static String CHARSET = "UTF-8";

    /*
     * Fields
     */

    private String regionName;

    /*
     * Constructor
     */

    public CustomMetadata(String regionName) {
        this.regionName = regionName;
    }

    /*
     * Getters/setters
     */

    public String getRegionName() {
        return regionName;
    }

    public void setRegionName(String regionName) {
        this.regionName = regionName;
    }

    /*
     * Helper methods to encode/decode metadata into/from byte[]
     */

    public byte[] encode() throws UnsupportedEncodingException {
        return new Gson()
                .toJson(this, CustomMetadata.class)
                .getBytes(CustomMetadata.CHARSET);
    }

    public static CustomMetadata decode(byte[] metadata) throws UnsupportedEncodingException {
        String json = new String(metadata, CustomMetadata.CHARSET);
        return new Gson()
                .fromJson(json, CustomMetadata.class);
    }

}
