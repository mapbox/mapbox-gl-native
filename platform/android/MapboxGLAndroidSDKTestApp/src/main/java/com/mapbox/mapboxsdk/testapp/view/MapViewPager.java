package com.mapbox.mapboxsdk.testapp.view;

import android.content.Context;
import android.support.v4.view.PagerTabStrip;
import android.support.v4.view.ViewPager;
import android.util.AttributeSet;
import android.view.SurfaceView;
import android.view.View;

public class MapViewPager extends ViewPager {

  public MapViewPager(Context context, AttributeSet attrs) {
    super(context, attrs);
  }

  @Override
  protected boolean canScroll(View v, boolean checkV, int dx, int x, int y) {
    return v instanceof SurfaceView || v instanceof PagerTabStrip || (super.canScroll(v, checkV, dx, x, y));
  }
}
