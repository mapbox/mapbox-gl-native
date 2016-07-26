package com.mapbox.mapboxsdk.style.sources;

import java.net.URL;

/**
 * Construct a Raster Source.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-raster">The style specificition</a>
 */
public class RasterSource extends Source {
    public static final String TYPE = "raster";
    private static final String URL_KEY = "url";
    private static final String TILE_SIZE_KEY = "tileSize";

    public RasterSource(String id, URL url) {
        this(id, url.toExternalForm());
    }

    public RasterSource(String id, String url) {
        super(id, TYPE);
        this.put(URL_KEY, url);
    }

    public RasterSource(String id, TileSet tileSet) {
        super(id, TYPE);
        this.putAll(tileSet.toValueObject());
    }

    public RasterSource withTileSize(int tileSize) {
        this.put(TILE_SIZE_KEY, (float) tileSize);
        return this;
    }
}
