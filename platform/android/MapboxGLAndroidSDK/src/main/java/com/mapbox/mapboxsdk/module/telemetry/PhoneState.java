package com.mapbox.mapboxsdk.module.telemetry;

import android.content.Context;
import android.content.res.Configuration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import androidx.annotation.NonNull;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.view.WindowManager;

import com.mapbox.android.telemetry.TelemetryUtils;

/**
 * Class that holds kinds of states of the current phone.
 */
class PhoneState {
  private static final String NO_CARRIER = "EMPTY_CARRIER";
  private static final int NO_NETWORK = -1;
  private String created;
  private String cellularNetworkType;
  private Orientation orientation;
  private String carrier;
  private int batteryLevel;
  private boolean pluggedIn;
  private boolean wifi;
  private float accessibilityFontScale;
  private float resolution;

  PhoneState() {

  }

  PhoneState(@NonNull Context context) {
    this.created = TelemetryUtils.obtainCurrentDate();
    this.batteryLevel = TelemetryUtils.obtainBatteryLevel(context);
    this.pluggedIn = TelemetryUtils.isPluggedIn(context);
    this.cellularNetworkType = TelemetryUtils.obtainCellularNetworkType(context);
    this.orientation = Orientation.getOrientation(context.getResources().getConfiguration().orientation);
    this.accessibilityFontScale = context.getResources().getConfiguration().fontScale;
    this.carrier = obtainCellularCarrier(context);
    this.resolution = obtainDisplayDensity(context);
    this.wifi = isConnectedToWifi(context);
  }

  private String obtainCellularCarrier(@NonNull Context context) {
    TelephonyManager manager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
    if (manager == null) {
      return NO_CARRIER;
    }
    String carrierName = manager.getNetworkOperatorName();
    if (TextUtils.isEmpty(carrierName)) {
      return NO_CARRIER;
    }
    return carrierName;
  }

  private float obtainDisplayDensity(@NonNull Context context) {
    DisplayMetrics displayMetrics = new DisplayMetrics();
    ((WindowManager) context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getMetrics(displayMetrics);
    return displayMetrics.density;
  }

  private boolean isConnectedToWifi(@NonNull Context context) {
    try {
      WifiManager wifiMgr = (WifiManager) context.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
      if (wifiMgr == null) {
        return false;
      }
      //noinspection MissingPermission
      WifiInfo wifiInfo = wifiMgr.getConnectionInfo();
      return wifiMgr.isWifiEnabled() && wifiInfo.getNetworkId() != NO_NETWORK;
    } catch (Exception exception) {
      return false;
    }
  }

  String getCreated() {
    return created;
  }

  void setCreated(String created) {
    this.created = created;
  }

  int getBatteryLevel() {
    return batteryLevel;
  }

  void setBatteryLevel(int batteryLevel) {
    this.batteryLevel = batteryLevel;
  }

  boolean isPluggedIn() {
    return pluggedIn;
  }

  void setPluggedIn(boolean pluggedIn) {
    this.pluggedIn = pluggedIn;
  }

  String getCellularNetworkType() {
    return cellularNetworkType;
  }

  void setCellularNetworkType(String cellularNetworkType) {
    this.cellularNetworkType = cellularNetworkType;
  }

  String getOrientation() {
    return orientation.getOrientation();
  }

  void setOrientation(Orientation orientation) {
    this.orientation = orientation;
  }

  String getCarrier() {
    return carrier;
  }

  void setCarrier(String carrier) {
    this.carrier = carrier;
  }

  boolean isWifi() {
    return wifi;
  }

  void setWifi(boolean wifi) {
    this.wifi = wifi;
  }

  float getAccessibilityFontScale() {
    return accessibilityFontScale;
  }

  void setAccessibilityFontScale(float accessibilityFontScale) {
    this.accessibilityFontScale = accessibilityFontScale;
  }

  float getResolution() {
    return resolution;
  }

  void setResolution(float resolution) {
    this.resolution = resolution;
  }

  enum Orientation {
    ORIENTATION_PORTRAIT("Portrait"),
    ORIENTATION_LANDSCAPE("Landscape");
    private String orientation;

    Orientation(String orientation) {
      this.orientation = orientation;
    }

    public static Orientation getOrientation(int index) {
      if (Configuration.ORIENTATION_PORTRAIT == index) {
        return ORIENTATION_PORTRAIT;
      }
      return ORIENTATION_LANDSCAPE;
    }

    public String getOrientation() {
      return orientation;
    }
  }
}
