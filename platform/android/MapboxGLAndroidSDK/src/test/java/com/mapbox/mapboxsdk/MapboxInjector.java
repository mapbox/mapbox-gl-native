package com.mapbox.mapboxsdk;

import android.content.Context;

import java.lang.reflect.Field;

public class MapboxInjector {

  public static void inject(Context context, String accessToken) {
    Mapbox mapbox = new Mapbox(context, accessToken);
    try {
      Field field = Mapbox.class.getDeclaredField("INSTANCE");
      field.setAccessible(true);
      field.set(mapbox, mapbox);
    } catch (Exception exception) {
      throw new AssertionError();
    }
  }

  public static void clear() {
    try {
      Field field = Mapbox.class.getDeclaredField("INSTANCE");
      field.setAccessible(true);
      field.set(field, null);
    } catch (Exception exception) {
      throw new AssertionError();
    }
  }
}
