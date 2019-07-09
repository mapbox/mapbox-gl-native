package com.mapbox.mapboxsdk.exceptions;

import android.content.Context;
import android.support.annotation.NonNull;

/**
 * A MapboxConfigurationException is thrown by MapboxMap when the SDK hasn't been properly initialised.
 * <p>
 * This occurs either when {@link com.mapbox.mapboxsdk.Mapbox} is not correctly initialised or the provided access token
 * through {@link com.mapbox.mapboxsdk.Mapbox#getInstance(Context, String)} isn't valid.
 * </p>
 *
 * @see com.mapbox.mapboxsdk.Mapbox#getInstance(Context, String)
 */
public class MapboxConfigurationException extends RuntimeException {

  /**
   * Creates a Mapbox configuration exception thrown by MapboxMap when the SDK hasn't been properly initialised.
   */
  public MapboxConfigurationException() {
    super("\nUsing MapView requires calling Mapbox.getInstance(Context context, String accessToken) before "
      + "inflating or creating the view. The access token parameter is required when using a Mapbox service."
      + "\nPlease see https://www.mapbox.com/help/create-api-access-token/ to learn how to create one."
      + "\nMore information in this guide https://www.mapbox.com/help/first-steps-android-sdk/#access-tokens.");
  }

  /**
   * Creates a Mapbox configuration exception thrown by MapboxMap when the SDK hasn't been properly initialised.
   */
  public MapboxConfigurationException(@NonNull String message) {
    super(message);
  }
}
