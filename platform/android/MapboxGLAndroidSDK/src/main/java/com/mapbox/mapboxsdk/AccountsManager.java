package com.mapbox.mapboxsdk;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.text.TextUtils;
import android.text.format.DateUtils;

import com.mapbox.android.accounts.v1.MapboxAccounts;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.log.Logger;

/**
 * IF YOU USE THIS CODE WITH MAPBOX MAPPING API, REMOVAL OR MODIFICATION OF
 * THE FOLLOWING CODE VIOLATES THE MAPBOX TERMS OF SERVICE.
 *
 * The following code is used to access Mapbox's Mapping APIs. Removal or
 * modification of this code when used with Mapbox's Mapping APIs can result
 * in higher fees and/or termination of your account with Mapbox.
 *
 * Under the Mapbox Terms of Service, you may not use this code to access
 * Mapbox Mapping APIs other than through Mapbox SDKs.
 *
 * The Android documentation to access Mapping APIs is available at
 * https://www.mapbox.com/android and the Mapbox Terms of Service are
 * available at https://www.mapbox.com/tos/.
 */
class AccountsManager {
  private static final String TAG = "Mbgl-AccountsManager";

  private static final String PREFERENCE_USER_ID = "com.mapbox.mapboxsdk.accounts.userid";
  private static final String PREFERENCE_TIMESTAMP = "com.mapbox.mapboxsdk.accounts.timestamp";
  private static final String PREFERENCE_SKU_TOKEN = "com.mapbox.mapboxsdk.accounts.skutoken";

  private long timestamp;
  @Nullable
  private String skuToken;

  private boolean isEnabled;

  AccountsManager() {
    isEnabled = isSkuTokenEnabled();
    if (isEnabled) {
      String userId = validateUserId();
      validateRotation(userId);
    } else {
      timestamp = 0L;
      skuToken = null;
    }
  }

  private boolean isSkuTokenEnabled() {
    boolean value = MapboxConstants.DEFAULT_ENABLE_SKU_TOKEN;
    try {
      // Try getting a custom value from the app Manifest
      ApplicationInfo appInfo = Mapbox.getApplicationContext().getPackageManager().getApplicationInfo(
          Mapbox.getApplicationContext().getPackageName(),
          PackageManager.GET_META_DATA);
      if (appInfo.metaData != null) {
        value = appInfo.metaData.getBoolean(
            MapboxConstants.KEY_META_DATA_ENABLE_SKU_TOKEN,
            MapboxConstants.DEFAULT_ENABLE_SKU_TOKEN
        );
      }
    } catch (Exception exception) {
      Logger.e(TAG, "Failed to read the package metadata: ", exception);
    }

    return value;
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

  @Nullable
  String getSkuToken() {
    if (isEnabled && isExpired()) {
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
    return MapboxAccounts.obtainEndUserId();
  }

  @NonNull
  private String generateSkuToken(String userId) {
    return MapboxAccounts.obtainMapsSkuUserToken(userId);
  }
}
