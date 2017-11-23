package com.mapbox.mapboxsdk.testapp.utils;

import android.support.annotation.NonNull;

import org.json.JSONObject;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.testapp.activity.offline.OfflineActivity.JSON_CHARSET;
import static com.mapbox.mapboxsdk.testapp.activity.offline.OfflineActivity.JSON_FIELD_REGION_NAME;

public class OfflineUtils {

  public static String convertRegionName(@NonNull byte[] metadata) {
    try {
      String json = new String(metadata, JSON_CHARSET);
      JSONObject jsonObject = new JSONObject(json);
      return jsonObject.getString(JSON_FIELD_REGION_NAME);
    } catch (Exception exception) {
      return null;
    }
  }

  public static byte[] convertRegionName(String regionName) {
    byte[] metadata = null;
    try {
      JSONObject jsonObject = new JSONObject();
      jsonObject.put(JSON_FIELD_REGION_NAME, regionName);
      String json = jsonObject.toString();
      metadata = json.getBytes(JSON_CHARSET);
    } catch (Exception exception) {
      Timber.e(exception, "Failed to encode metadata: ");
    }
    return metadata;
  }

}
