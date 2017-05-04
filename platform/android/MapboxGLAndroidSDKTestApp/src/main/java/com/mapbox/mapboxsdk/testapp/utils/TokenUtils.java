package com.mapbox.mapboxsdk.testapp.utils;


import android.content.Context;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.Mapbox;

public class TokenUtils {

  /**
   * <p>
   * Returns the Mapbox access token set in the app resources.
   * </p>
   * It will first search for a token in the Mapbox object. If not found it
   * will then attempt to load the access token from the
   * {@code res/values/dev.xml} development file.
   *
   * @param context The {@link Context} of the {@link android.app.Activity} or {@link android.app.Fragment}.
   * @return The Mapbox access token or null if not found.
   */
  public static String getMapboxAccessToken(@NonNull Context context) {
    try {
      // Read out AndroidManifest
      String token = Mapbox.getAccessToken();
      if (token == null || token.isEmpty()) {
        throw new IllegalArgumentException();
      }
      return token;
    } catch (Exception exception) {
      // Use fallback on string resource, used for development
      int tokenResId = context.getResources()
        .getIdentifier("mapbox_access_token", "string", context.getPackageName());
      return tokenResId != 0 ? context.getString(tokenResId) : null;
    }
  }
}
