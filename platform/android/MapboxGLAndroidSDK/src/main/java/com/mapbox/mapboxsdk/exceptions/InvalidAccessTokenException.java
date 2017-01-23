package com.mapbox.mapboxsdk.exceptions;

import android.content.Context;
import android.os.Bundle;

import com.mapbox.mapboxsdk.maps.MapView;

/**
 * A {@code InvalidAccessTokenException} is thrown by {@link com.mapbox.mapboxsdk.maps.MapboxMap}
 * when there is either no access token set before {@link MapView#onCreate(Bundle)} or an invalid access token
 * is set in {@link com.mapbox.mapboxsdk.Mapbox#getInstance(Context, String)}
 *
 * @see MapView#onCreate(Bundle)
 * @see com.mapbox.mapboxsdk.Mapbox#getInstance(Context, String)
 */
public class InvalidAccessTokenException extends RuntimeException {

  public InvalidAccessTokenException() {
    super("\nUsing MapView requires setting a valid access token. Use Mapbox.getInstance(Context context, "
      + "String accessToken) to provide one. "
      + "\nPlease see https://www.mapbox.com/help/create-api-access-token/ to learn how to create one."
      + "\nMore information in this guide https://www.mapbox.com/help/first-steps-android-sdk/#access-tokens.");
  }
}
