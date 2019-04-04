package com.mapbox.mapboxsdk.module.telemetry;

import android.content.Context;
import android.content.res.Configuration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.support.annotation.NonNull;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.view.WindowManager;

import com.mapbox.android.telemetry.Event;
import com.mapbox.android.telemetry.TelemetryUtils;

import java.util.HashMap;
import java.util.Map;

public class MapEventFactory {
  private static final String LANDSCAPE = "Landscape";
  private static final String PORTRAIT = "Portrait";
  private static final String NO_CARRIER = "EMPTY_CARRIER";
  private static final int NO_NETWORK = -1;

  private static final Map<Integer, String> ORIENTATIONS = new HashMap<Integer, String>() {
    {
      put(Configuration.ORIENTATION_LANDSCAPE, LANDSCAPE);
      put(Configuration.ORIENTATION_PORTRAIT, PORTRAIT);
    }
  };

  private final Context appContext;

  MapEventFactory(Context appContext) {
   this.appContext = appContext;
  }

  Event buildMapLoadEvent() {
    String userId = TelemetryUtils.retrieveVendorId();
    MapLoadEvent mapLoadEvent = new MapLoadEvent(userId).setDeviceInfo(appContext);
    mapLoadEvent.setOrientation(obtainOrientation(appContext));
    mapLoadEvent.setAccessibilityFontScale(obtainAccessibilityFontScaleSize(appContext));
    mapLoadEvent.setCarrier(obtainCellularCarrier(appContext));
    mapLoadEvent.setResolution(obtainDisplayDensity(appContext));
    mapLoadEvent.setWifi(isConnectedToWifi(appContext));
    return mapLoadEvent;
  }

  Event buildOfflineDownloadStartEvent(String shapeForOfflineRegion,
                                       Double minZoom, Double maxZoom,
                                       String styleURL) {

    OfflineDownloadStartEvent offlineEvent =
      new OfflineDownloadStartEvent(shapeForOfflineRegion, minZoom, maxZoom);
    offlineEvent.setStyleURL(styleURL);
    return offlineEvent;
  }

  public Event buildOfflineDownloadCompleteEvent(String shapeForOfflineRegion,
                                                 Double minZoom, Double maxZoom,
                                                 String styleURL,
                                                 Long sizeOfResourcesCompleted,
                                                 Long numberOfTilesCompleted,
                                                 String state) {

    OfflineDownloadEndEvent offlineEvent =
      new OfflineDownloadEndEvent(shapeForOfflineRegion, minZoom, maxZoom);
    offlineEvent.setStyleURL(styleURL);
    offlineEvent.setSizeOfResourcesCompleted(sizeOfResourcesCompleted);
    offlineEvent.setNumberOfTilesCompleted(numberOfTilesCompleted);
    return offlineEvent;
  }

  MapClickEvent buildMapClickEvent(@NonNull MapState mapState) {
    MapClickEvent mapClickEvent = new MapClickEvent(mapState).setDeviceInfo(appContext);
    mapClickEvent.setOrientation(obtainOrientation(appContext));
    mapClickEvent.setCarrier(obtainCellularCarrier(appContext));
    mapClickEvent.setWifi(isConnectedToWifi(appContext));

    return mapClickEvent;
  }

  MapDragendEvent buildMapDragendEvent(@NonNull MapState mapState) {
    MapDragendEvent mapDragendEvent = new MapDragendEvent(mapState).setDeviceInfo(appContext);
    mapDragendEvent.setOrientation(obtainOrientation(appContext));
    mapDragendEvent.setCarrier(obtainCellularCarrier(appContext));
    mapDragendEvent.setWifi(isConnectedToWifi(appContext));

    return mapDragendEvent;
  }

  private String obtainOrientation(Context context) {
    return ORIENTATIONS.get(context.getResources().getConfiguration().orientation);
  }

  private float obtainAccessibilityFontScaleSize(Context context) {
    return context.getResources().getConfiguration().fontScale;
  }

  private String obtainCellularCarrier(Context context) {
    TelephonyManager manager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
    if(manager == null) return NO_CARRIER;
    String carrierName = manager.getNetworkOperatorName();

    if (TextUtils.isEmpty(carrierName)) {
      return NO_CARRIER;
    }

    return carrierName;
  }

  private float obtainDisplayDensity(Context context) {
    DisplayMetrics displayMetrics = new DisplayMetrics();
    ((WindowManager) context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getMetrics(displayMetrics);

    return displayMetrics.density;
  }

  private boolean isConnectedToWifi(Context context) {
    try {
      WifiManager wifiMgr = (WifiManager) context.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
      //noinspection MissingPermission
      WifiInfo wifiInfo = wifiMgr.getConnectionInfo();

      return isWifiConnected(wifiMgr, wifiInfo);
    } catch (Exception exception) {
      return false;
    }
  }

  private boolean isWifiConnected(WifiManager wifiMgr, WifiInfo wifiInfo) {
    return wifiMgr.isWifiEnabled() && networkConnected(wifiInfo);
  }

  private boolean networkConnected(WifiInfo wifiInfo) {
    return wifiInfo.getNetworkId() != NO_NETWORK;
  }

}
