package com.mapbox.mapboxsdk.module.telemetry;

import android.annotation.SuppressLint;

/**
 * Event that represents users' gestures on map, for the details of gestures,
 * please refer to {@link com.mapbox.mapboxsdk.constants.TelemetryConstants}
 */
@SuppressLint("ParcelCreator")
class MapClickEvent extends MapBaseEvent {
  private static final String EVENT_NAME = "map.click";
  private final String gesture;
  private final String cellularNetworkType;
  private final String carrier;
  private final String orientation;
  private final double lat;
  private final double lng;
  private final double zoom;
  private final int batteryLevel;
  private final boolean pluggedIn;
  private final boolean wifi;

  MapClickEvent(PhoneState phoneState, MapState mapState) {
    super(phoneState);
    this.gesture = mapState.getGesture();
    this.lat = mapState.getLatitude();
    this.lng = mapState.getLongitude();
    this.zoom = mapState.getZoom();
    this.batteryLevel = phoneState.getBatteryLevel();
    this.pluggedIn = phoneState.isPluggedIn();
    this.cellularNetworkType = phoneState.getCellularNetworkType();
    this.orientation = phoneState.getOrientation();
    this.carrier = phoneState.getCarrier();
    this.wifi = phoneState.isWifi();
  }

  @Override
  String getEventName() {
    return EVENT_NAME;
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

  double getLat() {
    return lat;
  }

  double getLng() {
    return lng;
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
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    MapClickEvent that = (MapClickEvent) o;

    if (Double.compare(that.lat, lat) != 0) {
      return false;
    }
    if (Double.compare(that.lng, lng) != 0) {
      return false;
    }
    if (Double.compare(that.zoom, zoom) != 0) {
      return false;
    }
    if (batteryLevel != that.batteryLevel) {
      return false;
    }
    if (pluggedIn != that.pluggedIn) {
      return false;
    }
    if (wifi != that.wifi) {
      return false;
    }
    if (gesture != null ? !gesture.equals(that.gesture) : that.gesture != null) {
      return false;
    }
    if (cellularNetworkType != null ? !cellularNetworkType.equals(that.cellularNetworkType) :
      that.cellularNetworkType != null) {
      return false;
    }
    if (carrier != null ? !carrier.equals(that.carrier) : that.carrier != null) {
      return false;
    }
    return orientation != null ? orientation.equals(that.orientation) : that.orientation == null;
  }

  @Override
  public int hashCode() {
    int result;
    long temp;
    result = gesture != null ? gesture.hashCode() : 0;
    result = 31 * result + (cellularNetworkType != null ? cellularNetworkType.hashCode() : 0);
    result = 31 * result + (carrier != null ? carrier.hashCode() : 0);
    result = 31 * result + (orientation != null ? orientation.hashCode() : 0);
    temp = Double.doubleToLongBits(lat);
    result = 31 * result + (int) (temp ^ (temp >>> 32));
    temp = Double.doubleToLongBits(lng);
    result = 31 * result + (int) (temp ^ (temp >>> 32));
    temp = Double.doubleToLongBits(zoom);
    result = 31 * result + (int) (temp ^ (temp >>> 32));
    result = 31 * result + batteryLevel;
    result = 31 * result + (pluggedIn ? 1 : 0);
    result = 31 * result + (wifi ? 1 : 0);
    return result;
  }

  @Override
  public String toString() {
    return "MapClickEvent{"
      + ", gesture='" + gesture + '\''
      + ", cellularNetworkType='" + cellularNetworkType + '\''
      + ", carrier='" + carrier + '\''
      + ", orientation='" + orientation + '\''
      + ", lat=" + lat
      + ", lng=" + lng
      + ", zoom=" + zoom
      + ", batteryLevel=" + batteryLevel
      + ", pluggedIn=" + pluggedIn
      + ", wifi=" + wifi
      + '}';
  }

}
