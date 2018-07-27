package com.mapbox.mapboxsdk.style.sources;

import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Geometry;
import com.mapbox.mapboxsdk.style.expressions.Expression;

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
  public GeoJsonSource(long nativePtr) {
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
  public GeoJsonSource(String id, String geoJson) {
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
  public GeoJsonSource(String id, String geoJson, GeoJsonOptions options) {
    super();
    if (geoJson == null || geoJson.startsWith("http")) {
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
   */
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
   */
  public GeoJsonSource(String id, URL url, GeoJsonOptions options) {
    super();
    initialize(id, options);
    nativeSetUrl(url.toExternalForm());
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
   * Updates the GeoJson with a single feature
   *
   * @param feature the GeoJSON {@link Feature} to set
   */
  public void setGeoJson(Feature feature) {
    checkThread();
    nativeSetFeature(feature);
  }

  /**
   * Updates the GeoJson with a single geometry
   *
   * @param geometry the GeoJSON {@link Geometry} to set
   */
  public void setGeoJson(Geometry geometry) {
    checkThread();
    nativeSetGeometry(geometry);
  }

  /**
   * Updates the GeoJson
   *
   * @param features the GeoJSON FeatureCollection
   */
  public void setGeoJson(FeatureCollection features) {
    checkThread();
    nativeSetFeatureCollection(features);
  }

  /**
   * Updates the GeoJson
   *
   * @param json the raw GeoJson FeatureCollection string
   */
  public void setGeoJson(String json) {
    checkThread();
    nativeSetGeoJsonString(json);
  }

  /**
   * Updates the url
   *
   * @param url the GeoJSON FeatureCollection url
   */
  public void setUrl(URL url) {
    checkThread();
    setUrl(url.toExternalForm());
  }

  /**
   * Updates the url
   *
   * @param url the GeoJSON FeatureCollection url
   */
  public void setUrl(String url) {
    checkThread();
    nativeSetUrl(url);
  }

  /**
   * @return The url or null
   */
  @Nullable
  public String getUrl() {
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

  @Keep
  protected native void initialize(String layerId, Object options);

  @Keep
  protected native void nativeSetUrl(String url);

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

  @Keep
  private native Feature[] querySourceFeatures(Object[] filter);

  @Override
  @Keep
  protected native void finalize() throws Throwable;

}
