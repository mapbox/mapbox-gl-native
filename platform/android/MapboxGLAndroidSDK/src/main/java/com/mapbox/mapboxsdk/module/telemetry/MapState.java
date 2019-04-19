package com.mapbox.mapboxsdk.module.telemetry;

import android.support.annotation.FloatRange;

import com.mapbox.mapboxsdk.constants.MapboxConstants;

public class MapState {
  private double latitude;
  private double longitude;
  private double zoom;
  private String gesture;

  MapState(double latitude, double longitude, @FloatRange(from = MapboxConstants.MINIMUM_ZOOM,
    to = MapboxConstants.MAXIMUM_ZOOM) double zoom) {
    this.latitude = latitude;
    this.longitude = longitude;
    this.zoom = zoom;
  }

  String getGesture() {
    return gesture;
  }

  void setGesture(String gesture) {
    this.gesture = gesture;
  }

  double getLatitude() {
    return latitude;
  }

  double getLongitude() {
    return longitude;
  }

  double getZoom() {
    return zoom;
  }
}

