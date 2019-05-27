package com.mapbox.mapboxsdk.style.sources;

import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;

import java.net.URI;
import java.net.URL;

/**
 * A raster DEM source. Currently only supports Mapbox Terrain RGB (mapbox://mapbox.terrain-rgb)
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-js/style-spec/#sources-raster-dem">The style specification</a>
 */
@UiThread
public class RasterDemSource extends Source {
  public static final int DEFAULT_TILE_SIZE = 512;

  /**
   * Internal use
   *
   * @param nativePtr - pointer to native peer
   */
  @Keep
  RasterDemSource(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Create the raster dem source from an URL
   *
   * @param id  the source id
   * @param url the source url
   * @deprecated use {@link #RasterDemSource(String, URI)} instead
   */
  @Deprecated
  public RasterDemSource(String id, URL url) {
    this(id, url.toExternalForm());
  }

  /**
   * Create the raster dem source from an URI
   *
   * @param id  the source id
   * @param uri the source uri
   */
  public RasterDemSource(String id, URI uri) {
    this(id, uri.toString());
  }

  /**
   * Create the raster dem source from an URI
   *
   * @param id  the source id
   * @param uri the source uri
   */
  public RasterDemSource(String id, String uri) {
    super();
    initialize(id, uri, DEFAULT_TILE_SIZE);
  }

  /**
   * Create the raster source from an URL with a specific tile size
   *
   * @param id       the source id
   * @param uri     the source url
   * @param tileSize the tile size
   */
  public RasterDemSource(String id, String uri, int tileSize) {
    super();
    initialize(id, uri, tileSize);
  }

  /**
   * Create the raster dem source from a {@link TileSet}
   *
   * @param id      the source id
   * @param tileSet the {@link TileSet}
   */
  public RasterDemSource(String id, TileSet tileSet) {
    super();
    initialize(id, tileSet.toValueObject(), DEFAULT_TILE_SIZE);
  }

  /**
   * Create the raster source from a {@link TileSet} with a specific tile size
   *
   * @param id       the source id
   * @param tileSet  the {@link TileSet}
   * @param tileSize tje tile size
   */
  public RasterDemSource(String id, TileSet tileSet, int tileSize) {
    super();
    initialize(id, tileSet.toValueObject(), tileSize);
  }

  /**
   * @return The url or null
   * @deprecated use {@link #getUri()} instead
   */
  @Deprecated
  @Nullable
  public String getUrl() {
    checkThread();
    return nativeGetUrl();
  }

  /**
   * @return The uri or null
   */
  @Nullable
  public String getUri() {
    checkThread();
    return nativeGetUrl();
  }

  @Keep
  protected native void initialize(String layerId, Object payload, int tileSize);

  @Override
  @Keep
  protected native void finalize() throws Throwable;

  @NonNull
  @Keep
  protected native String nativeGetUrl();

}
