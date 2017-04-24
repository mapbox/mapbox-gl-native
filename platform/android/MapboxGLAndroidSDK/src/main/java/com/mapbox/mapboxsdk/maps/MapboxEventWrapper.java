package com.mapbox.mapboxsdk.maps;

import android.location.Location;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.mapbox.services.android.telemetry.MapboxEvent;

import java.util.Hashtable;

/**
 * Wrapper class for MapboxEvent
 * <p>
 * Provides facility methods to use Transform and handle the case that the zoom, required for a telemetry event,
 * isn't available yet.
 * </p>
 */
class MapboxEventWrapper {

  @Nullable
  static Hashtable<String, Object> buildMapClickEvent(
    @NonNull Location location, @NonNull String gestureId, Transform transform) {
    try {
      return MapboxEvent.buildMapClickEvent(location, gestureId, transform.getZoom());
    } catch (NullPointerException exception) {
      // Map/Transform is not ready yet #8650
      // returning null is valid, event is ignored.
      return null;
    }
  }

  @Nullable
  static Hashtable<String, Object> buildMapDragEndEvent(
    @NonNull Location location, Transform transform) {
    try {
      return MapboxEvent.buildMapDragEndEvent(location, transform.getZoom());
    } catch (NullPointerException exception) {
      // Map/Transform is not ready yet #8650
      // returning null is valid, event is ignored.
      return null;
    }
  }

  @Nullable
  static Hashtable<String, Object> buildMapLoadEvent() {
    return MapboxEvent.buildMapLoadEvent();
  }
}
