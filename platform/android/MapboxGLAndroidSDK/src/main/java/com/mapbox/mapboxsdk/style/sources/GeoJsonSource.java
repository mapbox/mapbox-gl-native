package com.mapbox.mapboxsdk.style.sources;

import com.mapbox.services.commons.geojson.Feature;
import com.mapbox.services.commons.geojson.FeatureCollection;

import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * A GeoJson source.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">the style specification</a>
 */
public class GeoJsonSource extends Source {

    /**
     * Internal use
     */
    public GeoJsonSource(long nativePtr) {
        super(nativePtr);
    }

    /**
     * Create an empty GeoJsonSource
     *
     * @param id the source id
     */
    public GeoJsonSource(String id) {
        initialize(id, null);
        setGeoJson(FeatureCollection.fromFeatures(new ArrayList<Feature>()));
    }

    /**
     * Create an empty GeoJsonSource
     *
     * @param id      the source id
     * @param options options
     */
    public GeoJsonSource(String id, GeoJsonOptions options) {
        initialize(id, options);
        setGeoJson(FeatureCollection.fromFeatures(new ArrayList<Feature>()));
    }

    /**
     * Create a GeoJsonSource from a raw json string
     *
     * @param id      the source id
     * @param geoJson raw Json body
     */
    public GeoJsonSource(String id, String geoJson) {
        if (geoJson == null || geoJson.startsWith("http")) {
            throw new IllegalArgumentException("Expected a raw json body");
        }
        initialize(id, null);
        setGeoJson(geoJson);
    }

    /**
     * Create a GeoJsonSource from a raw json string
     *
     * @param id      the source id
     * @param geoJson raw Json body
     * @param options options
     */
    public GeoJsonSource(String id, String geoJson, GeoJsonOptions options) {
        if (geoJson == null || geoJson.startsWith("http")) {
            throw new IllegalArgumentException("Expected a raw json body");
        }
        initialize(id, options);
        setGeoJson(geoJson);
    }

    /**
     * Create a GeoJsonSource from a remote geo json file
     *
     * @param id  the source id
     * @param url remote json file
     */
    public GeoJsonSource(String id, URL url) {
        initialize(id, null);
        nativeSetUrl(url.toExternalForm());
    }

    /**
     * Create a GeoJsonSource from a remote geo json file
     *
     * @param id      the source id
     * @param url     remote json file
     * @param options options
     */
    public GeoJsonSource(String id, URL url, GeoJsonOptions options) {
        initialize(id, options);
        nativeSetUrl(url.toExternalForm());
    }

    /**
     * Create a GeoJsonSource from a FeatureCollection
     *
     * @param id       the source id
     * @param features the features
     */
    public GeoJsonSource(String id, FeatureCollection features) {
        initialize(id, null);
        setGeoJson(features);
    }

    /**
     * Create a GeoJsonSource from a FeatureCollection
     *
     * @param id       the source id
     * @param features the features
     * @param options  options
     */
    public GeoJsonSource(String id, FeatureCollection features, GeoJsonOptions options) {
        initialize(id, options);
        setGeoJson(features);
    }

    public void setGeoJson(FeatureCollection features) {
        checkValidity();
        setGeoJson(features.toJson());
    }

    public void setGeoJson(String json) {
        checkValidity();
        setRawJson(json);
    }

    public void setUrl(URL url) {
        checkValidity();
        setUrl(url.toExternalForm());
    }

    public void setUrl(String url) {
        checkValidity();
        nativeSetUrl(url);
    }

    protected void setRawJson(String geoJson) {
        //Wrap the String in a map as an Object is expected by the
        //style conversion template
        HashMap<String, String> wrapper = new HashMap<>();
        wrapper.put("data", geoJson);
        nativeSetGeoJson(wrapper);
    }

    protected native void initialize(String layerId, Object options);

    protected native void nativeSetUrl(String url);

    private native void nativeSetGeoJson(Object geoJson);

    @Override
    protected native void finalize() throws Throwable;

}
