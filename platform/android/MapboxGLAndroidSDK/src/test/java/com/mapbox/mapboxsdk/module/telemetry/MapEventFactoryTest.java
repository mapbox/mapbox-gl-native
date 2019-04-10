package com.mapbox.mapboxsdk.module.telemetry;

import android.os.Build;

import com.google.gson.Gson;
import com.mapbox.mapboxsdk.BuildConfig;
import com.mapbox.mapboxsdk.constants.TelemetryConstants;
import com.mapbox.mapboxsdk.offline.OfflineRegion;

import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

public class MapEventFactoryTest {
  private static final float FONT_SCALE = 1.f;
  private static final double LATITUDE = 10.0;
  private static final double LONGITUDE = 11.0;
  private static final double ZOOM = 10.0;
  private static final int BATTERY_LEVEL = 50;
  private static final String CARRIER = "carrier";
  private static final String NETWORK_TYPE = "network";
  private static final String CREATED = "2019-04-09";
  private static final boolean PLUGIN = true;
  private static final float RESOLUTION = 1024.f;
  private static final boolean WIFI = true;
  private static final String SHAPE = "shape";
  private static final Double MIN_ZOOM = 1.0;
  private static final Double MAX_ZOOM = 10.0;
  private static final String STYLE_URL = "style url";
  private static final long SIZE_OF_RESOURCES_COMPLETED = 100L;
  private static final long NUMBER_OF_TILE_COMPLETED = 1000L;
  private static final int STATE = OfflineRegion.STATE_ACTIVE;

  private MapState mapState;
  private PhoneState phoneState;
  private Gson gson = new Gson();

  @Before
  public void setUp() {
    mapState = new MapState(LATITUDE, LONGITUDE, ZOOM);
    phoneState = new PhoneState();
    phoneState.setAccessibilityFontScale(FONT_SCALE);
    phoneState.setBatteryLevel(BATTERY_LEVEL);
    phoneState.setCarrier(CARRIER);
    phoneState.setCellularNetworkType(NETWORK_TYPE);
    phoneState.setCreated(CREATED);
    phoneState.setOrientation(PhoneState.Orientation.ORIENTATION_LANDSCAPE);
    phoneState.setPluggedIn(PLUGIN);
    phoneState.setResolution(RESOLUTION);
    phoneState.setWifi(WIFI);
  }

  @Test
  public void testClickEvent() {
    mapState.setGesture(TelemetryConstants.DOUBLE_TAP);
    MapClickEvent mapClickEvent = MapEventFactory.buildMapClickEvent(phoneState, mapState);
    assertEquals(LATITUDE, mapClickEvent.getLatitude(), 0);
    assertEquals(LONGITUDE, mapClickEvent.getLongitude(), 0);
    assertEquals(ZOOM, mapClickEvent.getZoom(), 0);
    assertEquals(BATTERY_LEVEL, mapClickEvent.getBatteryLevel());
    assertEquals(CARRIER, mapClickEvent.getCarrier());
    assertEquals(NETWORK_TYPE, mapClickEvent.getCellularNetworkType());
    assertEquals(CREATED, mapClickEvent.getCreated());
    assertEquals(TelemetryConstants.DOUBLE_TAP, mapClickEvent.getGesture());
    assertEquals("Landscape", mapClickEvent.getOrientation());
    assertEquals(PLUGIN, mapClickEvent.isPluggedIn());
    assertEquals(WIFI, mapClickEvent.isWifi());
    String json = gson.toJson(mapClickEvent);
    MapClickEvent event = gson.fromJson(json, MapClickEvent.class);
    assertEquals(mapClickEvent, event);
  }

  @Test
  public void testMapLoadEvent() {
    MapLoadEvent mapLoadEvent = MapEventFactory.buildMapLoadEvent(phoneState);
    assertEquals("Android - " + Build.VERSION.RELEASE, mapLoadEvent.getOperatingSystem());
    assertEquals(Build.MODEL, mapLoadEvent.getModel());
    assertNotNull(mapLoadEvent.getUserId());
    assertEquals(BATTERY_LEVEL, mapLoadEvent.getBatteryLevel());
    assertEquals(CARRIER, mapLoadEvent.getCarrier());
    assertEquals(NETWORK_TYPE, mapLoadEvent.getCellularNetworkType());
    assertEquals(CREATED, mapLoadEvent.getCreated());
    assertEquals("Landscape", mapLoadEvent.getOrientation());
    assertEquals(BuildConfig.MAPBOX_SDK_IDENTIFIER, mapLoadEvent.getSdkIdentifier());
    assertEquals(BuildConfig.MAPBOX_SDK_VERSION, mapLoadEvent.getSdkVersion());
    assertEquals(PLUGIN, mapLoadEvent.isPluggedIn());
    assertEquals(WIFI, mapLoadEvent.isWifi());
    assertEquals(FONT_SCALE, mapLoadEvent.getAccessibilityFontScale(), 0);
    String json = gson.toJson(mapLoadEvent);
    MapLoadEvent event = gson.fromJson(json, MapLoadEvent.class);
    assertEquals(mapLoadEvent, event);
  }

