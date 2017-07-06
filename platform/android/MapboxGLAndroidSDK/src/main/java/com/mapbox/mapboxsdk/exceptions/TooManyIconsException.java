package com.mapbox.mapboxsdk.exceptions;

import com.mapbox.mapboxsdk.annotations.IconFactory;

/**
 * <p>
 * A TooManyIconsException is thrown by IconFactory when it
 * cannot create a Icon because there are already too many icons created.
 * </p>
 * You should try to reuse Icon objects whenever possible.
 *
 * @see IconFactory
 */
public class TooManyIconsException extends RuntimeException {

  public TooManyIconsException() {
    super("Cannot create an Icon because there are already too many. Try reusing Icon objects whenever possible.");
  }

}
