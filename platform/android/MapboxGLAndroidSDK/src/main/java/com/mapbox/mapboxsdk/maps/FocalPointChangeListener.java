package com.mapbox.mapboxsdk.maps;

import android.graphics.PointF;
import android.support.annotation.NonNull;

/**
 * Interface definition of a callback that is invoked when the focal point will change.
 */
public interface FocalPointChangeListener {

  void onFocalPointChanged(@NonNull PointF pointF);
}
