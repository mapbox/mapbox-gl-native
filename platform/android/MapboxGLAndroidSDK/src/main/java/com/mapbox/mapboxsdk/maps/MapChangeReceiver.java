package com.mapbox.mapboxsdk.maps;

import com.mapbox.mapboxsdk.log.Logger;

import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

class MapChangeReceiver implements NativeMapView.StateCallback {

  private static final String TAG = "Mbgl-MapChangeReceiver";

  private final List<MapView.OnCameraWillChangeListener> onCameraWillChangeListenerList = new CopyOnWriteArrayList<>();
  private final List<MapView.OnCameraIsChangingListener> onCameraIsChangingListenerList = new CopyOnWriteArrayList<>();
  private final List<MapView.OnCameraDidChangeListener> onCameraDidChangeListenerList = new CopyOnWriteArrayList<>();
  private final List<MapView.OnWillStartLoadingMapListener> onWillStartLoadingMapListenerList
    = new CopyOnWriteArrayList<>();
  private final List<MapView.OnDidFinishLoadingMapListener> onDidFinishLoadingMapListenerList
    = new CopyOnWriteArrayList<>();
  private final List<MapView.OnDidFailLoadingMapListener> onDidFailLoadingMapListenerList
    = new CopyOnWriteArrayList<>();
  private final List<MapView.OnWillStartRenderingFrameListener> onWillStartRenderingFrameList
    = new CopyOnWriteArrayList<>();
  private final List<MapView.OnDidFinishRenderingFrameListener> onDidFinishRenderingFrameList
    = new CopyOnWriteArrayList<>();
  private final List<MapView.OnWillStartRenderingMapListener> onWillStartRenderingMapListenerList
    = new CopyOnWriteArrayList<>();
  private final List<MapView.OnDidFinishRenderingMapListener> onDidFinishRenderingMapListenerList
    = new CopyOnWriteArrayList<>();
  private final List<MapView.OnDidEnterIdleListener> onDidEnterIdleListenerList
      = new CopyOnWriteArrayList<>();
  private final List<MapView.OnDidFinishLoadingStyleListener> onDidFinishLoadingStyleListenerList
    = new CopyOnWriteArrayList<>();
  private final List<MapView.OnSourceChangedListener> onSourceChangedListenerList = new CopyOnWriteArrayList<>();

  @Override
  public void onCameraWillChange(boolean animated) {
    try {
      if (!onCameraWillChangeListenerList.isEmpty()) {
        for (MapView.OnCameraWillChangeListener onCameraWillChangeListener : onCameraWillChangeListenerList) {
          onCameraWillChangeListener.onCameraWillChange(animated);
        }
      }
    } catch (Throwable err) {
      Logger.e(TAG, "Exception in onCameraWillChange", err);
      throw err;
    }
  }

  @Override
  public void onCameraIsChanging() {
    try {
      if (!onCameraIsChangingListenerList.isEmpty()) {
        for (MapView.OnCameraIsChangingListener onCameraIsChangingListener : onCameraIsChangingListenerList) {
          onCameraIsChangingListener.onCameraIsChanging();
        }
      }
    } catch (Throwable err) {
      Logger.e(TAG, "Exception in onCameraIsChanging", err);
      throw err;
    }
  }

  @Override
  public void onCameraDidChange(boolean animated) {
    try {
      if (!onCameraDidChangeListenerList.isEmpty()) {
        for (MapView.OnCameraDidChangeListener onCameraDidChangeListener : onCameraDidChangeListenerList) {
          onCameraDidChangeListener.onCameraDidChange(animated);
        }
      }
    } catch (Throwable err) {
      Logger.e(TAG, "Exception in onCameraDidChange", err);
      throw err;
    }
  }

  @Override
  public void onWillStartLoadingMap() {
    try {
      if (!onWillStartLoadingMapListenerList.isEmpty()) {
        for (MapView.OnWillStartLoadingMapListener onWillStartLoadingMapListener : onWillStartLoadingMapListenerList) {
          onWillStartLoadingMapListener.onWillStartLoadingMap();
        }
      }
    } catch (Throwable err) {
      Logger.e(TAG, "Exception in onWillStartLoadingMap", err);
      throw err;
    }
  }

