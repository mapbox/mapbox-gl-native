package com.mapbox.mapboxsdk.annotations;

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.ViewGroup;
import android.widget.FrameLayout;

/**
 * ViewGroup that dispatches TouchEvents to the parent ViewGroup.
 * <p>
 * This allows to dispatch touch events that occur on MarkerView to MapView.
 * https://github.com/mapbox/mapbox-gl-native/issues/5388
 * </p>
 */
public class MarkerViewContainer extends FrameLayout {

  public MarkerViewContainer(Context context, AttributeSet attrs) {
    super(context, attrs);
  }

  @Override
  public boolean dispatchTouchEvent(MotionEvent ev) {
    final boolean childResult = super.dispatchTouchEvent(ev);
    if (childResult) {
      ((ViewGroup) getParent()).onTouchEvent(ev);
    }
    return childResult;
  }
}