  @Test
  public void testMapDraggedEvent() {
    MapDragendEvent mapDraggedEvent = MapEventFactory.buildMapDragendEvent(phoneState, mapState);
    assertEquals(LATITUDE, mapDraggedEvent.getLat(), 0);
    assertEquals(LONGITUDE, mapDraggedEvent.getLng(), 0);
    assertEquals(ZOOM, mapDraggedEvent.getZoom(), 0);
    assertEquals(BATTERY_LEVEL, mapDraggedEvent.getBatteryLevel());
    assertEquals(CARRIER, mapDraggedEvent.getCarrier());
    assertEquals(NETWORK_TYPE, mapDraggedEvent.getCellularNetworkType());
    assertEquals(CREATED, mapDraggedEvent.getCreated());
    assertEquals("Landscape", mapDraggedEvent.getOrientation());
    assertEquals(PLUGIN, mapDraggedEvent.isPluggedIn());
    assertEquals(WIFI, mapDraggedEvent.isWifi());
    String json = gson.toJson(mapDraggedEvent);
    MapDragendEvent event = gson.fromJson(json, MapDragendEvent.class);
    assertEquals(mapDraggedEvent, event);
  }

  @Test
  public void testOfflineDownloadEndEvent() {
    OfflineDownloadEndEvent offlineDownloadEndEvent = MapEventFactory.buildOfflineDownloadCompleteEvent(phoneState,
      SHAPE, MIN_ZOOM, MAX_ZOOM, STYLE_URL, SIZE_OF_RESOURCES_COMPLETED, NUMBER_OF_TILE_COMPLETED, STATE);
    assertEquals(SHAPE, offlineDownloadEndEvent.getShapeForOfflineRegion());
    assertEquals(MIN_ZOOM, offlineDownloadEndEvent.getMinZoom());
    assertEquals(MAX_ZOOM, offlineDownloadEndEvent.getMaxZoom());
    assertEquals(STYLE_URL, offlineDownloadEndEvent.getStyleURL());
    assertEquals(SIZE_OF_RESOURCES_COMPLETED, offlineDownloadEndEvent.getSizeOfResourcesCompleted());
    assertEquals(NUMBER_OF_TILE_COMPLETED, offlineDownloadEndEvent.getNumberOfTilesCompleted());
    assertEquals(STATE, offlineDownloadEndEvent.getState());
    assertEquals(CREATED, offlineDownloadEndEvent.getCreated());
    String json = gson.toJson(offlineDownloadEndEvent);
    OfflineDownloadEndEvent event = gson.fromJson(json, OfflineDownloadEndEvent.class);
    assertEquals(offlineDownloadEndEvent, event);
  }

  @Test
  public void testOfflineDownloadStartEvent() {
    OfflineDownloadStartEvent offlineDownloadStartEvent = MapEventFactory.buildOfflineDownloadStartEvent(phoneState,
      SHAPE, MIN_ZOOM, MAX_ZOOM, STYLE_URL);
    assertEquals(SHAPE, offlineDownloadStartEvent.getShapeForOfflineRegion());
    assertEquals(MIN_ZOOM, offlineDownloadStartEvent.getMinZoom());
    assertEquals(MAX_ZOOM, offlineDownloadStartEvent.getMaxZoom());
    assertEquals(STYLE_URL, offlineDownloadStartEvent.getStyleURL());
    assertEquals(CREATED, offlineDownloadStartEvent.getCreated());
    String json = gson.toJson(offlineDownloadStartEvent);
    OfflineDownloadStartEvent event = gson.fromJson(json, OfflineDownloadStartEvent.class);
    assertEquals(offlineDownloadStartEvent, event);
  }
}
