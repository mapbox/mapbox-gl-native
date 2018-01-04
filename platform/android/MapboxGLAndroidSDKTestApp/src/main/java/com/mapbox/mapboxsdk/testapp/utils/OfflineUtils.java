package com.mapbox.mapboxsdk.testapp.utils;

import android.support.annotation.NonNull;

import com.google.gson.Gson;
import com.google.gson.JsonObject;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.testapp.activity.offline.OfflineActivity.JSON_CHARSET;
import static com.mapbox.mapboxsdk.testapp.activity.offline.OfflineActivity.JSON_FIELD_REGION_NAME;

public class OfflineUtils {

  public static String convertRegionName(@NonNull byte[] metadata) {
    try {
      String json = new String(metadata, JSON_CHARSET);
      JsonObject jsonObject = new Gson().fromJson(json, JsonObject.class);
      return jsonObject.get(JSON_FIELD_REGION_NAME).getAsString();
    } catch (Exception exception) {
      return null;
    }
  }

  public static byte[] convertRegionName(String regionName) {
    try {
      JsonObject jsonObject = new JsonObject();
      jsonObject.addProperty(JSON_FIELD_REGION_NAME, regionName);
      return jsonObject.toString().getBytes(JSON_CHARSET);
    } catch (Exception exception) {
      Timber.e(exception, "Failed to encode metadata: ");
    }
    return null;
  }

}
