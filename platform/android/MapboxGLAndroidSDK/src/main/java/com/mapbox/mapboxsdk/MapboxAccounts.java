package com.mapbox.mapboxsdk;

import android.content.Context;
import android.content.SharedPreferences;
import android.support.annotation.NonNull;

import com.mapbox.core.utils.TextUtils;
import com.mapbox.mapboxsdk.constants.MapboxConstants;

import java.util.Calendar;

class MapboxAccounts {

  // TODO: Move to Constants? Repeated in FileSource.
  private static final String MAPBOX_SHARED_PREFERENCES = "MapboxSharedPreferences";

  private final static String PREFERENCE_TIMESTAMP = "com.mapbox.mapboxsdk.accounts.timestamp";
  private final static String PREFERENCE_SKU_TOKEN = "com.mapbox.mapboxsdk.accounts.skutoken";

  private final static long ONE_HOUR_MILLIS = 60 * 60 * 1_000L;

  private long timestamp;
  private String skuToken;

  MapboxAccounts() {
    SharedPreferences sharedPreferences = getSharedPreferences();
    timestamp = sharedPreferences.getLong(PREFERENCE_TIMESTAMP, 0L);
    skuToken = sharedPreferences.getString(PREFERENCE_SKU_TOKEN, "");
    if (timestamp == 0L || TextUtils.isEmpty(skuToken)) {
      skuToken = generateSkuToken();
      timestamp = persistToken(skuToken);
    }
  }

  String getSkuToken() {
    if (isExpired()) {
      skuToken = generateSkuToken();
      timestamp = persistToken(skuToken);
    }

    return skuToken;
  }

  private boolean isExpired() {
    return (getNow() - timestamp > ONE_HOUR_MILLIS);
  }

  private long persistToken(String skuToken) {
    long now = getNow();
    SharedPreferences.Editor editor = getSharedPreferences().edit();
    editor.putLong(PREFERENCE_TIMESTAMP, now);
    editor.putString(PREFERENCE_SKU_TOKEN, skuToken);
    editor.apply();
    return now;
  }

  private @NonNull SharedPreferences getSharedPreferences() {
    return Mapbox.getApplicationContext().getSharedPreferences(MAPBOX_SHARED_PREFERENCES, Context.MODE_PRIVATE);
  }

  private long getNow() {
    return Calendar.getInstance(MapboxConstants.MAPBOX_LOCALE).getTimeInMillis();
  }

  private @NonNull String generateSkuToken() {
    return "placeholder"; // TODO
  }
}
