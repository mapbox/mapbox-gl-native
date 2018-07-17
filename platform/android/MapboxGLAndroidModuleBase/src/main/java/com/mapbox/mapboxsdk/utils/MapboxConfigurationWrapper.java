package com.mapbox.mapboxsdk.utils;

import android.content.Context;
import android.content.ContextWrapper;

/**
 * Application context wrapper, provides context to modular components.
 * <p>
 * Initialised as part of Mapbox.java.
 * </p>
 */
public class MapboxConfigurationWrapper extends ContextWrapper {

  private static MapboxConfigurationWrapper instance;
  private static String accessToken;

  public static synchronized MapboxConfigurationWrapper initialize(Context context, String accessToken) {
    if (instance == null) {
      instance = new MapboxConfigurationWrapper(context.getApplicationContext(), accessToken);
    }
    return instance;
  }

  private MapboxConfigurationWrapper(Context base, String token) {
    super(base.getApplicationContext());
    accessToken = token;
  }

  public static Context getContext() {
    Context context = instance.getBaseContext();
    if (context == null) {
      throw new IllegalStateException("MapboxConfigurationWrapper isn't initialised correctly.");
    }
    return instance.getBaseContext();
  }

  public static String getAccessToken() {
    if (accessToken == null) {
      throw new IllegalStateException("MapboxConfigurationWrapper isn't initialised correctly.");
    }
    return accessToken;
  }
}
