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
    setTag(false);
  }

  @Override
  public boolean dispatchTouchEvent(MotionEvent ev) {
    super.dispatchTouchEvent(ev);
    boolean actionUp = (boolean) getTag();
    if (!actionUp) {
      ((ViewGroup) getParent()).onTouchEvent(ev);
    } else {
      setTag(false);
    }
    return true;
  }
}
