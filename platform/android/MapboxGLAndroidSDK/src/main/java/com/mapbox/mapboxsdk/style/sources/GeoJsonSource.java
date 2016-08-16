package com.mapbox.mapboxsdk.style.sources;

import com.google.gson.Gson;
import com.mapbox.services.commons.geojson.FeatureCollection;

import java.net.URL;
import java.util.HashMap;

/**
 * A GeoJson source.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">the style specification</a>
 */
public class GeoJsonSource extends Source {
    public static final String TYPE = "geojson";
    private static final String DATA_KEY = "data";

    /**
     * Create a GeoJsonSource from a raw json string
     *
     * @param id      the source id
     * @param geoJson raw Json body
     */
    public GeoJsonSource(String id, String geoJson) {
        super(id, TYPE);
        if (geoJson == null || geoJson.startsWith("http")) {
            throw new IllegalArgumentException("Expected a raw json body");
        }
        setRawJson(geoJson);
    }

    /**
     * Create a GeoJsonSource from a remote geo json file
     *
     * @param id  the source id
     * @param url remote json file
     */
    public GeoJsonSource(String id, URL url) {
        super(id, TYPE);
        this.put(DATA_KEY, url.toExternalForm());
    }

    /**
     * Create a GeoJsonSource from a FeatureCollection
     *
     * @param id       the source id
     * @param features the features
     */
    public GeoJsonSource(String id, FeatureCollection features) {
        super(id, TYPE);
        setRawJson(features.toJson());
    }

    public GeoJsonSource withCluster(boolean cluster) {
        this.put("cluster", cluster);
        return this;
    }

    public GeoJsonSource withClusterMaxZoom(float zoom) {
        this.put("clusterMaxZoom", zoom);
        return this;
    }

    public GeoJsonSource withClusterRadius(float radius) {
        this.put("clusterRadius", radius);
        return this;
    }

    private void setRawJson(String geoJson) {
        //Wrap the String in a map as an Object is expected by the
        //style conversion template
        HashMap<String, String> wrapper = new HashMap<>();
        wrapper.put(DATA_KEY, geoJson);
        this.put(DATA_KEY, wrapper);
    }
}
