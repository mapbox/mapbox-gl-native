package com.mapbox.mapboxsdk.exceptions;

/**
 * A ConversionException is thrown when a conversion failed to execute.
 */
public class ConversionException extends RuntimeException {

  public ConversionException(String detailMessage) {
    super(detailMessage);
  }
}
