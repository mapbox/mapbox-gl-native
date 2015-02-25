package com.mapbox.mapboxgl.lib.util;

public class JavaFileSource {

    private static JavaFileSource instance = null;

    /**
     * Private Constructor to support Singleton pattern
     */
    private JavaFileSource() {
        super();
    }

    /**
     * Get the singleton instance of JavaFileSource
     * @return Reference to the Singleton Instance of JavaFileSource
     */
    public static JavaFileSource getInstance() {
        if (instance == null) {
            instance = new JavaFileSource();
        }
        return instance;
    }
}
