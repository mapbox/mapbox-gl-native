package com.mapbox.mapboxsdk.maps;

import com.mapbox.mapboxsdk.MapboxPresenter;

public interface MapPresenter extends MapboxPresenter<MapView> {

  void onSurfaceCreated();

  void addOnCameraWillChangeListener(MapView.OnCameraWillChangeListener listener);

  void removeOnCameraWillChangeListener(MapView.OnCameraWillChangeListener listener);

  void addOnCameraIsChangingListener(MapView.OnCameraIsChangingListener listener);

  void removeOnCameraIsChangingListener(MapView.OnCameraIsChangingListener listener);

  void addOnCameraDidChangeListener(MapView.OnCameraDidChangeListener listener);

  void removeOnCameraDidChangeListener(MapView.OnCameraDidChangeListener listener);

  void addOnWillStartLoadingMapListener(MapView.OnWillStartLoadingMapListener listener);

  void removeOnWillStartLoadingMapListener(MapView.OnWillStartLoadingMapListener listener);

  void addOnDidFinishLoadingMapListener(MapView.OnDidFinishLoadingMapListener listener);

  void removeOnDidFinishLoadingMapListener(MapView.OnDidFinishLoadingMapListener listener);

  void addOnDidFailLoadingMapListener(MapView.OnDidFailLoadingMapListener listener);

  void removeOnDidFailLoadingMapListener(MapView.OnDidFailLoadingMapListener listener);

  void addOnWillStartRenderingFrameListener(MapView.OnWillStartRenderingFrameListener listener);

  void removeOnWillStartRenderingFrameListener(MapView.OnWillStartRenderingFrameListener listener);

  void addOnDidFinishRenderingFrameListener(MapView.OnDidFinishRenderingFrameListener listener);

  void removeOnDidFinishRenderingFrameListener(MapView.OnDidFinishRenderingFrameListener listener);

  void addOnWillStartRenderingMapListener(MapView.OnWillStartRenderingMapListener listener);

  void removeOnWillStartRenderingMapListener(MapView.OnWillStartRenderingMapListener listener);

  void addOnDidFinishRenderingMapListener(MapView.OnDidFinishRenderingMapListener listener);

  void removeOnDidFinishRenderingMapListener(MapView.OnDidFinishRenderingMapListener listener);

  void addOnDidBecomeIdleListener(MapView.OnDidBecomeIdleListener listener);

  void removeOnDidBecomeIdleListener(MapView.OnDidBecomeIdleListener listener);

  void addOnDidFinishLoadingStyleListener(MapView.OnDidFinishLoadingStyleListener listener);

  void removeOnDidFinishLoadingStyleListener(MapView.OnDidFinishLoadingStyleListener listener);

  void addOnSourceChangedListener(MapView.OnSourceChangedListener listener);

  void removeOnSourceChangedListener(MapView.OnSourceChangedListener listener);
}
