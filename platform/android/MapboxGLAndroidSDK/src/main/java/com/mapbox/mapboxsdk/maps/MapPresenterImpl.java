package com.mapbox.mapboxsdk.maps;

import android.content.Context;
import android.support.annotation.NonNull;

class MapPresenterImpl implements MapPresenter {

  private final MapChangeReceiver mapChangeReceiver;

  private final IMapView mapView;
  private final MapboxMapOptions mapboxMapOptions;

  MapPresenterImpl(@NonNull IMapView mapView, @NonNull Context context, @NonNull MapboxMapOptions mapboxMapOptions,
                   @NonNull MapChangeReceiver mapChangeReceiver) {
    this.mapView = mapView;
    this.mapboxMapOptions = mapboxMapOptions;
    this.mapChangeReceiver = mapChangeReceiver;

    mapView.setForegroundColor(mapboxMapOptions.getForegroundLoadColor());
    mapView.inflateInternalViews();
    mapView.setViewOptions();

    String localFontFamily = mapboxMapOptions.getLocalIdeographFontFamily();
    if (mapboxMapOptions.getTextureMode()) {
      mapView.createTextureView(localFontFamily, mapboxMapOptions.getTranslucentTextureSurface());
    } else {
      mapView.createSurfaceView(localFontFamily, mapboxMapOptions.getRenderSurfaceOnTop());
    }

    mapView.createNativeMapView(getPixelRatio(), mapboxMapOptions.getCrossSourceCollisions(), mapChangeReceiver);
  }

  @Override
  public void onSurfaceCreated() {

  }

  private float getPixelRatio() {
    // check is user defined his own pixel ratio value
    float pixelRatio = mapboxMapOptions.getPixelRatio();
    if (pixelRatio == 0) {
      // if not, get the one defined by the system
      pixelRatio = mapView.getDensity();
    }
    return pixelRatio;
  }

  @Override
  public void addOnCameraWillChangeListener(MapView.OnCameraWillChangeListener listener) {
    mapChangeReceiver.addOnCameraWillChangeListener(listener);
  }

  @Override
  public void removeOnCameraWillChangeListener(MapView.OnCameraWillChangeListener listener) {
    mapChangeReceiver.removeOnCameraWillChangeListener(listener);
  }

  @Override
  public void addOnCameraIsChangingListener(MapView.OnCameraIsChangingListener listener) {
    mapChangeReceiver.addOnCameraIsChangingListener(listener);
  }

  @Override
  public void removeOnCameraIsChangingListener(MapView.OnCameraIsChangingListener listener) {
    mapChangeReceiver.removeOnCameraIsChangingListener(listener);
  }

  @Override
  public void addOnCameraDidChangeListener(MapView.OnCameraDidChangeListener listener) {
    mapChangeReceiver.addOnCameraDidChangeListener(listener);
  }

  @Override
  public void removeOnCameraDidChangeListener(MapView.OnCameraDidChangeListener listener) {
    mapChangeReceiver.removeOnCameraDidChangeListener(listener);
  }

  @Override
  public void addOnWillStartLoadingMapListener(MapView.OnWillStartLoadingMapListener listener) {
    mapChangeReceiver.addOnWillStartLoadingMapListener(listener);
  }

  @Override
  public void removeOnWillStartLoadingMapListener(MapView.OnWillStartLoadingMapListener listener) {
    mapChangeReceiver.removeOnWillStartLoadingMapListener(listener);
  }

  @Override
  public void addOnDidFinishLoadingMapListener(MapView.OnDidFinishLoadingMapListener listener) {
    mapChangeReceiver.addOnDidFinishLoadingMapListener(listener);
  }

  @Override
  public void removeOnDidFinishLoadingMapListener(MapView.OnDidFinishLoadingMapListener listener) {
    mapChangeReceiver.removeOnDidFinishLoadingMapListener(listener);
  }

  @Override
  public void addOnDidFailLoadingMapListener(MapView.OnDidFailLoadingMapListener listener) {
    mapChangeReceiver.addOnDidFailLoadingMapListener(listener);
  }

  @Override
  public void removeOnDidFailLoadingMapListener(MapView.OnDidFailLoadingMapListener listener) {
    mapChangeReceiver.removeOnDidFailLoadingMapListener(listener);
  }

  @Override
  public void addOnWillStartRenderingFrameListener(MapView.OnWillStartRenderingFrameListener listener) {
    mapChangeReceiver.addOnWillStartRenderingFrameListener(listener);
  }

  @Override
  public void removeOnWillStartRenderingFrameListener(MapView.OnWillStartRenderingFrameListener listener) {

    mapChangeReceiver.removeOnWillStartRenderingFrameListener(listener);
  }

  @Override
  public void addOnDidFinishRenderingFrameListener(MapView.OnDidFinishRenderingFrameListener listener) {
    mapChangeReceiver.addOnDidFinishRenderingFrameListener(listener);
  }

  @Override
  public void removeOnDidFinishRenderingFrameListener(MapView.OnDidFinishRenderingFrameListener listener) {

    mapChangeReceiver.removeOnDidFinishRenderingFrameListener(listener);
  }

  @Override
  public void addOnWillStartRenderingMapListener(MapView.OnWillStartRenderingMapListener listener) {
    mapChangeReceiver.addOnWillStartRenderingMapListener(listener);
  }

  @Override
  public void removeOnWillStartRenderingMapListener(MapView.OnWillStartRenderingMapListener listener) {
    mapChangeReceiver.removeOnWillStartRenderingMapListener(listener);
  }

  @Override
  public void addOnDidFinishRenderingMapListener(MapView.OnDidFinishRenderingMapListener listener) {
    mapChangeReceiver.addOnDidFinishRenderingMapListener(listener);
  }

  @Override
  public void removeOnDidFinishRenderingMapListener(MapView.OnDidFinishRenderingMapListener listener) {
    mapChangeReceiver.removeOnDidFinishRenderingMapListener(listener);
  }

  @Override
  public void addOnDidBecomeIdleListener(MapView.OnDidBecomeIdleListener listener) {
    mapChangeReceiver.addOnDidBecomeIdleListener(listener);
  }

  @Override
  public void removeOnDidBecomeIdleListener(MapView.OnDidBecomeIdleListener listener) {
    mapChangeReceiver.removeOnDidBecomeIdleListener(listener);
  }

  @Override
  public void addOnDidFinishLoadingStyleListener(MapView.OnDidFinishLoadingStyleListener listener) {
    mapChangeReceiver.addOnDidFinishLoadingStyleListener(listener);
  }

  @Override
  public void removeOnDidFinishLoadingStyleListener(MapView.OnDidFinishLoadingStyleListener listener) {
    mapChangeReceiver.removeOnDidFinishLoadingStyleListener(listener);
  }

  @Override
  public void addOnSourceChangedListener(MapView.OnSourceChangedListener listener) {
    mapChangeReceiver.addOnSourceChangedListener(listener);
  }

  @Override
  public void removeOnSourceChangedListener(MapView.OnSourceChangedListener listener) {
    mapChangeReceiver.removeOnSourceChangedListener(listener);
  }
}
