package com.mapbox.mapboxsdk.style.layers;

/**
 * Thrown when adding a layer to a map twice
 */
public class CannotAddLayerException extends RuntimeException {

  public CannotAddLayerException(String message) {
    super(message);
  }

}
