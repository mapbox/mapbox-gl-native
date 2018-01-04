package com.mapbox.mapboxsdk.testapp.utils;

import android.content.Context;
import android.graphics.Typeface;

import java.util.Hashtable;

import timber.log.Timber;

public class FontCache {

  private static Hashtable<String, Typeface> fontCache = new Hashtable<>();

  public static Typeface get(String name, Context context) {
    Typeface tf = fontCache.get(name);
    if (tf == null) {
      try {
        tf = Typeface.createFromAsset(context.getAssets(), name);
        fontCache.put(name, tf);
      } catch (Exception exception) {
        Timber.e("Font not found");
      }
    }
    return tf;
  }
}
