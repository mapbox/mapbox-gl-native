package com.mapbox.mapboxsdk.module.telemetry;

import android.content.Context;
import android.content.res.Configuration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.view.WindowManager;

import com.mapbox.android.telemetry.Event;
import com.mapbox.android.telemetry.MapBuildEvent;
import com.mapbox.android.telemetry.MapClickEvent;
import com.mapbox.android.telemetry.MapDragendEvent;
import com.mapbox.android.telemetry.MapLoadEvent;
import com.mapbox.android.telemetry.MapState;
import com.mapbox.android.telemetry.MapboxTelemetry;
import com.mapbox.android.telemetry.OfflineDownloadEndEvent;
import com.mapbox.android.telemetry.OfflineDownloadStartEvent;
import com.mapbox.android.telemetry.TelemetryUtils;

import java.util.HashMap;
import java.util.Map;

public class MapEventFactory {
  private static final String APPLICATION_CONTEXT_CANT_BE_NULL = "Create a MapboxTelemetry instance before calling "
    + "this method.";
  private static final String LANDSCAPE = "Landscape";
  private static final String PORTRAIT = "Portrait";
  private static final String NO_CARRIER = "EMPTY_CARRIER";
  private static final int NO_NETWORK = -1;
  private static final String NOT_A_LOAD_MAP_EVENT_TYPE = "Type must be a load map event.";
  private static final String NOT_A_GESTURE_MAP_EVENT_TYPE = "Type must be a gesture map event.";
  private static final String NOT_OFFLINEDOWNLOAD_EVENT_TYPE = "Type must be an offline download map event.";
  private static final String MAP_STATE_ILLEGAL_NULL = "MapState cannot be null.";

  private static final Map<Integer, String> ORIENTATIONS = new HashMap<Integer, String>() {
    {
      put(Configuration.ORIENTATION_LANDSCAPE, LANDSCAPE);
      put(Configuration.ORIENTATION_PORTRAIT, PORTRAIT);
    }
  };
  private final Map<Event.Type, MapBuildEvent> BUILD_EVENT_MAP_GESTURE = new HashMap<Event.Type, MapBuildEvent>() {
    {
      put(Event.Type.MAP_CLICK, new MapBuildEvent() {
        @Override
        public Event build(MapState mapState) {
          return buildMapClickEvent(mapState);
        }
      });
      put(Event.Type.MAP_DRAGEND, new MapBuildEvent() {
        @Override
        public Event build(MapState mapState) {
          return buildMapDragendEvent(mapState);
        }
      });
    }
  };

  public MapEventFactory() {
    if (MapboxTelemetry.applicationContext == null) {
      throw new IllegalStateException(APPLICATION_CONTEXT_CANT_BE_NULL);
    }
  }

  public Event createMapLoadEvent(Event.Type type) {
    checkLoad(type);
    return buildMapLoadEvent();
  }

  public Event createMapGestureEvent(Event.Type type, MapState mapState) {
    checkGesture(type, mapState);
    return BUILD_EVENT_MAP_GESTURE.get(type).build(mapState);
  }

  public Event createOfflineDownloadStartEvent(String shapeForOfflineRegion,
                                               Double minZoom, Double maxZoom,
                                               String styleURL) {

    OfflineDownloadStartEvent offlineEvent =
      new OfflineDownloadStartEvent(shapeForOfflineRegion, minZoom, maxZoom);
    offlineEvent.setStyleURL(styleURL);
    return offlineEvent;
  }

  public Event createOfflineDownloadCompleteEvent(String shapeForOfflineRegion,
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

  private MapClickEvent buildMapClickEvent(MapState mapState) {
    MapClickEvent mapClickEvent = new MapClickEvent(mapState).setDeviceInfo(MapboxTelemetry.applicationContext);
    mapClickEvent.setOrientation(obtainOrientation(MapboxTelemetry.applicationContext));
    mapClickEvent.setCarrier(obtainCellularCarrier(MapboxTelemetry.applicationContext));
    mapClickEvent.setWifi(obtainConnectedToWifi(MapboxTelemetry.applicationContext));

    return mapClickEvent;
  }

  private MapDragendEvent buildMapDragendEvent(MapState mapState) {
    MapDragendEvent mapDragendEvent = new MapDragendEvent(mapState).setDeviceInfo(MapboxTelemetry.applicationContext);
    mapDragendEvent.setOrientation(obtainOrientation(MapboxTelemetry.applicationContext));
    mapDragendEvent.setCarrier(obtainCellularCarrier(MapboxTelemetry.applicationContext));
    mapDragendEvent.setWifi(obtainConnectedToWifi(MapboxTelemetry.applicationContext));

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

  private Boolean obtainConnectedToWifi(Context context) {
    return isConnectedToWifi(context);
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
    if (wifiInfo.getNetworkId() != NO_NETWORK) {
      return true;
    }
    return false;
  }

  private MapLoadEvent buildMapLoadEvent() {
    String userId = TelemetryUtils.retrieveVendorId();
    MapLoadEvent mapLoadEvent = new MapLoadEvent(userId).setDeviceInfo(MapboxTelemetry.applicationContext);
    mapLoadEvent.setOrientation(obtainOrientation(MapboxTelemetry.applicationContext));
    mapLoadEvent.setAccessibilityFontScale(obtainAccessibilityFontScaleSize(MapboxTelemetry.applicationContext));
    mapLoadEvent.setCarrier(obtainCellularCarrier(MapboxTelemetry.applicationContext));
    mapLoadEvent.setResolution(obtainDisplayDensity(MapboxTelemetry.applicationContext));
    mapLoadEvent.setWifi(obtainConnectedToWifi(MapboxTelemetry.applicationContext));
    return mapLoadEvent;
  }

  private void checkLoad(Event.Type type) {
    if (type != Event.Type.MAP_LOAD) {
      throw new IllegalArgumentException(NOT_A_LOAD_MAP_EVENT_TYPE);
    }
  }

  private void checkGesture(Event.Type type, MapState mapState) {
    checkGestureMapEvent(type);
    isNotNull(mapState);
  }

  private void checkGestureMapEvent(Event.Type type) {
    if (!Event.mapGestureEventTypes.contains(type)) {
      throw new IllegalArgumentException(NOT_A_GESTURE_MAP_EVENT_TYPE);
    }
  }

  private void isNotNull(MapState mapState) {
    if (mapState == null) {
      throw new IllegalArgumentException(MAP_STATE_ILLEGAL_NULL);
    }
  }
}
