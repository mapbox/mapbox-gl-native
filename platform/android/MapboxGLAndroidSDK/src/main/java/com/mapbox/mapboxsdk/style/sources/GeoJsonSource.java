package com.mapbox.mapboxsdk.style.sources;

import androidx.annotation.Keep;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.UiThread;
import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Geometry;
import com.mapbox.mapboxsdk.style.expressions.Expression;

import java.net.URI;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * GeoJson source, allows using FeatureCollections from Json.
 *
 * @see <a href="https://www.mapbox.com/mapbox-gl-style-spec/#sources-geojson">the style specification</a>
 */
@UiThread
public class GeoJsonSource extends Source {

  /**
   * Internal use
   *
   * @param nativePtr - pointer to native peer
   */
  @Keep
  GeoJsonSource(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Create an empty GeoJsonSource
   *
   * @param id the source id
   */
  public GeoJsonSource(String id) {
    super();
    initialize(id, null);
    setGeoJson(FeatureCollection.fromFeatures(new ArrayList<Feature>()));
  }

  /**
   * Create an empty GeoJsonSource with non-default GeoJsonOptions.
   *
   * @param id      the source id
   * @param options options
   */
  public GeoJsonSource(String id, GeoJsonOptions options) {
    super();
    initialize(id, options);
    setGeoJson(FeatureCollection.fromFeatures(new ArrayList<Feature>()));
  }

  /**
   * Create a GeoJsonSource from a raw json string
   *
   * @param id      the source id
   * @param geoJson raw Json FeatureCollection
   */
  public GeoJsonSource(String id, @Nullable String geoJson) {
    super();
    if (geoJson == null || geoJson.startsWith("http")) {
      throw new IllegalArgumentException("Expected a raw json body");
    }
    initialize(id, null);
    setGeoJson(geoJson);
  }

  /**
   * Create a GeoJsonSource from a raw json string and non-default GeoJsonOptions
   *
   * @param id      the source id
   * @param geoJson raw Json body
   * @param options options
   */
  public GeoJsonSource(String id, @Nullable String geoJson, GeoJsonOptions options) {
    super();
    if (geoJson == null || geoJson.startsWith("http") || geoJson.startsWith("asset") || geoJson.startsWith("file")) {
      throw new IllegalArgumentException("Expected a raw json body");
    }
    initialize(id, options);
    setGeoJson(geoJson);
  }

  /**
   * Create a GeoJsonSource from a remote geo json file
   *
   * @param id  the source id
   * @param url remote json file
   * @deprecated use {@link #GeoJsonSource(String, URI)} instead
   */
  @Deprecated
  public GeoJsonSource(String id, URL url) {
    super();
    initialize(id, null);
    nativeSetUrl(url.toExternalForm());
  }

  /**
   * Create a GeoJsonSource from a remote geo json file and non-default GeoJsonOptions
   *
   * @param id      the source id
   * @param url     remote json file
   * @param options options
   * @deprecated use {@link #GeoJsonSource(String, URI, GeoJsonOptions)} instead
   */
  @Deprecated
  public GeoJsonSource(String id, URL url, GeoJsonOptions options) {
    super();
    initialize(id, options);
    nativeSetUrl(url.toExternalForm());
  }

  /**
   * Create a GeoJsonSource from a geo json URI
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
   * @param uri unique resource identifier
   */
  public GeoJsonSource(String id, URI uri) {
    super();
    initialize(id, null);
    nativeSetUrl(uri.toString());
  }

  /**
   * Create a GeoJsonSource from a geo json URI and non-default GeoJsonOptions
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
   * @param id      the source id
   * @param uri     remote json file
   * @param options options
   */
  public GeoJsonSource(String id, URI uri, GeoJsonOptions options) {
    super();
    initialize(id, options);
    nativeSetUrl(uri.toString());
  }

  /**
   * Create a GeoJsonSource from a FeatureCollection.
   *
   * @param id       the source id
   * @param features the features
   */
  public GeoJsonSource(String id, FeatureCollection features) {
    super();
    initialize(id, null);
    setGeoJson(features);
  }

  /**
   * Create a GeoJsonSource from a FeatureCollection and non-default GeoJsonOptions.
   *
   * @param id       the source id
   * @param features the features
   * @param options  options
   */
  public GeoJsonSource(String id, FeatureCollection features, GeoJsonOptions options) {
    super();
    initialize(id, options);
    setGeoJson(features);
  }

  /**
   * Create a GeoJsonSource from a {@link Feature}
   *
   * @param id      the source id
   * @param feature the feature
   */
  public GeoJsonSource(String id, Feature feature) {
    super();
    initialize(id, null);
    setGeoJson(feature);
  }

  /**
   * Create a GeoJsonSource from a {@link Feature} and non-default {@link GeoJsonOptions}
   *
   * @param id      the source id
   * @param feature the feature
   * @param options options
   */
  public GeoJsonSource(String id, Feature feature, GeoJsonOptions options) {
    super();
    initialize(id, options);
    setGeoJson(feature);
  }

  /**
   * Create a GeoJsonSource from a {@link Geometry}
   *
   * @param id       the source id
   * @param geometry the geometry
   */
  public GeoJsonSource(String id, Geometry geometry) {
    super();
    initialize(id, null);
    setGeoJson(geometry);
  }

  /**
   * Create a GeoJsonSource from a {@link Geometry} and non-default {@link GeoJsonOptions}
   *
   * @param id       the source id
   * @param geometry the geometry
   * @param options  options
   */
  public GeoJsonSource(String id, Geometry geometry, GeoJsonOptions options) {
    super();
    initialize(id, options);
    setGeoJson(geometry);
  }

  /**
   * Updates the GeoJson with a single feature. The update is performed asynchronously,
   * so the data won't be immediately visible or available to query when this method returns.
   *
   * @param feature the GeoJSON {@link Feature} to set
   */
  public void setGeoJson(Feature feature) {
    if (detached) {
      return;
    }
    checkThread();
    nativeSetFeature(feature);
  }

  /**
   * Updates the GeoJson with a single geometry. The update is performed asynchronously,
   * so the data won't be immediately visible or available to query when this method returns.
   *
   * @param geometry the GeoJSON {@link Geometry} to set
   */
  public void setGeoJson(Geometry geometry) {
    if (detached) {
      return;
    }
    checkThread();
    nativeSetGeometry(geometry);
  }

  /**
   * Updates the GeoJson. The update is performed asynchronously,
   * so the data won't be immediately visible or available to query when this method returns.
   *
   * @param featureCollection the GeoJSON FeatureCollection
   */
  public void setGeoJson(@Nullable FeatureCollection featureCollection) {
    if (detached) {
      return;
    }
    checkThread();

    if (featureCollection != null && featureCollection.features() != null) {
      List<Feature> features = featureCollection.features();
      List<Feature> featuresCopy = new ArrayList<>(features);
      nativeSetFeatureCollection(FeatureCollection.fromFeatures(featuresCopy));
    } else {
      nativeSetFeatureCollection(featureCollection);
    }
  }

  /**
   * Updates the GeoJson. The update is performed asynchronously,
   * so the data won't be immediately visible or available to query when this method returns.
   *
   * @param json the raw GeoJson FeatureCollection string
   */
  public void setGeoJson(String json) {
    if (detached) {
      return;
    }
    checkThread();
    nativeSetGeoJsonString(json);
  }

  /**
   * Updates the url
   *
   * @param url the GeoJSON FeatureCollection url
   * @deprecated use {@link #setUri(URI)} instead
   */
  @Deprecated
  public void setUrl(@NonNull URL url) {
    checkThread();
    setUrl(url.toExternalForm());
  }

  /**
   * Updates the URI of the source.
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
   * @param uri the GeoJSON FeatureCollection uri
   */
  public void setUri(@NonNull URI uri) {
    setUri(uri.toString());
  }

  /**
   * Updates the url
   *
   * @param url the GeoJSON FeatureCollection url
   * @deprecated use {@link #setUri(String)} instead
   */
  @Deprecated
  public void setUrl(String url) {
    checkThread();
    nativeSetUrl(url);
  }

  /**
   * Updates the URI of the source.
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
   * @param uri the GeoJSON FeatureCollection uri
   */
  public void setUri(String uri) {
    checkThread();
    nativeSetUrl(uri);
  }

  /**
   * @return The url or null
   * @deprecated use {@link #getUri()} instead
   */
  @Nullable
  public String getUrl() {
    checkThread();
    return nativeGetUrl();
  }

  /**
   * Get the URI of the source.
   *
   * @return The uri or null
   */
  @Nullable
  public String getUri() {
    checkThread();
    return nativeGetUrl();
  }

  /**
   * Queries the source for features.
   *
   * @param filter an optional filter expression to filter the returned Features
   * @return the features
   */
  @NonNull
  public List<Feature> querySourceFeatures(@Nullable Expression filter) {
    checkThread();
    Feature[] features = querySourceFeatures(filter != null ? filter.toArray() : null);
    return features != null ? Arrays.asList(features) : new ArrayList<Feature>();
  }

  /**
   * Returns the children of a cluster (on the next zoom level) given its id (cluster_id value from feature properties).
   * <p>
   * Requires configuring this source as a cluster by calling {@link GeoJsonOptions#withCluster(boolean)}.
   * </p>
   *
   * @param cluster cluster from which to retrieve children from
   * @return a list of features for the underlying children
   */
  @NonNull
  public FeatureCollection getClusterChildren(@NonNull Feature cluster) {
    checkThread();
    return FeatureCollection.fromFeatures(nativeGetClusterChildren(cluster));
  }

  /**
   * Returns all the leaves of a cluster (given its cluster_id), with pagination support: limit is the number of leaves
   * to return (set to Infinity for all points), and offset is the amount of points to skip (for pagination).
   * <p>
   * Requires configuring this source as a cluster by calling {@link GeoJsonOptions#withCluster(boolean)}.
   * </p>
   *
   * @param cluster cluster from which to retrieve leaves from
   * @param limit   limit is the number of points to return
   * @param offset  offset is the amount of points to skip (for pagination)
   * @return a list of features for the underlying leaves
   */
  @NonNull
  public FeatureCollection getClusterLeaves(@NonNull Feature cluster, long limit, long offset) {
    checkThread();
    return FeatureCollection.fromFeatures(nativeGetClusterLeaves(cluster, limit, offset));
  }

  /**
   * Returns the zoom on which the cluster expands into several children (useful for "click to zoom" feature)
   * given the cluster's cluster_id (cluster_id value from feature properties).
   * <p>
   * Requires configuring this source as a cluster by calling {@link GeoJsonOptions#withCluster(boolean)}.
   * </p>
   *
   * @param cluster cluster from which to retrieve the expansion zoom from
   * @return the zoom on which the cluster expands into several children
   */
  public int getClusterExpansionZoom(@NonNull Feature cluster) {
    checkThread();
    return nativeGetClusterExpansionZoom(cluster);
  }

  @Keep
  protected native void initialize(String layerId, Object options);

  @Keep
  protected native void nativeSetUrl(String url);

  @NonNull
  @Keep
  protected native String nativeGetUrl();

  @Keep
  private native void nativeSetGeoJsonString(String geoJson);

  @Keep
  private native void nativeSetFeatureCollection(FeatureCollection geoJson);

  @Keep
  private native void nativeSetFeature(Feature feature);

  @Keep
  private native void nativeSetGeometry(Geometry geometry);

  @NonNull
  @Keep
  private native Feature[] querySourceFeatures(Object[] filter);

  @Keep
  private native Feature[] nativeGetClusterChildren(Feature feature);

  @Keep
  private native Feature[] nativeGetClusterLeaves(Feature feature, long limit, long offset);

  @Keep
  private native int nativeGetClusterExpansionZoom(Feature feature);

  @Override
  @Keep
  protected native void finalize() throws Throwable;

}
