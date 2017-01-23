package com.mapbox.mapboxsdk.exceptions;

/**
 * Thrown on conversion errors
 */
public class ConversionException extends RuntimeException {

  public ConversionException() {
  }

  public ConversionException(String detailMessage) {
    super(detailMessage);
  }

  public ConversionException(String detailMessage, Throwable throwable) {
    super(detailMessage, throwable);
  }

  public ConversionException(Throwable throwable) {
    super(throwable);
  }
}
