package com.mapbox.mapboxsdk.style.sources;

import java.net.URL;

public class VectorSource extends Source {
    public static final String TYPE = "vector";
    private static final String URL_KEY = "url";

    public VectorSource(String id, URL url) {
        this(id, url.toExternalForm());
    }

    public VectorSource(String id, String url) {
        super(id, TYPE);
        this.put(URL_KEY, url);
    }

    public VectorSource(String id, TileSet tileSet) {
        super(id, TYPE);
        this.putAll(tileSet.toValueObject());
    }
}
