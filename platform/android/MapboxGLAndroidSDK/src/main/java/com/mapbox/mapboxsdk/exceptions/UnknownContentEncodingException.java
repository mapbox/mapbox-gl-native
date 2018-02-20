package com.mapbox.mapboxsdk.exceptions;

/**
 * An UnknownContentEncodingException is thrown by HTTPRequest
 * when there aren't enough LatLng to create a bounds.
 */
public class UnknownContentEncodingException extends RuntimeException {

  public UnknownContentEncodingException(String encoding) {
    super("Unknown content encoding '" + encoding + "'");
  }
}
