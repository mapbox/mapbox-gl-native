package com.mapbox.mapboxsdk.maps;

import android.view.animation.Interpolator;

public class PaddingCameraTransition extends CameraTransition<Double[]> {

  public PaddingCameraTransition(int type, double duration, double delay, Double[] endValue, Interpolator interpolator) {
    super(type, duration, delay, endValue, interpolator);
  }

  @Override
  int getCameraProperty() {
    return PROPERTY_PADDING;
  }

  @Override
  Double[] getAnimatedValue(double fraction) {
    Double[] startValue = getStartValue();
    Double[] endValue = getEndValue();

    Double[] value = new Double[4];
    for (int i = 0; i < 4; i++) {
      value[i] = startValue[i] + ((endValue[i] - startValue[i]) * fraction);
    }

    return value;
  }
}
