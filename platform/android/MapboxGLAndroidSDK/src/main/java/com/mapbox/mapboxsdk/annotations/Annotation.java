package com.mapbox.mapboxsdk.annotations;

import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;

/**
 * Annotation is an overlay on top of a Map.
 * <p>
 * Known subclasses are {@link Polygon}, {@link Polyline} and {@link Marker}.
 * </p>
 * <p>
 * This class manages attachment to a map and identification, but does not require
 * content to be placed at a geographical point.
 * </p>
 */
public abstract class Annotation implements Comparable<Annotation> {

  /**
   * <p>
   * The annotation id
   * </p>
   * Internal C++ id is stored as unsigned int.
   */
  private long id = -1; // -1 unless added to a MapView
  protected MapboxMap mapboxMap;
  protected MapView mapView;

  protected Annotation() {
  }

  /**
   * <p>
   * Gets the annotation's unique ID.
   * </p>
   * This ID is unique for a MapView instance and is suitable for associating your own extra
   * data with.
   *
   * @return the assigned id.
   */
  public long getId() {
    return id;
  }

  /**
   * Do not use this method, used internally by the SDK.
   */
  public void remove() {
    if (mapboxMap == null) {
      return;
    }
    mapboxMap.removeAnnotation(this);
  }

  /**
   * Do not use this method, used internally by the SDK.
   *
   * @param id the assigned id
   */
  public void setId(long id) {
    this.id = id;
  }

  /**
   * Do not use this method, used internally by the SDK.
   *
   * @param mapboxMap the hosting mapbox map
   */
  public void setMapboxMap(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
  }

  /**
   * Gets the hosting mapbox map.
   *
   * @return the MapboxMap
   */
  protected MapboxMap getMapboxMap() {
    return mapboxMap;
  }

  /**
   * Do not use this method, used internally by the SDK.
   *
   * @param mapView the hosting map view
   */
  public void setMapView(MapView mapView) {
    this.mapView = mapView;
  }

  /**
   * Gets the hosting map view.
   *
   * @return The MapView
   */
  protected MapView getMapView() {
    return mapView;
  }

  /**
   * Compares this Annotation object with another Annotation.
   *
   * @param annotation Another Annotation to compare with this object.
   * @return returns 0 if id's match, 1 if id is lower, -1 if id is higher of another Annotation
   */
  @Override
  public int compareTo(@NonNull Annotation annotation) {
    if (id < annotation.getId()) {
      return 1;
    } else if (id > annotation.getId()) {
      return -1;
    }
    return 0;
  }

  /**
   * Checks if this Annotation object is equal to another Annotation.
   *
   * @param object Another Annotation to check equality with this object.
   * @return returns true both id's match else returns false.
   */
  @Override
  public boolean equals(Object object) {
    if (this == object) {
      return true;
    }
    if (object == null || !(object instanceof Annotation)) {
      return false;
    }
    Annotation that = (Annotation) object;
    return id == that.getId();
  }

  /**
   * Gives an integer which can be used as the bucket number for storing elements of the set/map.
   * This bucket number is the address of the element inside the set/map. There's no guarantee
   * that this hash value will be consistent between different Java implementations, or even
   * between different execution runs of the same program.
   *
   * @return integer value you can use for storing element.
   */
  @Override
  public int hashCode() {
    return (int) (getId() ^ (getId() >>> 32));
  }
}
