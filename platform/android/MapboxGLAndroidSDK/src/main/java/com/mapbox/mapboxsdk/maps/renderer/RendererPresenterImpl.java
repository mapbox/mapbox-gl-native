package com.mapbox.mapboxsdk.maps.renderer;

import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.maps.MapPresenter;

public class RendererPresenterImpl implements RendererPresenter {
  private final MapRenderer renderer;
  private final MapPresenter mapPresenter;

  public RendererPresenterImpl(@NonNull MapRenderer renderer, @NonNull MapPresenter mapPresenter) {
    this.renderer = renderer;
    this.mapPresenter = mapPresenter;
  }
}