  @Override
  public void onDidFinishLoadingMap() {
    try {
      if (!onDidFinishLoadingMapListenerList.isEmpty()) {
        for (MapView.OnDidFinishLoadingMapListener onDidFinishLoadingMapListener : onDidFinishLoadingMapListenerList) {
          onDidFinishLoadingMapListener.onDidFinishLoadingMap();
        }
      }
    } catch (Throwable err) {
      Logger.e(TAG, "Exception in onDidFinishLoadingMap", err);
      throw err;
    }
  }

  @Override
  public void onDidFailLoadingMap(String error) {
    try {
      if (!onDidFailLoadingMapListenerList.isEmpty()) {
        for (MapView.OnDidFailLoadingMapListener onDidFailLoadingMapListener : onDidFailLoadingMapListenerList) {
          onDidFailLoadingMapListener.onDidFailLoadingMap(error);
        }
      }
    } catch (Throwable err) {
      Logger.e(TAG, "Exception in onDidFailLoadingMap", err);
      throw err;
    }
  }

  @Override
  public void onWillStartRenderingFrame() {
    try {
      if (!onWillStartRenderingFrameList.isEmpty()) {
        for (MapView.OnWillStartRenderingFrameListener listener : onWillStartRenderingFrameList) {
          listener.onWillStartRenderingFrame();
        }
      }
    } catch (Throwable err) {
      Logger.e(TAG, "Exception in onWillStartRenderingFrame", err);
      throw err;
    }
  }

  @Override
  public void onDidFinishRenderingFrame(boolean fully) {
    try {
      if (!onDidFinishRenderingFrameList.isEmpty()) {
        for (MapView.OnDidFinishRenderingFrameListener listener : onDidFinishRenderingFrameList) {
          listener.onDidFinishRenderingFrame(fully);
        }
      }
    } catch (Throwable err) {
      Logger.e(TAG, "Exception in onDidFinishRenderingFrame", err);
      throw err;
    }
  }

  @Override
  public void onWillStartRenderingMap() {
    try {
      if (!onWillStartRenderingMapListenerList.isEmpty()) {
        for (MapView.OnWillStartRenderingMapListener listener : onWillStartRenderingMapListenerList) {
          listener.onWillStartRenderingMap();
        }
      }
    } catch (Throwable err) {
      Logger.e(TAG, "Exception in onWillStartRenderingMap", err);
      throw err;
    }
  }

  @Override
  public void onDidFinishRenderingMap(boolean fully) {
    try {
      if (!onDidFinishRenderingMapListenerList.isEmpty()) {
        for (MapView.OnDidFinishRenderingMapListener listener : onDidFinishRenderingMapListenerList) {
          listener.onDidFinishRenderingMap(fully);
        }
      }
    } catch (Throwable err) {
      Logger.e(TAG, "Exception in onDidFinishRenderingMap", err);
      throw err;
    }
  }

  @Override
  public void onDidEnterIdle() {
    try {
      if (!onDidEnterIdleListenerList.isEmpty()) {
        for (MapView.OnDidEnterIdleListener listener : onDidEnterIdleListenerList) {
          listener.onDidEnterIdle();
        }
      }
    } catch (Throwable err) {
      Logger.e(TAG, "Exception in onDidEnterIdle", err);
      throw err;
    }
  }

  @Override
  public void onDidFinishLoadingStyle() {
    try {
      if (!onDidFinishLoadingStyleListenerList.isEmpty()) {
        for (MapView.OnDidFinishLoadingStyleListener listener : onDidFinishLoadingStyleListenerList) {
          listener.onDidFinishLoadingStyle();
        }
      }
    } catch (Throwable err) {
      Logger.e(TAG, "Exception in onDidFinishLoadingStyle", err);
      throw err;
    }
  }

  @Override
  public void onSourceChanged(String sourceId) {
    try {
      if (!onSourceChangedListenerList.isEmpty()) {
        for (MapView.OnSourceChangedListener onSourceChangedListener : onSourceChangedListenerList) {
          onSourceChangedListener.onSourceChangedListener(sourceId);
        }
      }
    } catch (Throwable err) {
      Logger.e(TAG, "Exception in onSourceChanged", err);
      throw err;
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

  void addOnDidEnterIdleListener(MapView.OnDidEnterIdleListener listener) {
    onDidEnterIdleListenerList.add(listener);
  }

  void removeOnDidEnterIdleListener(MapView.OnDidEnterIdleListener listener) {
    onDidEnterIdleListenerList.remove(listener);
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
    onDidEnterIdleListenerList.clear();
    onDidFinishLoadingStyleListenerList.clear();
    onSourceChangedListenerList.clear();
  }
}