package com.mapbox.mapboxsdk.maps;

import android.support.annotation.Nullable;

interface IMapView {
  void setForegroundColor(int foregroundLoadColor);

  void inflateInternalViews();

  void setViewOptions();

  void createTextureView(@Nullable String localFontFamily, boolean translucent);

  void createSurfaceView(@Nullable String localFontFamily, boolean renderSurfaceOnTop);

  float getDensity();

  void createNativeMapView(float pixelRatio, boolean crossSourceCollisions, MapChangeReceiver mapChangeReceiver);
}
