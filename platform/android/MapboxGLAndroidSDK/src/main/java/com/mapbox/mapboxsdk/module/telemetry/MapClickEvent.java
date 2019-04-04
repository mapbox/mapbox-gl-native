package com.mapbox.mapboxsdk.module.telemetry;

import android.content.Context;
import android.os.Parcel;
import android.os.Parcelable;

import com.google.gson.annotations.SerializedName;
import com.mapbox.android.telemetry.Event;
import com.mapbox.android.telemetry.TelemetryUtils;

class MapClickEvent extends Event implements Parcelable {
  private static final String MAP_CLICK = "map.click";

  @SerializedName("event")
  private final String event;
  @SerializedName("created")
  private String created;
  @SerializedName("gesture")
  private final String gesture;
  @SerializedName("lat")
  private double latitude;
  @SerializedName("lng")
  private double longitude;
  @SerializedName("zoom")
  private double zoom;
  @SerializedName("orientation")
  private String orientation = null;
  @SerializedName("batteryLevel")
  private Integer batteryLevel;
  @SerializedName("pluggedIn")
  private Boolean pluggedIn;
  @SerializedName("carrier")
  private String carrier = null;
  @SerializedName("cellularNetworkType")
  private String cellularNetworkType;
  @SerializedName("wifi")
  private Boolean wifi = null;

  MapClickEvent(MapState mapState) {
    this.event = MAP_CLICK;
    this.gesture = mapState.getGesture();
    this.latitude = mapState.getLatitude();
    this.longitude = mapState.getLongitude();
    this.zoom = mapState.getZoom();
    this.created = TelemetryUtils.obtainCurrentDate();
    this.batteryLevel = 0;
    this.pluggedIn = false;
    this.cellularNetworkType = "";
  }

  MapClickEvent setDeviceInfo(Context context) {
    this.batteryLevel = TelemetryUtils.obtainBatteryLevel(context);
    this.pluggedIn = TelemetryUtils.isPluggedIn(context);
    this.cellularNetworkType = TelemetryUtils.obtainCellularNetworkType(context);
    return this;
  }

  void setOrientation(String orientation) {
    this.orientation = orientation;
  }

  void setCarrier(String carrier) {
    this.carrier = carrier;
  }

  void setWifi(boolean wifi) {
    this.wifi = wifi;
  }

  private MapClickEvent(Parcel in) {
    event = in.readString();
    created = in.readString();
    gesture = in.readString();
    latitude = in.readDouble();
    longitude = in.readDouble();
    zoom = in.readDouble();
    orientation = in.readString();
    batteryLevel = in.readInt();
    pluggedIn = in.readByte() != 0x00;
    carrier = in.readString();
    cellularNetworkType = in.readString();
    byte wifiVal = in.readByte();
    wifi = wifiVal == 0x02 ? null : wifiVal != 0x00;
  }

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel dest, int flags) {
    dest.writeString(event);
    dest.writeString(created);
    dest.writeString(gesture);
    dest.writeDouble(latitude);
    dest.writeDouble(longitude);
    dest.writeDouble(zoom);
    dest.writeString(orientation);
    dest.writeInt(batteryLevel);
    dest.writeByte((byte) (pluggedIn ? 0x01 : 0x00));
    dest.writeString(carrier);
    dest.writeString(cellularNetworkType);
    if (wifi == null) {
      dest.writeByte((byte) (0x02));
    } else {
      dest.writeByte((byte) (wifi ? 0x01 : 0x00));
    }
  }

  public static final Creator<MapClickEvent> CREATOR = new Creator<MapClickEvent>() {
    @Override
    public MapClickEvent createFromParcel(Parcel in) {
      return new MapClickEvent(in);
    }

    @Override
    public MapClickEvent[] newArray(int size) {
      return new MapClickEvent[size];
    }
  };
}
