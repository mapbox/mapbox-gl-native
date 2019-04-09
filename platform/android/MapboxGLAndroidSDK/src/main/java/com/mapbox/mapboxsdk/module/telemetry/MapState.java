package com.mapbox.mapboxsdk.module.telemetry;

public class MapState {
  private double latitude;
  private double longitude;
  private double zoom;
  private String gesture;

  MapState(double latitude, double longitude, double zoom) {
    this.latitude = latitude;
    this.longitude = longitude;
    this.zoom = zoom;
  }

  void setGesture(String gesture) {
    this.gesture = gesture;
  }

  String getGesture() {
    return gesture;
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

