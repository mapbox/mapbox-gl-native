package com.mapbox.mapboxsdk.style.sources;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;

import com.mapbox.mapboxsdk.style.layers.Filter;
import com.mapbox.services.commons.geojson.Feature;
import com.mapbox.services.commons.geojson.FeatureCollection;

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
  public GeoJsonSource(long nativePtr) {
    super(nativePtr);
  }

  /**
   * Create an empty GeoJsonSource
   *
   * @param id the source id
   */
  public GeoJsonSource(String id) {
    initialize(id, null);
    setGeoJson(FeatureCollection.fromFeatures(new ArrayList<Feature>()));
  }

  /**
   * Create an empty GeoJsonSource with non-default {@link GeoJsonOptions}
   *
   * @param id      the source id
   * @param options options
   */
  public GeoJsonSource(String id, GeoJsonOptions options) {
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
    if (geoJson == null || geoJson.startsWith("http")) {
      throw new IllegalArgumentException("Expected a raw json body");
    }
    initialize(id, null);
    setGeoJson(geoJson);
  }

  /**
   * Create a GeoJsonSource from a raw json string and non-default {@link GeoJsonOptions}
   *
   * @param id      the source id
   * @param geoJson raw Json body
   * @param options options
   */
  public GeoJsonSource(String id, String geoJson, GeoJsonOptions options) {
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
    initialize(id, null);
    nativeSetUrl(url.toExternalForm());
  }

  /**
   * Create a GeoJsonSource from a remote geo json file and non-default {@link GeoJsonOptions}
   *
   * @param id      the source id
   * @param url     remote json file
   * @param options options
   */
  public GeoJsonSource(String id, URL url, GeoJsonOptions options) {
    initialize(id, options);
    nativeSetUrl(url.toExternalForm());
  }

  /**
   * Create a GeoJsonSource from a {@link FeatureCollection}
   *
   * @param id       the source id
   * @param features the features
   */
  public GeoJsonSource(String id, FeatureCollection features) {
    initialize(id, null);
    setGeoJson(features);
  }

  /**
   * Create a GeoJsonSource from a {@link FeatureCollection} and non-default {@link GeoJsonOptions}
   *
   * @param id       the source id
   * @param features the features
   * @param options  options
   */
  public GeoJsonSource(String id, FeatureCollection features, GeoJsonOptions options) {
    initialize(id, options);
    setGeoJson(features);
  }

  /**
   * Updates the GeoJson
   *
   * @param features the GeoJSON {@link FeatureCollection}
   */
  public void setGeoJson(FeatureCollection features) {
    nativeSetFeatureCollection(features);
  }

  /**
   * Updates the GeoJson
   *
   * @param json the raw GeoJson FeatureCollection string
   */
  public void setGeoJson(String json) {
    nativeSetGeoJsonString(json);
  }

  /**
   * Updates the url
   *
   * @param url the GeoJSON FeatureCollection url
   */
  public void setUrl(URL url) {
    setUrl(url.toExternalForm());
  }

  /**
   * Updates the url
   *
   * @param url the GeoJSON FeatureCollection url
   */
  public void setUrl(String url) {
    nativeSetUrl(url);
  }

  /**
   * Queries the source for features.
   *
   * @param filter an optional filter statement to filter the returned Features
   * @return the features
   */
  @NonNull
  public List<Feature> querySourceFeatures(@Nullable Filter.Statement filter) {
    Feature[] features = querySourceFeatures(filter != null ? filter.toArray() : null);
    return features != null ? Arrays.asList(features) : new ArrayList<Feature>();
  }

  protected native void initialize(String layerId, Object options);

  protected native void nativeSetUrl(String url);

  private native void nativeSetGeoJsonString(String geoJson);

  private native void nativeSetFeatureCollection(FeatureCollection geoJson);

  private native Feature[] querySourceFeatures(Object[] filter);

  @Override
  protected native void finalize() throws Throwable;

}
