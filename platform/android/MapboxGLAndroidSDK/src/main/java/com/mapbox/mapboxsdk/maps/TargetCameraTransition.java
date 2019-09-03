package com.mapbox.mapboxsdk.maps;

import com.mapbox.mapboxsdk.geometry.LatLng;

public class TargetCameraTransition extends CameraTransition<LatLng> {

  private final LatLng latLng = new LatLng();

  public TargetCameraTransition(int type, long delay, long duration, LatLng endValue) {
    super(type, duration, delay, endValue);
  }

  @Override
  int getCameraProperty() {
    return PROPERTY_CENTER;
  }

  @Override
  LatLng onFrame(double currentTime) {
    double fraction = (currentTime - startTime) / duration;

    latLng.setLatitude(startValue.getLatitude()
      + ((endValue.getLatitude() - startValue.getLatitude()) * fraction));
    latLng.setLongitude(startValue.getLongitude()
      + ((endValue.getLongitude() - startValue.getLongitude()) * fraction));

    return latLng;
  }
}
