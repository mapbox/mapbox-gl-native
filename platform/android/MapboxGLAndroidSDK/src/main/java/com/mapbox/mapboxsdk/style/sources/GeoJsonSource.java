package com.mapbox.mapboxsdk.style.sources;

import java.net.URL;
import java.util.HashMap;

public class GeoJsonSource extends Source {
    public static final String TYPE = "geojson";
    private static final String DATA_KEY = "data";

    public GeoJsonSource(String id, String geoJson) {
        super(id, TYPE);
        //Wrap the String in a map as an Object is expected by the
        //style conversion template
        HashMap<String, String> wrapper = new HashMap<>();
        wrapper.put(DATA_KEY, geoJson);
        this.put(DATA_KEY, wrapper);
    }

    public GeoJsonSource(String id, URL url) {
        super(id, TYPE);
        this.put(DATA_KEY, url.toExternalForm());
    }
}
