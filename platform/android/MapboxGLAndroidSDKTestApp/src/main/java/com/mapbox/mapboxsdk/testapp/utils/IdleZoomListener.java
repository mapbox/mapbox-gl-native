package com.mapbox.mapboxsdk.testapp.utils;

import android.content.Context;
import android.widget.TextView;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;

public class IdleZoomListener implements MapboxMap.OnCameraIdleListener {

  private MapboxMap mapboxMap;
  private TextView textView;

  public IdleZoomListener(MapboxMap mapboxMap, TextView textView) {
    this.mapboxMap = mapboxMap;
    this.textView = textView;
  }

  @Override
  public void onCameraIdle() {
    Context context = textView.getContext();
    CameraPosition position = mapboxMap.getCameraPosition();
    textView.setText(String.format(context.getString(R.string.debug_zoom), position.zoom));
  }
}