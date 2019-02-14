package com.mapbox.mapboxsdk.maps;

import android.content.Context;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.maps.renderer.MapRenderer;

class MapPresenterImpl implements MapPresenter {
  private final MapView mapView;
  private final MapboxMapOptions mapboxMapOptions;
  private MapRenderer renderer;

  MapPresenterImpl(@NonNull MapView mapView, @NonNull Context context, @NonNull MapboxMapOptions mapboxMapOptions) {
    this.mapView = mapView;
    this.mapboxMapOptions = mapboxMapOptions;
    initialize();
  }

  private void initialize() {
    mapView.setForegroundColor(mapboxMapOptions.getForegroundLoadColor());
    mapView.inflateInternalViews();
    mapView.setViewOptions();

    String localFontFamily = mapboxMapOptions.getLocalIdeographFontFamily();
    if (mapboxMapOptions.getTextureMode()) {
      renderer = mapView.initializeTextureView(localFontFamily, mapboxMapOptions.getTranslucentTextureSurface());
    } else {
      renderer = mapView.initializeSurfaceView(localFontFamily);
    }
  }
}
