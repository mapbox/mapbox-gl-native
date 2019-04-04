package com.mapbox.mapboxsdk.module.telemetry;

import android.content.Context;
import android.os.Build;
import android.os.Parcel;
import android.os.Parcelable;

import com.google.gson.annotations.SerializedName;
import com.mapbox.android.telemetry.Event;
import com.mapbox.android.telemetry.TelemetryUtils;

class MapLoadEvent extends Event implements Parcelable {
  private static final String MAP_LOAD = "map.load";
  private static final String OPERATING_SYSTEM = "Android - " + Build.VERSION.RELEASE;

  @SerializedName("event")
  private final String event;
  @SerializedName("created")
  private String created;
  @SerializedName("userId")
  private String userId;
  @SerializedName("model")
  private String model = null;
  @SerializedName("operatingSystem")
  private String operatingSystem = null;
  @SerializedName("resolution")
  private Float resolution = null;
  @SerializedName("accessibilityFontScale")
  private Float accessibilityFontScale = null;
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
  @SerializedName("sdkIdentifier")
  private String sdkIdentifier = null;
  @SerializedName("sdkVersion")
  private String sdkVersion = null;

  MapLoadEvent(String userId) {
    this.event = MAP_LOAD;
    this.model = Build.MODEL;
    this.operatingSystem = OPERATING_SYSTEM;
    this.created = TelemetryUtils.obtainCurrentDate();
    this.userId = userId;
    this.batteryLevel = 0;
    this.pluggedIn = false;
    this.cellularNetworkType = "";
  }

  MapLoadEvent setDeviceInfo(Context context) {
    this.batteryLevel = TelemetryUtils.obtainBatteryLevel(context);
    this.pluggedIn = TelemetryUtils.isPluggedIn(context);
    this.cellularNetworkType = TelemetryUtils.obtainCellularNetworkType(context);
    return this;
  }

  void setResolution(float resolution) {
    this.resolution = resolution;
  }

  void setAccessibilityFontScale(float accessibilityFontScale) {
    this.accessibilityFontScale = accessibilityFontScale;
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

  void setSdkIdentifier(String sdkIdentifier) {
    this.sdkIdentifier = sdkIdentifier;
  }

  void setSdkVersion(String sdkVersion) {
    this.sdkVersion = sdkVersion;
  }

  private MapLoadEvent(Parcel in) {
    event = in.readString();
    created = in.readString();
    userId = in.readString();
    model = in.readString();
    operatingSystem = in.readString();
    resolution = in.readByte() == 0x00 ? null : in.readFloat();
    accessibilityFontScale = in.readByte() == 0x00 ? null : in.readFloat();
    orientation = in.readString();
    batteryLevel = in.readInt();
    pluggedIn = in.readByte() != 0x00;
    carrier = in.readString();
    cellularNetworkType = in.readString();
    byte wifiVal = in.readByte();
    wifi = wifiVal == 0x02 ? null : wifiVal != 0x00;
    sdkIdentifier = in.readString();
    sdkVersion = in.readString();
  }

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel dest, int flags) {
    dest.writeString(event);
    dest.writeString(created);
    dest.writeString(userId);
    dest.writeString(model);
    dest.writeString(operatingSystem);
    if (resolution == null) {
      dest.writeByte((byte) (0x00));
    } else {
      dest.writeByte((byte) (0x01));
      dest.writeFloat(resolution);
    }
    if (accessibilityFontScale == null) {
      dest.writeByte((byte) (0x00));
    } else {
      dest.writeByte((byte) (0x01));
      dest.writeFloat(accessibilityFontScale);
    }
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
    dest.writeString(sdkIdentifier);
    dest.writeString(sdkVersion);
  }

  @SuppressWarnings("unused")
  public static final Creator<MapLoadEvent> CREATOR = new Creator<MapLoadEvent>() {
    @Override
    public MapLoadEvent createFromParcel(Parcel in) {
      return new MapLoadEvent(in);
    }

    @Override
    public MapLoadEvent[] newArray(int size) {
      return new MapLoadEvent[size];
    }
  };
}
