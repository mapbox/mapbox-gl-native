package com.mapbox.mapboxsdk.module.telemetry;

import android.annotation.SuppressLint;
import android.os.Parcel;

import com.mapbox.android.telemetry.Event;

/**
 * Event that represents users' gestures on map, for the details of gestures,
 * please refer to {@link com.mapbox.mapboxsdk.constants.TelemetryConstants}
 */
@SuppressLint("ParcelCreator")
class MapClickEvent extends Event {
  private final String event = "map.click";
  private final String created;
  private final String gesture;
  private final String cellularNetworkType;
  private final String carrier;
  private final String orientation;
  private final double latitude;
  private final double longitude;
  private final double zoom;
  private final int batteryLevel;
  private final boolean pluggedIn;
  private final boolean wifi;

  MapClickEvent(PhoneState phoneState, MapState mapState) {
    this.gesture = mapState.getGesture();
    this.latitude = mapState.getLatitude();
    this.longitude = mapState.getLongitude();
    this.zoom = mapState.getZoom();
    this.created = phoneState.getCreated();
    this.batteryLevel = phoneState.getBatteryLevel();
    this.pluggedIn = phoneState.isPluggedIn();
    this.cellularNetworkType = phoneState.getCellularNetworkType();
    this.orientation = phoneState.getOrientation();
    this.carrier = phoneState.getCarrier();
    this.wifi = phoneState.isWifi();
  }

  String getCreated() {
    return created;
  }

  String getGesture() {
    return gesture;
  }

  String getCellularNetworkType() {
    return cellularNetworkType;
  }

  String getCarrier() {
    return carrier;
  }

  String getOrientation() {
    return orientation;
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

  int getBatteryLevel() {
    return batteryLevel;
  }

  boolean isPluggedIn() {
    return pluggedIn;
  }

  boolean isWifi() {
    return wifi;
  }

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel dest, int flags) {
  }
}
