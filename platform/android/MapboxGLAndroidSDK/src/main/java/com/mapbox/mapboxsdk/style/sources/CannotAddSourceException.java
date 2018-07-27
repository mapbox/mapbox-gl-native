package com.mapbox.mapboxsdk.style.sources;

import android.support.annotation.Keep;

/**
 * Thrown when adding a source to a map twice
 */
@Keep
public class CannotAddSourceException extends RuntimeException {

  public CannotAddSourceException(String message) {
    super(message);
  }

}
