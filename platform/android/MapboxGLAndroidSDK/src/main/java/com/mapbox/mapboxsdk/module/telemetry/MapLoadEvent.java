package com.mapbox.mapboxsdk.module.telemetry;

import android.annotation.SuppressLint;
import android.os.Build;
import android.os.Parcel;

import com.mapbox.android.telemetry.Event;
import com.mapbox.mapboxsdk.BuildConfig;

/**
 * Event will be sent while map is loaded.
 */
@SuppressLint("ParcelCreator")
class MapLoadEvent extends Event {
  private final String event = "map.load";
  private final String operatingSystem = "Android - " + Build.VERSION.RELEASE;
  private final String sdkIdentifier = BuildConfig.MAPBOX_SDK_IDENTIFIER;
  private final String sdkVersion = BuildConfig.MAPBOX_SDK_VERSION;
  private final String model = Build.MODEL;
  private final String created;
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
    this.created = phoneState.getCreated();
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

  String getCreated() {
    return created;
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
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel dest, int flags) {
  }
}
