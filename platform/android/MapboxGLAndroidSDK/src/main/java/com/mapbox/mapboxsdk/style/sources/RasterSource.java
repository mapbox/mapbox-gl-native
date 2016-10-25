package com.mapbox.mapboxsdk.style.sources;

import java.net.URL;

/**
 * Construct a Raster Source.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-raster">The style specificition</a>
 */
public class RasterSource extends Source {
    public static final int DEFAULT_TILE_SIZE = 512;

    /**
     * Internal use
     */
    public RasterSource(long nativePtr) {
        super(nativePtr);
    }

    public RasterSource(String id, URL url) {
        this(id, url.toExternalForm());
    }

    public RasterSource(String id, String url) {
        initialize(id, url, DEFAULT_TILE_SIZE);
    }

    public RasterSource(String id, String url, int tileSize) {
        initialize(id, url, tileSize);
    }

    public RasterSource(String id, TileSet tileSet) {
        initialize(id, tileSet.toValueObject(), DEFAULT_TILE_SIZE);
    }

    public RasterSource(String id, TileSet tileSet, int tileSize) {
        initialize(id, tileSet.toValueObject(), tileSize);
    }

    protected native void initialize(String layerId, Object payload, int tileSize);

    @Override
    protected native void finalize() throws Throwable;
}
