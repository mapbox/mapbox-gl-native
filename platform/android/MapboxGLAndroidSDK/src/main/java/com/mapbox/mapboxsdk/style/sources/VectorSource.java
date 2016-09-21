package com.mapbox.mapboxsdk.style.sources;

import java.net.URL;

/**
 * A vector source.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-vector">the style specification</a>
 */
public class VectorSource extends Source {

    /**
     * Internal use
     */
    public VectorSource(long nativePtr) {
        super(nativePtr);
    }

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
        initialize(id, url);
    }

    /**
     * Create a vector source from a tilset
     *
     * @param id      the source id
     * @param tileSet the tileset
     */
    public VectorSource(String id, TileSet tileSet) {
        initialize(id, tileSet.toValueObject());
    }

    protected native void initialize(String layerId, Object payload);

    @Override
    protected native void finalize() throws Throwable;
}
