package com.mapbox.mapboxsdk.exceptions;

import android.content.Context;
import android.os.Bundle;

import com.mapbox.mapboxsdk.maps.MapView;

/**
 * A {@code DuplicateInitializationException} is thrown by {@link com.mapbox.mapboxsdk.Mapbox}
 * when the global Mapbox singleton has been initialized already
 *
 * @see MapView#onCreate(Bundle)
 * @see com.mapbox.mapboxsdk.Mapbox#getInstance(Context, String)
 */
public class DuplicateInitializationException extends RuntimeException {

  public DuplicateInitializationException() {
    super("Mapbox singleton can only be initialized once");
  }
}
