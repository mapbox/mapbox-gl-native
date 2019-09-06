package com.mapbox.mapboxsdk.maps;

import android.view.animation.Interpolator;

import com.mapbox.mapboxsdk.geometry.LatLng;

public class TargetCameraTransition extends CameraTransition<LatLng> {

  private final LatLng latLng = new LatLng();

  public TargetCameraTransition(int type, long duration, long delay, LatLng endValue, Interpolator interpolator) {
    super(type, duration, delay, endValue, interpolator);
  }

  @Override
  int getCameraProperty() {
    return PROPERTY_CENTER;
  }

  @Override
  protected LatLng getAnimatedValue(double fraction) {
    latLng.setLatitude(getStartValue().getLatitude()
      + ((getEndValue().getLatitude() - getStartValue().getLatitude()) * fraction));
    latLng.setLongitude(getStartValue().getLongitude()
      + ((getEndValue().getLongitude() - getStartValue().getLongitude()) * fraction));
    return latLng;
  }

  public static class Builder extends CameraTransition.Builder<TargetCameraTransition, LatLng> {

    public Builder(LatLng endValue) {
      super(endValue);
    }

    @Override
    public TargetCameraTransition build() {
      return new TargetCameraTransition(reason, durationMillis, delayMillis, endValue, interpolator);
    }
  }
}
