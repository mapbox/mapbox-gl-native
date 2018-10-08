package com.mapbox.mapboxsdk.maps;

import java.util.ArrayList;
import java.util.List;

class MapChangeReceiver implements NativeMapView.StateCallback {

  private final List<MapView.OnCameraWillChangeListener> onCameraWillChangeListenerList = new ArrayList<>();
  private final List<MapView.OnCameraIsChangingListener> onCameraIsChangingListenerList = new ArrayList<>();
  private final List<MapView.OnCameraDidChangeListener> onCameraDidChangeListenerList = new ArrayList<>();
  private final List<MapView.OnWillStartLoadingMapListener> onWillStartLoadingMapListenerList = new ArrayList<>();
  private final List<MapView.OnDidFinishLoadingMapListener> onDidFinishLoadingMapListenerList = new ArrayList<>();
  private final List<MapView.OnDidFailLoadingMapListener> onDidFailLoadingMapListenerList = new ArrayList<>();
  private final List<MapView.OnWillStartRenderingFrameListener> onWillStartRenderingFrameList = new ArrayList<>();
  private final List<MapView.OnDidFinishRenderingFrameListener> onDidFinishRenderingFrameList = new ArrayList<>();
  private final List<MapView.OnWillStartRenderingMapListener> onWillStartRenderingMapListenerList = new ArrayList<>();
  private final List<MapView.OnDidFinishRenderingMapListener> onDidFinishRenderingMapListenerList = new ArrayList<>();
  private final List<MapView.OnDidFinishLoadingStyleListener> onDidFinishLoadingStyleListenerList = new ArrayList<>();
  private final List<MapView.OnSourceChangedListener> onSourceChangedListenerList = new ArrayList<>();

  @Override
  public void onCameraWillChange(boolean animated) {
    if (!onCameraWillChangeListenerList.isEmpty()) {
      for (MapView.OnCameraWillChangeListener onCameraWillChangeListener : onCameraWillChangeListenerList) {
        onCameraWillChangeListener.onCameraWillChange(animated);
      }
    }
  }

  @Override
  public void onCameraIsChanging() {
    if (!onCameraIsChangingListenerList.isEmpty()) {
      for (MapView.OnCameraIsChangingListener onCameraIsChangingListener : onCameraIsChangingListenerList) {
        onCameraIsChangingListener.onCameraIsChanging();
      }
    }
  }

  @Override
  public void onCameraDidChange(boolean animated) {
    if (!onCameraDidChangeListenerList.isEmpty()) {
      for (MapView.OnCameraDidChangeListener onCameraDidChangeListener : onCameraDidChangeListenerList) {
        onCameraDidChangeListener.onCameraDidChange(animated);
      }
    }
  }

  @Override
  public void onWillStartLoadingMap() {
    if (!onWillStartLoadingMapListenerList.isEmpty()) {
      for (MapView.OnWillStartLoadingMapListener onWillStartLoadingMapListener : onWillStartLoadingMapListenerList) {
        onWillStartLoadingMapListener.onWillStartLoadingMap();
      }
    }
  }

  @Override
  public void onDidFinishLoadingMap() {
    if (!onDidFinishLoadingMapListenerList.isEmpty()) {
      for (MapView.OnDidFinishLoadingMapListener onDidFinishLoadingMapListener : onDidFinishLoadingMapListenerList) {
        onDidFinishLoadingMapListener.onDidFinishLoadingMap();
      }
    }
  }

  @Override
  public void onDidFailLoadingMap(String error) {
    if (!onDidFailLoadingMapListenerList.isEmpty()) {
      for (MapView.OnDidFailLoadingMapListener onDidFailLoadingMapListener : onDidFailLoadingMapListenerList) {
        onDidFailLoadingMapListener.onDidFailLoadingMap(error);
      }
    }
  }

  @Override
  public void onWillStartRenderingFrame() {
    if (!onWillStartRenderingFrameList.isEmpty()) {
      for (MapView.OnWillStartRenderingFrameListener listener : onWillStartRenderingFrameList) {
        listener.onWillStartRenderingFrame();
      }
    }
  }

  @Override
  public void onDidFinishRenderingFrame(boolean fully) {
    if (!onDidFinishRenderingFrameList.isEmpty()) {
      for (MapView.OnDidFinishRenderingFrameListener listener : onDidFinishRenderingFrameList) {
        listener.onDidFinishRenderingFrame(fully);
      }
    }
  }

  @Override
  public void onWillStartRenderingMap() {
    if (!onWillStartRenderingMapListenerList.isEmpty()) {
      for (MapView.OnWillStartRenderingMapListener listener : onWillStartRenderingMapListenerList) {
        listener.onWillStartRenderingMap();
      }
    }
  }

  @Override
  public void onDidFinishRenderingMap(boolean fully) {
    if (!onDidFinishRenderingMapListenerList.isEmpty()) {
      for (MapView.OnDidFinishRenderingMapListener listener : onDidFinishRenderingMapListenerList) {
        listener.onDidFinishRenderingMap(fully);
      }
    }
  }

