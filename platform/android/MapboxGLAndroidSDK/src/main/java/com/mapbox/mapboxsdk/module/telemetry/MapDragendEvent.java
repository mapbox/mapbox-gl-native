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
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    MapDragendEvent that = (MapDragendEvent) o;

    if (batteryLevel != that.batteryLevel) {
      return false;
    }
    if (Double.compare(that.lat, lat) != 0) {
      return false;
    }
    if (Double.compare(that.lng, lng) != 0) {
      return false;
    }
    if (Double.compare(that.zoom, zoom) != 0) {
      return false;
    }
    if (pluggedIn != that.pluggedIn) {
      return false;
    }
    if (wifi != that.wifi) {
      return false;
    }
    if (created != null ? !created.equals(that.created) : that.created != null) {
      return false;
    }
    if (orientation != null ? !orientation.equals(that.orientation) : that.orientation != null) {
      return false;
    }
    if (carrier != null ? !carrier.equals(that.carrier) : that.carrier != null) {
      return false;
    }
    return cellularNetworkType != null ? cellularNetworkType.equals(that.cellularNetworkType) :
      that.cellularNetworkType == null;
  }

  @Override
  public int hashCode() {
    int result;
    long temp;
    result = event.hashCode();
    result = 31 * result + (created != null ? created.hashCode() : 0);
    result = 31 * result + (orientation != null ? orientation.hashCode() : 0);
    result = 31 * result + (carrier != null ? carrier.hashCode() : 0);
    result = 31 * result + (cellularNetworkType != null ? cellularNetworkType.hashCode() : 0);
    result = 31 * result + batteryLevel;
    temp = Double.doubleToLongBits(lat);
    result = 31 * result + (int) (temp ^ (temp >>> 32));
    temp = Double.doubleToLongBits(lng);
    result = 31 * result + (int) (temp ^ (temp >>> 32));
    temp = Double.doubleToLongBits(zoom);
    result = 31 * result + (int) (temp ^ (temp >>> 32));
    result = 31 * result + (pluggedIn ? 1 : 0);
    result = 31 * result + (wifi ? 1 : 0);
    return result;
  }

  @Override
  public String toString() {
    return "MapDragendEvent{"
      + "event='" + event + '\''
      + ", created='" + created + '\''
      + ", orientation='" + orientation + '\''
      + ", carrier='" + carrier + '\''
      + ", cellularNetworkType='" + cellularNetworkType + '\''
      + ", batteryLevel=" + batteryLevel
      + ", lat=" + lat
      + ", lng=" + lng
      + ", zoom=" + zoom
      + ", pluggedIn=" + pluggedIn
      + ", wifi=" + wifi
      + '}';
  }

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel dest, int flags) {
  }
}