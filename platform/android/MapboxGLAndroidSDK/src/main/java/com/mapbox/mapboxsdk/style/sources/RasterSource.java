package com.mapbox.mapboxsdk.style.sources;

import androidx.annotation.Keep;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.net.URI;
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
  @Keep
  RasterSource(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Create the raster source from an URL
   *
   * @param id  the source id
   * @param url the source url
   * @deprecated use {@link #RasterSource(String, URI)} instead
   */
  public RasterSource(String id, URL url) {
    this(id, url.toExternalForm());
  }

  /**
   * Create the raster source from an URI.
   * <p>
   * An URI is a combination of a protocol and a resource path.
   * The following URI protocol schemes are supported:
   * </p>
   * <ul>
   * <li>http://</li>
   * <ul>
   * <li>load resources using HyperText Transfer Protocol</li>
   * </ul>
   * <li>file://</li>
   * <ul>
   * <li>load resources from the Android file system</li>
   * </ul>
   * <li>asset://</li>
   * <ul>
   * <li>load resources from the binary packaged assets folder</li>
   * </ul>
   * </ul>
   *
   * @param id  the source id
   * @param uri the source uri
   */
  public RasterSource(String id, URI uri) {
    this(id, uri.toString());
  }

  /**
   * Create the raster source from an URI.
   * <p>
   * An URI is a combination of a protocol and a resource path.
   * The following URI protocol schemes are supported:
   * </p>
   * <ul>
   * <li>http://</li>
   * <ul>
   * <li>load resources using HyperText Transfer Protocol</li>
   * </ul>
   * <li>file://</li>
   * <ul>
   * <li>load resources from the Android file system</li>
   * </ul>
   * <li>asset://</li>
   * <ul>
   * <li>load resources from the binary packaged assets folder</li>
   * </ul>
   * </ul>
   *
   * @param id  the source id
   * @param uri the source uri
   */
  public RasterSource(String id, String uri) {
    super();
    initialize(id, uri, DEFAULT_TILE_SIZE);
  }

  /**
   * Create the raster source from an URI with a specific tile size.
   * <p>
   * An URI is a combination of a protocol and a resource path.
   * The following URI protocol schemes are supported:
   * </p>
   * <ul>
   * <li>http://</li>
   * <ul>
   * <li>load resources using HyperText Transfer Protocol</li>
   * </ul>
   * <li>file://</li>
   * <ul>
   * <li>load resources from the Android file system</li>
   * </ul>
   * <li>asset://</li>
   * <ul>
   * <li>load resources from the binary packaged assets folder</li>
   * </ul>
   * </ul>
   *
   * @param id       the source id
   * @param uri      the source uri
   * @param tileSize the tile size
   */
  public RasterSource(String id, String uri, int tileSize) {
    super();
    initialize(id, uri, tileSize);
  }

  /**
   * Create the raster source from a {@link TileSet}
   *
   * @param id      the source id
   * @param tileSet the {@link TileSet}
   */
  public RasterSource(String id, TileSet tileSet) {
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
  public RasterSource(String id, TileSet tileSet, int tileSize) {
    super();
    initialize(id, tileSet.toValueObject(), tileSize);
  }

  /**
   * @return The url or null
   * @deprecated use {@link #getUri()} instead
   */
  @Nullable
  @Deprecated
  public String getUrl() {
    checkThread();
    return nativeGetUrl();
  }

  /**
   * Get the source URI.
   *
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
