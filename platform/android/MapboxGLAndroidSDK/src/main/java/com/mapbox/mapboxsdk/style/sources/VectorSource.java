package com.mapbox.mapboxsdk.style.sources;

import java.net.URL;

/**
 * A vector source.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-vector">the style specification</a>
 */
public class VectorSource extends Source {
    public static final String TYPE = "vector";
    private static final String URL_KEY = "url";

    /**
     * Create a vector source from a remote url
     *
     * @param id  the source id
     * @param url the url
     */
    public VectorSource(String id, URL url) {
        this(id, url.toExternalForm());
    }

    /**
     * Create a vector source from a remote url
     *
     * @param id  the source id
     * @param url the url
     */
    public VectorSource(String id, String url) {
        super(id, TYPE);
        this.put(URL_KEY, url);
    }

    /**
     * Create a vector source from a tilset
     *
     * @param id      the source id
     * @param tileSet the tileset
     */
    public VectorSource(String id, TileSet tileSet) {
        super(id, TYPE);
        this.putAll(tileSet.toValueObject());
    }
}
