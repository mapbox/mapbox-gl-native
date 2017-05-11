package com.mapbox.mapboxsdk.style.sources;

import android.support.annotation.Nullable;

import java.net.URL;

/**
 * Raster source, allows using raster tiles as source.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-raster">The style specification</a>
 */
public class RasterSource extends Source {
  public static final int DEFAULT_TILE_SIZE = 512;

  /**
   * Internal use
   *
   * @param nativePtr - pointer to native peer
   */
  public RasterSource(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Create the raster source from an URL
   *
   * @param id  the source id
   * @param url the source url
   */
  public RasterSource(String id, URL url) {
    this(id, url.toExternalForm());
  }


  /**
   * Create the raster source from an URL
   *
   * @param id  the source id
   * @param url the source url
   */
  public RasterSource(String id, String url) {
    initialize(id, url, DEFAULT_TILE_SIZE);
  }

  /**
   * Create the raster source from an URL with a specific tile size
   *
   * @param id       the source id
   * @param url      the source url
   * @param tileSize the tile size
   */
  public RasterSource(String id, String url, int tileSize) {
    initialize(id, url, tileSize);
  }

  /**
   * Create the raster source from a {@link TileSet}
   *
   * @param id      the source id
   * @param tileSet the {@link TileSet}
   */
  public RasterSource(String id, TileSet tileSet) {
    initialize(id, tileSet.toValueObject(), DEFAULT_TILE_SIZE);
  }

  /**
   * Create the raster source from a {@link TileSet} with a specific tile size
   *
   * @param id       the source id
   * @param tileSet  the {@link TileSet}
   * @param tileSize tje tile size
   */
  public RasterSource(String id, TileSet tileSet, int tileSize) {
    initialize(id, tileSet.toValueObject(), tileSize);
  }

  /**
   * @return The url or null
   */
  @Nullable
  public String getUrl() {
    return nativeGetUrl();
  }

  protected native void initialize(String layerId, Object payload, int tileSize);

  @Override
  protected native void finalize() throws Throwable;

  protected native String nativeGetUrl();

}
