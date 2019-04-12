package com.mapbox.mapboxsdk;

import android.content.Context;
import android.content.SharedPreferences;
import android.support.annotation.NonNull;
import android.text.format.DateUtils;

import com.mapbox.core.utils.TextUtils;
import com.mapbox.mapboxsdk.constants.MapboxConstants;

/**
 * This class wraps the functionality provided by MapboxAccounts and manages its state
 * using Android's SharedPreferences for storage. This class is meant for internal SDK
 * usage only.
 */
class AccountsManager {
  private static final String PREFERENCE_USER_ID = "com.mapbox.mapboxsdk.accounts.userid";
  private static final String PREFERENCE_TIMESTAMP = "com.mapbox.mapboxsdk.accounts.timestamp";
  private static final String PREFERENCE_SKU_TOKEN = "com.mapbox.mapboxsdk.accounts.skutoken";

  private long timestamp;
  private String skuToken;

  AccountsManager() {
    String userId = validateUserId();
    validateRotation(userId);
  }

  private String validateUserId() {
    SharedPreferences sharedPreferences = getSharedPreferences();
    String userId = sharedPreferences.getString(PREFERENCE_USER_ID, "");
    if (TextUtils.isEmpty(userId)) {
      userId = generateUserId();
      SharedPreferences.Editor editor = getSharedPreferences().edit();
      editor.putString(PREFERENCE_USER_ID, userId);
      editor.apply();
    }

    return userId;
  }

  private void validateRotation(String userId) {
    SharedPreferences sharedPreferences = getSharedPreferences();
    timestamp = sharedPreferences.getLong(PREFERENCE_TIMESTAMP, 0L);
    skuToken = sharedPreferences.getString(PREFERENCE_SKU_TOKEN, "");
    if (timestamp == 0L || TextUtils.isEmpty(skuToken)) {
      skuToken = generateSkuToken(userId);
      timestamp = persistRotation(skuToken);
    }
  }

  String getSkuToken() {
    if (isExpired()) {
      SharedPreferences sharedPreferences = getSharedPreferences();
      String userId = sharedPreferences.getString(PREFERENCE_USER_ID, "");
      skuToken = generateSkuToken(userId);
      timestamp = persistRotation(skuToken);
    }

    return skuToken;
  }

  private boolean isExpired() {
    return isExpired(getNow(), timestamp);
  }

  static boolean isExpired(long now, long then) {
    return ((now - then) > DateUtils.HOUR_IN_MILLIS);
  }

  private long persistRotation(String skuToken) {
    long now = getNow();
    SharedPreferences.Editor editor = getSharedPreferences().edit();
    editor.putLong(PREFERENCE_TIMESTAMP, now);
    editor.putString(PREFERENCE_SKU_TOKEN, skuToken);
    editor.apply();
    return now;
  }

  @NonNull
  private SharedPreferences getSharedPreferences() {
    return Mapbox.getApplicationContext()
        .getSharedPreferences(MapboxConstants.MAPBOX_SHARED_PREFERENCES, Context.MODE_PRIVATE);
  }

  static long getNow() {
    return System.currentTimeMillis();
  }

  @NonNull
  private String generateUserId() {
    return "placeholder"; // TODO
  }

  @NonNull
  private String generateSkuToken(String userId) {
    return "placeholder"; // TODO
  }
}
