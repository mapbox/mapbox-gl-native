package com.mapbox.mapboxsdk.module.telemetry;

import android.annotation.SuppressLint;
import android.os.Parcel;

import com.mapbox.android.telemetry.Event;

/**
 * When user drag map should send this event.
 */
@SuppressLint("ParcelCreator")
class MapDragendEvent extends Event {
  private final String event = "map.dragend";
  private final String created;
  private final String orientation;
  private final String carrier;
  private final String cellularNetworkType;
  private final int batteryLevel;
  private final double lat;
  private final double lng;
  private final double zoom;
  private final boolean pluggedIn;
  private final boolean wifi;

  MapDragendEvent(PhoneState phoneState, MapState mapState) {
    this.lat = mapState.getLatitude();
    this.lng = mapState.getLongitude();
    this.zoom = mapState.getZoom();
    this.created = phoneState.getCreated();
    this.batteryLevel = phoneState.getBatteryLevel();
    this.pluggedIn = phoneState.isPluggedIn();
    this.cellularNetworkType = phoneState.getCellularNetworkType();
    this.wifi = phoneState.isWifi();
    this.orientation = phoneState.getOrientation();
    this.carrier = phoneState.getCarrier();
  }

  String getCreated() {
    return created;
  }

  String getOrientation() {
    return orientation;
  }

  String getCarrier() {
    return carrier;
  }

  String getCellularNetworkType() {
    return cellularNetworkType;
  }

  int getBatteryLevel() {
    return batteryLevel;
  }

  double getLat() {
    return lat;
  }

  double getLng() {
    return lng;
  }

  double getZoom() {
    return zoom;
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