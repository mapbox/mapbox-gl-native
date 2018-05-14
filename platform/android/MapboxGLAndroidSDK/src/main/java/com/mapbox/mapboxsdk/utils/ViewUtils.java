package com.mapbox.mapboxsdk.utils;

import android.os.Build;
import android.support.annotation.NonNull;
import android.view.View;
import android.widget.FrameLayout;

public class ViewUtils {

  /**
   * Programatically set View's gravity.
   *
   * @param view    the view
   * @param gravity the gravity
   */
  public static void setViewGravity(@NonNull final View view, int gravity) {
    FrameLayout.LayoutParams layoutParams = (FrameLayout.LayoutParams) view.getLayoutParams();
    layoutParams.gravity = gravity;
    view.setLayoutParams(layoutParams);
  }

  /**
   * Programatically set View's margins.
   * @param view the view
   * @param initMargins initial margins
   * @param left left margin
   * @param top top margin
   * @param right right margin
   * @param bottom bottom margin
   */
  public static void setViewMargins(@NonNull final View view, int left, int top, int right, int bottom) {
    // convert initial margins with padding
    FrameLayout.LayoutParams layoutParams = (FrameLayout.LayoutParams) view.getLayoutParams();
    layoutParams.setMargins(left, top, right, bottom);

    // support RTL
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
      layoutParams.setMarginStart(left);
      layoutParams.setMarginEnd(right);
    }

    view.setLayoutParams(layoutParams);
  }
}
