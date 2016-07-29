package com.mapbox.mapboxsdk.style.sources;

import java.net.URL;

public class RasterSource extends Source {
    public static final String TYPE = "raster";
    private static final String URL_KEY = "url";

    public RasterSource(String id, URL url) {
        this(id, url.toExternalForm());
    }

    public RasterSource(String id, String url) {
        super(id, TYPE);
        this.put(URL_KEY, url);
    }
}
