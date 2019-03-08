package com.mapbox.mapboxsdk.location;

final class LocationComponentNotInitializedException extends RuntimeException {
  LocationComponentNotInitializedException() {
    super("The LocationComponent has to be activated with one of the LocationComponent#activateLocationComponent"
      + " overloads before any other methods are invoked.");
  }
}
