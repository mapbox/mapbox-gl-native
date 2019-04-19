package com.mapbox.mapboxsdk.module.telemetry;

import android.annotation.SuppressLint;
import android.os.Build;

import com.mapbox.mapboxsdk.BuildConfig;

/**
 * Event will be sent while map is loaded.
 */
@SuppressLint("ParcelCreator")
class MapLoadEvent extends MapBaseEvent {
  private static final String EVENT_NAME = "map.load";
  private final String operatingSystem = "Android - " + Build.VERSION.RELEASE;
  private final String sdkIdentifier = BuildConfig.MAPBOX_SDK_IDENTIFIER;
  private final String sdkVersion = BuildConfig.MAPBOX_SDK_VERSION;
  private final String model = Build.MODEL;
  private final String userId;
  private final String carrier;
  private final String cellularNetworkType;
  private final String orientation;
  private final float resolution;
  private final float accessibilityFontScale;
  private final int batteryLevel;
  private final boolean pluggedIn;
  private final boolean wifi;

  MapLoadEvent(String userId, PhoneState phoneState) {
    super(phoneState);
    this.userId = userId;
    this.batteryLevel = phoneState.getBatteryLevel();
    this.pluggedIn = phoneState.isPluggedIn();
    this.cellularNetworkType = phoneState.getCellularNetworkType();
    this.carrier = phoneState.getCarrier();
    this.resolution = phoneState.getResolution();
    this.accessibilityFontScale = phoneState.getAccessibilityFontScale();
    this.wifi = phoneState.isWifi();
    this.orientation = phoneState.getOrientation();
  }

  @Override
  String getEventName() {
    return EVENT_NAME;
  }

  String getOperatingSystem() {
    return operatingSystem;
  }

  String getSdkIdentifier() {
    return sdkIdentifier;
  }

  String getSdkVersion() {
    return sdkVersion;
  }

  String getModel() {
    return model;
  }

  String getUserId() {
    return userId;
  }

  String getCarrier() {
    return carrier;
  }

  String getCellularNetworkType() {
    return cellularNetworkType;
  }

  String getOrientation() {
    return orientation;
  }

  float getResolution() {
    return resolution;
  }

  float getAccessibilityFontScale() {
    return accessibilityFontScale;
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

    MapLoadEvent that = (MapLoadEvent) o;

    if (Float.compare(that.resolution, resolution) != 0) {
      return false;
    }
    if (Float.compare(that.accessibilityFontScale, accessibilityFontScale) != 0) {
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
    if (!operatingSystem.equals(that.operatingSystem)) {
      return false;
    }
    if (model != null ? !model.equals(that.model) : that.model != null) {
      return false;
    }
    if (userId != null ? !userId.equals(that.userId) : that.userId != null) {
      return false;
    }
    if (carrier != null ? !carrier.equals(that.carrier) : that.carrier != null) {
      return false;
    }
    if (cellularNetworkType != null ? !cellularNetworkType.equals(that.cellularNetworkType) :
      that.cellularNetworkType != null) {
      return false;
    }
    return orientation != null ? orientation.equals(that.orientation) : that.orientation == null;
  }

  @Override
  public int hashCode() {
    int result = operatingSystem != null ? operatingSystem.hashCode() : 0;
    result = 31 * result + sdkIdentifier.hashCode();
    result = 31 * result + sdkVersion.hashCode();
    result = 31 * result + (model != null ? model.hashCode() : 0);
    result = 31 * result + (userId != null ? userId.hashCode() : 0);
    result = 31 * result + (carrier != null ? carrier.hashCode() : 0);
    result = 31 * result + (cellularNetworkType != null ? cellularNetworkType.hashCode() : 0);
    result = 31 * result + (orientation != null ? orientation.hashCode() : 0);
    result = 31 * result + (resolution != +0.0f ? Float.floatToIntBits(resolution) : 0);
    result = 31 * result + (accessibilityFontScale != +0.0f ? Float.floatToIntBits(accessibilityFontScale) : 0);
    result = 31 * result + batteryLevel;
    result = 31 * result + (pluggedIn ? 1 : 0);
    result = 31 * result + (wifi ? 1 : 0);
    return result;
  }

  @Override
  public String toString() {
    return "MapLoadEvent{"
      + ", operatingSystem='" + operatingSystem + '\''
      + ", sdkIdentifier='" + sdkIdentifier + '\''
      + ", sdkVersion='" + sdkVersion + '\''
      + ", model='" + model + '\''
      + ", userId='" + userId + '\''
      + ", carrier='" + carrier + '\''
      + ", cellularNetworkType='" + cellularNetworkType + '\''
      + ", orientation='" + orientation + '\''
      + ", resolution=" + resolution
      + ", accessibilityFontScale=" + accessibilityFontScale
      + ", batteryLevel=" + batteryLevel
      + ", pluggedIn=" + pluggedIn
      + ", wifi=" + wifi
      + '}';
  }
}
