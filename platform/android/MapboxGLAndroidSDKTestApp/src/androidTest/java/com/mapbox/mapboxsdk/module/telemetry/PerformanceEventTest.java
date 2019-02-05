package com.mapbox.mapboxsdk.module.telemetry;

import android.app.ActivityManager;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.os.Parcel;
import android.support.test.runner.AndroidJUnit4;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.WindowManager;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.mapbox.mapboxsdk.Mapbox;

import org.junit.Test;
import org.junit.runner.RunWith;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.UUID;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

@RunWith(AndroidJUnit4.class)
public class PerformanceEventTest {

  @Test
  public void checksPerformanceEventWithMetaData() throws Exception {
    PerformanceEvent event = obtainPerformanceEvent();
    assertNotNull(event);

    Parcel parcel = Parcel.obtain();

    event.writeToParcel(parcel, 0);
    parcel.setDataPosition(0);

    PerformanceEvent newPerfEvent = PerformanceEvent.CREATOR.createFromParcel(parcel);
    assertNotNull(newPerfEvent);

    compare(event, newPerfEvent, "attributes", "style_id");
    compare(event, newPerfEvent, "counters", "int_value");
    compare(event, newPerfEvent, "counters", "long_value");
    compare(event, newPerfEvent, "counters", "double_value");
    assertEquals(getMetadata(event), getMetadata(newPerfEvent));
  }

  @Test
  public void checksPerformanceEventOnlyRequiredData() throws Exception {
    Bundle bundle = new Bundle();
    bundle.putString("property ignored", "value will be ignored");
    PerformanceEvent event = new PerformanceEvent(UUID.randomUUID().toString(), bundle);
    assertNotNull(event);

    Parcel parcel = Parcel.obtain();
    event.writeToParcel(parcel, 0);
    parcel.setDataPosition(0);

    PerformanceEvent newPerfEvent = PerformanceEvent.CREATOR.createFromParcel(parcel);
    assertNotNull(newPerfEvent);

    assertEquals(getAttributes(event).size(), 0);
    assertEquals(getCounters(event).size(), 0);
    assertEquals(getAttributes(newPerfEvent).size(), 0);
    assertEquals(getCounters(newPerfEvent).size(), 0);
    assertEquals(getMetadata(event), getMetadata(newPerfEvent));
  }


  private PerformanceEvent obtainPerformanceEvent() {
    String styleStr = "mapbox://styles/mapbox/streets-v11";
    boolean testPerfEvent = true;
    Double doubleValue = 40.5;
    Long longValue = 40L;
    Integer intValue = 40;

    List<Attribute<String>> attributes = new ArrayList<>();
    attributes.add(
            new Attribute<>("style_id", styleStr));
    attributes.add(
            new Attribute<>("test_perf_event", String.valueOf(testPerfEvent)));

    List<Attribute<? extends Number>> counters = new ArrayList();
    counters.add(new Attribute<>("long_value", longValue));
    counters.add(new Attribute<>("double_value", doubleValue));
    counters.add(new Attribute<>("int_value", intValue));

    Gson gson = new Gson();

    Bundle bundle = new Bundle();
    bundle.putString("attributes", gson.toJson(attributes));
    bundle.putString("counters", gson.toJson(counters));

    JsonObject metaData = new JsonObject();
    metaData.addProperty("os", "android");
    metaData.addProperty("manufacturer", Build.MANUFACTURER);
    metaData.addProperty("brand", Build.BRAND);
    metaData.addProperty("device", Build.MODEL);
    metaData.addProperty("version", Build.VERSION.RELEASE);
    metaData.addProperty("abi", Build.CPU_ABI);
    metaData.addProperty("country", Locale.getDefault().getISO3Country());
    metaData.addProperty("ram", getRam());
    metaData.addProperty("screenSize", getWindowSize());
    bundle.putString("metadata", metaData.toString());

    return new PerformanceEvent(UUID.randomUUID().toString(), bundle);
  }

  private void compare(PerformanceEvent event1, PerformanceEvent event2, String listFieldName, String name)
          throws NoSuchFieldException, IllegalAccessException {
    Object value1 = getValue(event1, listFieldName, name);
    Object value2  = getValue(event2, listFieldName, name);

    if (value1 instanceof Double && value2 instanceof Double) {
      assertEquals((Double)value1, (Double)value2, 0.00006);
    } else {
      assertEquals(value1, value2);
    }
  }

  private Object getPrivateFieldValue(Object object, String fieldName)
    throws NoSuchFieldException, IllegalAccessException {
    Field field = object.getClass().getDeclaredField(fieldName);
    field.setAccessible(true);
    return field.get(object);
  }

  private Object getValue(PerformanceEvent event, String listFieldName, String name)
    throws NoSuchFieldException, IllegalAccessException {
    ArrayList list = (ArrayList)getPrivateFieldValue(event, listFieldName);
    for (Object element : list) {
      Object elementName = getPrivateFieldValue(element, "name");
      if (elementName != null && elementName.equals((String)name)) {
        return getPrivateFieldValue(element, "value");
      }
    }
    return null;
  }

  private JsonObject getMetadata(PerformanceEvent event)
          throws NoSuchFieldException, IllegalAccessException {
    return (JsonObject)getPrivateFieldValue(event, "metadata");
  }

  private ArrayList getAttributes(PerformanceEvent event)
          throws NoSuchFieldException, IllegalAccessException {
    return (ArrayList)getPrivateFieldValue(event, "attributes");
  }

  private ArrayList getCounters(PerformanceEvent event)
          throws NoSuchFieldException, IllegalAccessException {
    return (ArrayList)getPrivateFieldValue(event, "counters");
  }

  private static String getRam() {
    ActivityManager actManager =
            (ActivityManager) Mapbox.getApplicationContext().getSystemService(Context.ACTIVITY_SERVICE);
    ActivityManager.MemoryInfo memInfo = new ActivityManager.MemoryInfo();
    actManager.getMemoryInfo(memInfo);
    return String.valueOf(memInfo.totalMem);
  }

  private static String getWindowSize() {
    WindowManager windowManager =
            (WindowManager) Mapbox.getApplicationContext().getSystemService(Context.WINDOW_SERVICE);
    Display display = windowManager.getDefaultDisplay();
    DisplayMetrics metrics = new DisplayMetrics();
    display.getMetrics(metrics);
    int width = metrics.widthPixels;
    int height = metrics.heightPixels;

    return "{" + width + "," + height + "}";
  }

  private class Attribute<T> {
    private String name;
    private T value;

    Attribute(String name, T value) {
      this.name = name;
      this.value = value;
    }
  }
}
