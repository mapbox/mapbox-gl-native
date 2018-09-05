package com.mapbox.mapboxsdk;

class MapStrictModeException extends RuntimeException {
  MapStrictModeException(String message) {
    super(String.format("Map detected an error that would fail silently otherwise: %s", message));
  }
}