  @Override
  public void onDidFinishLoadingStyle() {
    if (!onDidFinishLoadingStyleListenerList.isEmpty()) {
      for (MapView.OnDidFinishLoadingStyleListener listener : onDidFinishLoadingStyleListenerList) {
        listener.onDidFinishLoadingStyle();
      }
    }
  }

  @Override
  public void onSourceChanged(String sourceId) {
    if (!onSourceChangedListenerList.isEmpty()) {
      for (MapView.OnSourceChangedListener onSourceChangedListener : onSourceChangedListenerList) {
        onSourceChangedListener.onSourceChangedListener(sourceId);
      }
    }
  }

  void addOnCameraWillChangeListener(MapView.OnCameraWillChangeListener listener) {
    onCameraWillChangeListenerList.add(listener);
  }

  void removeOnCameraWillChangeListener(MapView.OnCameraWillChangeListener listener) {
    onCameraWillChangeListenerList.remove(listener);
  }

  void addOnCameraIsChangingListener(MapView.OnCameraIsChangingListener listener) {
    onCameraIsChangingListenerList.add(listener);
  }

  void removeOnCameraIsChangingListener(MapView.OnCameraIsChangingListener listener) {
    onCameraIsChangingListenerList.remove(listener);
  }

  void addOnCameraDidChangeListener(MapView.OnCameraDidChangeListener listener) {
    onCameraDidChangeListenerList.add(listener);
  }

  void removeOnCameraDidChangeListener(MapView.OnCameraDidChangeListener listener) {
    onCameraDidChangeListenerList.remove(listener);
  }

  void addOnWillStartLoadingMapListener(MapView.OnWillStartLoadingMapListener listener) {
    onWillStartLoadingMapListenerList.add(listener);
  }

  void removeOnWillStartLoadingMapListener(MapView.OnWillStartLoadingMapListener listener) {
    onWillStartLoadingMapListenerList.remove(listener);
  }

  void addOnDidFinishLoadingMapListener(MapView.OnDidFinishLoadingMapListener listener) {
    onDidFinishLoadingMapListenerList.add(listener);
  }

  void removeOnDidFinishLoadingMapListener(MapView.OnDidFinishLoadingMapListener listener) {
    onDidFinishLoadingMapListenerList.remove(listener);
  }

  void addOnDidFailLoadingMapListener(MapView.OnDidFailLoadingMapListener listener) {
    onDidFailLoadingMapListenerList.add(listener);
  }

  void removeOnDidFailLoadingMapListener(MapView.OnDidFailLoadingMapListener listener) {
    onDidFailLoadingMapListenerList.remove(listener);
  }

  void addOnWillStartRenderingFrameListener(MapView.OnWillStartRenderingFrameListener listener) {
    onWillStartRenderingFrameList.add(listener);
  }

  void removeOnWillStartRenderingFrameListener(MapView.OnWillStartRenderingFrameListener listener) {
    onWillStartRenderingFrameList.remove(listener);
  }

  void addOnDidFinishRenderingFrameListener(MapView.OnDidFinishRenderingFrameListener listener) {
    onDidFinishRenderingFrameList.add(listener);
  }

  void removeOnDidFinishRenderingFrameListener(MapView.OnDidFinishRenderingFrameListener listener) {
    onDidFinishRenderingFrameList.remove(listener);
  }

  void addOnWillStartRenderingMapListener(MapView.OnWillStartRenderingMapListener listener) {
    onWillStartRenderingMapListenerList.add(listener);
  }

  void removeOnWillStartRenderingMapListener(MapView.OnWillStartRenderingMapListener listener) {
    onWillStartRenderingMapListenerList.remove(listener);
  }

  void addOnDidFinishRenderingMapListener(MapView.OnDidFinishRenderingMapListener listener) {
    onDidFinishRenderingMapListenerList.add(listener);
  }

  void removeOnDidFinishRenderingMapListener(MapView.OnDidFinishRenderingMapListener listener) {
    onDidFinishRenderingMapListenerList.remove(listener);
  }

  void addOnDidFinishLoadingStyleListener(MapView.OnDidFinishLoadingStyleListener listener) {
    onDidFinishLoadingStyleListenerList.add(listener);
  }

  void removeOnDidFinishLoadingStyleListener(MapView.OnDidFinishLoadingStyleListener listener) {
    onDidFinishLoadingStyleListenerList.remove(listener);
  }

  void addOnSourceChangedListener(MapView.OnSourceChangedListener listener) {
    onSourceChangedListenerList.add(listener);
  }

  void removeOnSourceChangedListener(MapView.OnSourceChangedListener listener) {
    onSourceChangedListenerList.remove(listener);
  }

  void clear() {
    onCameraWillChangeListenerList.clear();
    onCameraIsChangingListenerList.clear();
    onCameraDidChangeListenerList.clear();
    onWillStartLoadingMapListenerList.clear();
    onDidFinishLoadingMapListenerList.clear();
    onDidFailLoadingMapListenerList.clear();
    onWillStartRenderingFrameList.clear();
    onDidFinishRenderingFrameList.clear();
    onWillStartRenderingMapListenerList.clear();
    onDidFinishRenderingMapListenerList.clear();
    onDidFinishLoadingStyleListenerList.clear();
    onSourceChangedListenerList.clear();
  }
}