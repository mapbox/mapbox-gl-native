package com.mapbox.mapboxsdk.testapp.offline;

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

}
