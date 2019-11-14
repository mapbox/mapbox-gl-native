package com.mapbox.mapboxsdk.style.sources;

import android.net.Uri;
import androidx.annotation.Keep;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.Size;
import androidx.annotation.UiThread;

import com.mapbox.geojson.Feature;
import com.mapbox.mapboxsdk.style.expressions.Expression;

import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Vector source, allows the use of vector tiles.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-vector">the style specification</a>
 */
@UiThread
public class VectorSource extends Source {

  /**
   * Internal use
   *
   * @param nativePtr - pointer to native peer
   */
  @Keep
  VectorSource(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Create a vector source from a remote url pointing to a TileJSON resource
   *
   * @param id  the source id
   * @param url the TileJSON resource url
   * @deprecated use {@link #VectorSource(String, Uri)} instead
   */
  @Deprecated
  public VectorSource(String id, URL url) {
    this(id, url.toExternalForm());
  }

  /**
   * Create a vector source from an URI.
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
   * @param uri the TileJSON resource uri
   */
  public VectorSource(String id, Uri uri) {
    this(id, uri.toString());
  }

  /**
   * Create a vector source from an URI.
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
   * @param uri the uri
   */
  public VectorSource(String id, String uri) {
    super();
    initialize(id, uri);
  }

  /**
   * Create a vector source from a tileset
   *
   * @param id      the source id
   * @param tileSet the tileset
   */
  public VectorSource(String id, TileSet tileSet) {
    super();
    initialize(id, tileSet.toValueObject());
  }

  /**
   * Queries the source for features.
   *
   * @param sourceLayerIds the source layer identifiers. At least one must be specified.
   * @param filter         an optional filter expression to filter the returned Features
   * @return the features
   */
  @NonNull
  public List<Feature> querySourceFeatures(@Size(min = 1) String[] sourceLayerIds,
                                           @Nullable Expression filter) {
    checkThread();
    Feature[] features = querySourceFeatures(
      sourceLayerIds,
      filter != null ? filter.toArray() : null);
    return features != null ? Arrays.asList(features) : new ArrayList<Feature>();
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
  protected native void initialize(String layerId, Object payload);

  @Override
  @Keep
  protected native void finalize() throws Throwable;

  @NonNull
  @Keep
  protected native String nativeGetUrl();

  @NonNull
  @Keep
  private native Feature[] querySourceFeatures(String[] sourceLayerId,
                                               Object[] filter);

}
