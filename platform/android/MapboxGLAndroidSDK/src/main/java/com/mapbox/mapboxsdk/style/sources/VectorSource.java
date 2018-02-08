package com.mapbox.mapboxsdk.style.sources;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.Size;
import android.support.annotation.UiThread;

import com.mapbox.geojson.Feature;
import com.mapbox.mapboxsdk.style.layers.Filter;

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
  public VectorSource(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Create a vector source from a remote url pointing to a TileJSON resource
   *
   * @param id  the source id
   * @param url the TileJSON resource url
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

  /**
   * Queries the source for features.
   *
   * @param sourceLayerIds the source layer identifiers. At least one must be specified.
   * @param filter         an optional filter statement to filter the returned Features
   * @return the features
   */
  @NonNull
  public List<Feature> querySourceFeatures(@Size(min = 1) String[] sourceLayerIds,
                                           @Nullable Filter.Statement filter) {
    Feature[] features = querySourceFeatures(
      sourceLayerIds,
      filter != null ? filter.toArray() : null);
    return features != null ? Arrays.asList(features) : new ArrayList<Feature>();
  }

  /**
   * @return The url or null
   */
  @Nullable
  public String getUrl() {
    return nativeGetUrl();
  }

  protected native void initialize(String layerId, Object payload);

  @Override
  protected native void finalize() throws Throwable;

  protected native String nativeGetUrl();

  private native Feature[] querySourceFeatures(String[] sourceLayerId,
                                               Object[] filter);

}
