package com.mapbox.mapboxsdk.module.telemetry;

import android.os.Build;
import android.os.Bundle;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.mapbox.mapboxsdk.BuildConfig;
import com.mapbox.mapboxsdk.offline.OfflineRegion;
import org.junit.Before;
import org.junit.Test;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import static com.mapbox.mapboxsdk.module.telemetry.PerformanceEvent.PerformanceAttribute;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class MapEventFactoryTest {
  private static final float FONT_SCALE = 1.f;
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
  private static final long SIZE_OF_RESOURCES_COMPLETED = 100;
  private static final long NUMBER_OF_TILE_COMPLETED = 1000;
  private static final int STATE = OfflineRegion.STATE_ACTIVE;
  private static final String SESSION_ID = "001";

  private PhoneState phoneState;
  private Gson gson = new Gson();

  @Before
  public void setUp() {
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
  public void testOfflineDownloadEndEvent() {
    OfflineDownloadEndEvent offlineDownloadEndEvent = MapEventFactory.buildOfflineDownloadCompleteEvent(phoneState,
      SHAPE, MIN_ZOOM, MAX_ZOOM, STYLE_URL, SIZE_OF_RESOURCES_COMPLETED, NUMBER_OF_TILE_COMPLETED, STATE);
    assertEquals(SHAPE, offlineDownloadEndEvent.getShapeForOfflineRegion());
    assertEquals(MIN_ZOOM, offlineDownloadEndEvent.getMinZoom());
    assertEquals(MAX_ZOOM, offlineDownloadEndEvent.getMaxZoom());
    assertEquals(STYLE_URL, offlineDownloadEndEvent.getStyleURL());
    assertEquals(SIZE_OF_RESOURCES_COMPLETED, offlineDownloadEndEvent.getSizeOfResourcesCompleted());
    assertEquals(NUMBER_OF_TILE_COMPLETED, offlineDownloadEndEvent.getNumberOfTilesCompleted());
    assertEquals(String.valueOf(STATE), offlineDownloadEndEvent.getState());
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

  @Test
  public void testPerformanceEvent() {
    String nameAttribute = "nameAttribute";
    String valueAttribute = "100";
    String nameCounters = "nameCounters";
    Double valuesCounters = 1000.0;
    JsonObject metaData = new JsonObject();
    metaData.addProperty("os", "android");
    metaData.addProperty("manufacturer", Build.MANUFACTURER);
    metaData.addProperty("brand", Build.BRAND);
    metaData.addProperty("device", Build.MODEL);
    metaData.addProperty("version", Build.VERSION.RELEASE);
    metaData.addProperty("abi", Build.CPU_ABI);
    metaData.addProperty("country", Locale.getDefault().getISO3Country());
    metaData.addProperty("ram", "ram");
    metaData.addProperty("screenSize", "1000");

    Bundle data = mock(Bundle.class);
    ArrayList<PerformanceEvent.PerformanceAttribute<String>> attribtueList = new ArrayList<>();
    PerformanceAttribute<String> attribute = new PerformanceAttribute<>(nameAttribute, valueAttribute);
    attribtueList.add(attribute);
    ArrayList<PerformanceEvent.PerformanceAttribute<Double>> countersList = new ArrayList<>();
    PerformanceAttribute<Double> counter = new PerformanceAttribute<>(nameCounters, valuesCounters);
    countersList.add(counter);
    when(data.getString("attributes")).thenReturn(gson.toJson(attribtueList));
    when(data.getString("counters")).thenReturn(gson.toJson(countersList));
    when(data.getString("metadata")).thenReturn(metaData.toString());
    PerformanceEvent performanceEvent = MapEventFactory.buildPerformanceEvent(phoneState,
      SESSION_ID, data);
    assertEquals(CREATED, performanceEvent.getCreated());
    assertEquals(metaData.toString(), performanceEvent.getMetadata().toString());
    assertEquals(SESSION_ID, performanceEvent.getSessionId());
    List<PerformanceEvent.PerformanceAttribute<String>> attributeList = performanceEvent.getAttributes();
    assertEquals(1, attribtueList.size());
    assertEquals(attribute, attribtueList.get(0));
    List<PerformanceEvent.PerformanceAttribute<Double>> counterList = performanceEvent.getCounters();
    assertEquals(1, counterList.size());
    assertEquals(counter, counterList.get(0));
  }
}
