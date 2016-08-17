package com.mapbox.mapboxsdk.style.sources;

import java.util.HashMap;

/**
 * Options for the <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">GeoJsonSource</a>
 */
public class GeoJsonOptions extends HashMap<String, Object> {

    /**
     * Defaults to 18.
     * Maximum zoom level at which to create vector tiles (higher means greater detail at high zoom levels).
     */
    public GeoJsonOptions withMaxZoom(int maxZoom) {
        this.put("maxzoom", maxZoom);
        return this;
    }

    /**
     * Defaults to 128.
     * Tile buffer size on each side (measured in 1/512ths of a tile; higher means fewer rendering artifacts near tile edges but slower performance).
     */
    public GeoJsonOptions withBuffer(int buffer) {
        this.put("buffer", buffer);
        return this;
    }

    /**
     * Defaults to 0.375.
     * Douglas-Peucker simplification tolerance (higher means simpler geometries and faster performance).
     */
    public GeoJsonOptions withTolerance(float tolerance) {
        this.put("tolerance", tolerance);
        return this;
    }

    /**
     * Defaults to false.
     * If the data is a collection of point features, setting this to true clusters the points by radius into groups.
     */
    public GeoJsonOptions withCluster(boolean cluster) {
        this.put("cluster", cluster);
        return this;
    }

    /**
     * Defaults to 50.
     * Radius of each cluster when clustering points, measured in 1/512ths of a tile.
     */
    public GeoJsonOptions withClusterMaxZoom(int clusterMaxZoom) {
        this.put("clusterMaxZoom", clusterMaxZoom);
        return this;
    }

    /**
     * Max zoom to cluster points on. Defaults to one zoom less than maxzoom (so that last zoom features are not clustered).
     */
    public GeoJsonOptions withClusterRadius(int clusterRadius) {
        this.put("clusterRadius", clusterRadius);
        return this;
    }

}
