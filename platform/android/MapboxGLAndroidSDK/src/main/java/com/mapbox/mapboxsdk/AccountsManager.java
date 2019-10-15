package com.mapbox.mapboxsdk;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import androidx.annotation.NonNull;
import androidx.annotation.VisibleForTesting;
import android.text.TextUtils;
import android.text.format.DateUtils;

import com.mapbox.android.accounts.v1.MapboxAccounts;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.log.Logger;

import static com.mapbox.mapboxsdk.constants.MapboxConstants.KEY_PREFERENCE_SKU_TOKEN;

/**
 * REMOVAL OR MODIFICATION OF THE FOLLOWING CODE VIOLATES THE MAPBOX TERMS
 * OF SERVICE
 * <p>
 * The following code is used to access Mapbox's Mapping APIs.
 * <p>
 * Removal or modification of this code when used with Mapbox's Mapping APIs
 * can result in termination of your agreement and/or your account with
 * Mapbox.
 * <p>
 * Using this code to access Mapbox Mapping APIs from outside the Mapbox Maps
 * SDK also violates the Mapbox Terms of Service. On Android, Mapping APIs
 * should be accessed using the methods documented at
 * https://www.mapbox.com/android.
 * <p>
 * You can access the Mapbox Terms of Service at https://www.mapbox.com/tos/
 */
class AccountsManager {
  private static final String TAG = "Mbgl-AccountsManager";
  private static final String PREFERENCE_USER_ID = "com.mapbox.mapboxsdk.accounts.userid";
  private static final String PREFERENCE_TIMESTAMP = "com.mapbox.mapboxsdk.accounts.timestamp";

  private SharedPreferences sharedPreferences;
  private String skuToken;
  private long timestamp;
  private boolean isManaged;

  AccountsManager() {
    isManaged = isSkuTokenManaged();
    initialize();
  }

  @VisibleForTesting
  AccountsManager(SharedPreferences sharedPreferences, boolean isManaged) {
    this.sharedPreferences = sharedPreferences;
    this.isManaged = isManaged;
    initialize();
  }

  private void initialize() {
    retrieveSkuTokenAndTimestamp();
    if (isManaged) {
      String userId = validateUserId();
      validateRotation(userId);
    }
  }

  private boolean isSkuTokenManaged() {
    boolean value = MapboxConstants.DEFAULT_MANAGE_SKU_TOKEN;
    try {
      // Try getting a custom value from the app Manifest
      ApplicationInfo appInfo = retrieveApplicationInfo();
      if (appInfo.metaData != null) {
        value = appInfo.metaData.getBoolean(
          MapboxConstants.KEY_META_DATA_MANAGE_SKU_TOKEN,
          MapboxConstants.DEFAULT_MANAGE_SKU_TOKEN
        );
      }
    } catch (Exception exception) {
      Logger.e(TAG, "Failed to read the package metadata: ", exception);
    }

    return value;
  }

  private ApplicationInfo retrieveApplicationInfo() throws PackageManager.NameNotFoundException {
    return Mapbox.getApplicationContext().getPackageManager().getApplicationInfo(
      Mapbox.getApplicationContext().getPackageName(),
      PackageManager.GET_META_DATA);
  }

  private void retrieveSkuTokenAndTimestamp() {
    SharedPreferences sharedPreferences = getSharedPreferences();
    skuToken = sharedPreferences.getString(KEY_PREFERENCE_SKU_TOKEN, "");
    timestamp = sharedPreferences.getLong(PREFERENCE_TIMESTAMP, 0L);
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
    if (TextUtils.isEmpty(skuToken) || timestamp == 0L) {
      skuToken = generateSkuToken(userId);
      timestamp = persistRotation(skuToken);
    }
  }

  String getSkuToken() {
    if (isManaged) {
      if (isExpired()) {
        SharedPreferences sharedPreferences = getSharedPreferences();
        String userId = sharedPreferences.getString(PREFERENCE_USER_ID, "");
        skuToken = generateSkuToken(userId);
        timestamp = persistRotation(skuToken);
      }
    } else {
      SharedPreferences sharedPreferences = getSharedPreferences();
      String notManagedSkuToken = sharedPreferences.getString(KEY_PREFERENCE_SKU_TOKEN, "");
      skuToken = notManagedSkuToken;
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
    editor.putString(KEY_PREFERENCE_SKU_TOKEN, skuToken);
    editor.apply();
    return now;
  }

  @NonNull
  private SharedPreferences getSharedPreferences() {
    if (sharedPreferences == null) {
      sharedPreferences = Mapbox.getApplicationContext()
        .getSharedPreferences(MapboxConstants.MAPBOX_SHARED_PREFERENCES, Context.MODE_PRIVATE);
    }
    return sharedPreferences;
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
