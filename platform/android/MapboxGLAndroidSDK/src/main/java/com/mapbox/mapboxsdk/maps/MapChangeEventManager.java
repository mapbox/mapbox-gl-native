package com.mapbox.mapboxsdk.maps;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

import timber.log.Timber;

/*
 * Class responsible for forwarding map change events emitted by core.
 * <p>
 * Events are dispatched to end-user defined callbacks and MapView.MapChangeResultHandler.
 * </p>
 * <p>
 * This class is responsible for managing the deprecated MapView.OnMapChangeListener API.
 * </p>
 */
class MapChangeEventManager {

  // Support deprecated API
  private final CopyOnWriteArrayList<MapView.OnMapChangedListener> onMapChangedListeners = new CopyOnWriteArrayList<>();

  // End-user callbacks
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

  // Internal components callbacks
  private final List<OnMapReadyCallback> onMapReadyCallbackList = new ArrayList<>();
  private MapboxMap mapboxMap;
  private boolean initialLoad = true;

  void addOnCameraWillChangeListener(MapView.OnCameraWillChangeListener listener) {
    onCameraWillChangeListenerList.add(listener);
  }

  void removeOnCameraWillChangeListener(MapView.OnCameraWillChangeListener listener) {
    if (onCameraWillChangeListenerList.contains(listener)) {
      onCameraWillChangeListenerList.remove(listener);
    }
  }

  void addOnCameraIsChangingListener(MapView.OnCameraIsChangingListener listener) {
    onCameraIsChangingListenerList.add(listener);
  }

  void removeOnCameraIsChangingListener(MapView.OnCameraIsChangingListener listener) {
    if (onCameraIsChangingListenerList.contains(listener)) {
      onCameraIsChangingListenerList.remove(listener);
    }
  }

  void addOnCameraDidChangeListener(MapView.OnCameraDidChangeListener listener) {
    onCameraDidChangeListenerList.add(listener);
  }

  void removeOnCameraDidChangeListener(MapView.OnCameraDidChangeListener listener) {
    if (onCameraDidChangeListenerList.contains(listener)) {
      onCameraDidChangeListenerList.remove(listener);
    }
  }

  void addOnWillStartLoadingMapListener(MapView.OnWillStartLoadingMapListener listener) {
    onWillStartLoadingMapListenerList.add(listener);
  }

  void removeOnWillStartLoadingMapListener(MapView.OnWillStartLoadingMapListener listener) {
    if (onWillStartLoadingMapListenerList.contains(listener)) {
      onWillStartLoadingMapListenerList.remove(listener);
    }
  }

  void addOnDidFinishLoadingMapListener(MapView.OnDidFinishLoadingMapListener listener) {
    onDidFinishLoadingMapListenerList.add(listener);
  }

  void removeOnDidFinishLoadingMapListener(MapView.OnDidFinishLoadingMapListener listener) {
    if (onDidFinishLoadingMapListenerList.contains(listener)) {
      onDidFinishLoadingMapListenerList.remove(listener);
    }
  }

  void addOnDidFailLoadingMapListener(MapView.OnDidFailLoadingMapListener listener) {
    onDidFailLoadingMapListenerList.add(listener);
  }

  void removeOnDidFailLoadingMapListener(MapView.OnDidFailLoadingMapListener listener) {
    if (onDidFailLoadingMapListenerList.contains(listener)) {
      onDidFailLoadingMapListenerList.remove(listener);
    }
  }

  void addOnWillStartRenderingFrameListener(MapView.OnWillStartRenderingFrameListener listener) {
    onWillStartRenderingFrameList.add(listener);
  }

  void removeOnWillStartRenderingFrameListener(MapView.OnWillStartRenderingFrameListener listener) {
    if (onWillStartRenderingFrameList.contains(listener)) {
      onWillStartRenderingFrameList.remove(listener);
    }
  }

  void addOnDidFinishRenderingFrameListener(MapView.OnDidFinishRenderingFrameListener listener) {
    onDidFinishRenderingFrameList.add(listener);
  }

  void removeOnDidFinishRenderingFrameListener(MapView.OnDidFinishRenderingFrameListener listener) {
    if (onDidFinishRenderingFrameList.contains(listener)) {
      onDidFinishRenderingFrameList.remove(listener);
    }
  }

  void addOnWillStartRenderingMapListener(MapView.OnWillStartRenderingMapListener listener) {
    onWillStartRenderingMapListenerList.add(listener);
  }

  void removeOnWillStartRenderingMapListener(MapView.OnWillStartRenderingMapListener listener) {
    if (onWillStartRenderingMapListenerList.contains(listener)) {
      onWillStartRenderingMapListenerList.remove(listener);
    }
  }

  void addOnDidFinishRenderingMapListener(MapView.OnDidFinishRenderingMapListener listener) {
    onDidFinishRenderingMapListenerList.add(listener);
  }

  void removeOnDidFinishRenderingMapListener(MapView.OnDidFinishRenderingMapListener listener) {
    if (onDidFinishRenderingMapListenerList.contains(listener)) {
      onDidFinishRenderingMapListenerList.remove(listener);
    }
  }

  void addOnDidFinishLoadingStyleListener(MapView.OnDidFinishLoadingStyleListener listener) {
    if (!onDidFinishLoadingStyleListenerList.contains(listener)) {
      onDidFinishLoadingStyleListenerList.add(listener);
    }
  }

  void removeOnDidFinishLoadingStyleListener(MapView.OnDidFinishLoadingStyleListener listener) {
    if (onDidFinishLoadingStyleListenerList.contains(listener)) {
      onDidFinishLoadingStyleListenerList.remove(listener);
    }
  }

  void addOnSourceChangedListener(MapView.OnSourceChangedListener listener) {
    onSourceChangedListenerList.add(listener);
  }

  void removeOnSourceChangedListener(MapView.OnSourceChangedListener listener) {
    if (onSourceChangedListenerList.contains(listener)) {
      onSourceChangedListenerList.remove(listener);
    }
  }

  void onCameraWillChange(boolean animated) {
    if (!onCameraWillChangeListenerList.isEmpty()) {
      for (MapView.OnCameraWillChangeListener onCameraWillChangeListener : onCameraWillChangeListenerList) {
        onCameraWillChangeListener.onCameraWillChange(animated);
      }
    }
    onMapChange(animated ? MapView.REGION_WILL_CHANGE_ANIMATED : MapView.REGION_WILL_CHANGE);
  }

  void onCameraIsChanging() {
    if (!onCameraIsChangingListenerList.isEmpty()) {
      for (MapView.OnCameraIsChangingListener onCameraIsChangingListener : onCameraIsChangingListenerList) {
        if (onCameraIsChangingListener != null) {
          onCameraIsChangingListener.onCameraIsChanging();
        }
      }
    }
    if (mapboxMap != null) {
      mapboxMap.onCameraChange();
    }
    onMapChange(MapView.REGION_IS_CHANGING);
  }

  void onCameraDidChange(boolean animated) {
    if (!onCameraDidChangeListenerList.isEmpty()) {
      for (MapView.OnCameraDidChangeListener onCameraDidChangeListener : onCameraDidChangeListenerList) {
        onCameraDidChangeListener.onCameraDidChange(animated);
      }
    }
    if (animated && mapboxMap != null) {
      mapboxMap.onCameraDidChangeAnimated();
    } else if (mapboxMap != null) {
      mapboxMap.onCameraChange();
    }
    onMapChange(animated ? MapView.REGION_DID_CHANGE_ANIMATED : MapView.REGION_DID_CHANGE);
  }

  void onWillStartLoadingMap() {
    if (!onWillStartLoadingMapListenerList.isEmpty()) {
      for (MapView.OnWillStartLoadingMapListener onWillStartLoadingMapListener : onWillStartLoadingMapListenerList) {
        onWillStartLoadingMapListener.onWillStartLoadingMap();
      }
    }
    onMapChange(MapView.WILL_START_LOADING_MAP);
  }

  void onDidFinishLoadingMap() {
    if (!onDidFinishLoadingMapListenerList.isEmpty()) {
      for (MapView.OnDidFinishLoadingMapListener onDidFinishLoadingMapListener : onDidFinishLoadingMapListenerList) {
        onDidFinishLoadingMapListener.onDidFinishLoadingMap();
      }
    }

    // we require an additional update after the map has finished loading
    // in case an end user action hasn't been invoked at that time
    mapboxMap.onCameraChange();
    onMapChange(MapView.DID_FINISH_LOADING_MAP);
  }

  void onDidFailLoadingMap(String errorMessage) {
    if (!onDidFailLoadingMapListenerList.isEmpty()) {
      for (MapView.OnDidFailLoadingMapListener onDidFailLoadingMapListener : onDidFailLoadingMapListenerList) {
        onDidFailLoadingMapListener.onDidFailLoadingMap(errorMessage);
      }
    }
    onMapChange(MapView.DID_FAIL_LOADING_MAP);
  }

  void onWillStartRenderingFrame() {
    if (!onWillStartRenderingFrameList.isEmpty()) {
      for (MapView.OnWillStartRenderingFrameListener frameListener : onWillStartRenderingFrameList) {
        frameListener.onWillStartRenderingFrame();
      }
    }
    onMapChange(MapView.WILL_START_RENDERING_FRAME);
  }

  void onDidFinishRenderingFrame(boolean partial) {
    if (!onDidFinishRenderingFrameList.isEmpty()) {
      for (MapView.OnDidFinishRenderingFrameListener frameListener : onDidFinishRenderingFrameList) {
        frameListener.onDidFinishRenderingFrame(partial);
      }
    }

    if (partial && mapboxMap != null) {
      mapboxMap.onDidFinishRenderingFrame();
    } else if (mapboxMap != null) {
      mapboxMap.onDidFinishRenderingFrameFully();
    }
    onMapChange(partial ? MapView.DID_FINISH_RENDERING_FRAME : MapView.DID_FINISH_RENDERING_FRAME_FULLY_RENDERED);
  }

  void onWillStartRenderingMap() {
    if (!onWillStartRenderingMapListenerList.isEmpty()) {
      for (MapView.OnWillStartRenderingMapListener frameListener : onWillStartRenderingMapListenerList) {
        frameListener.onWillStartRenderingMap();
      }
    }
    onMapChange(MapView.WILL_START_RENDERING_MAP);
  }

  void onDidFinishRenderingMap(boolean partial) {
    if (!onDidFinishRenderingMapListenerList.isEmpty()) {
      for (MapView.OnDidFinishRenderingMapListener mapListener : onDidFinishRenderingMapListenerList) {
        mapListener.onDidFinishRenderingMap(partial);
      }
    }
    onMapChange(partial ? MapView.DID_FINISH_RENDERING_MAP : MapView.DID_FINISH_RENDERING_MAP_FULLY_RENDERED);
  }

  void onDidFinishLoadingStyle() {
    if (!onDidFinishLoadingStyleListenerList.isEmpty()) {
      for (MapView.OnDidFinishLoadingStyleListener styleListener : onDidFinishLoadingStyleListenerList) {
        styleListener.onDidFinishLoadingStyle();
      }
    }

    if (initialLoad) {
      initialLoad = false;
      mapboxMap.onPreMapReady();
      onMapReady();
      mapboxMap.onPostMapReady();
    }

    onMapChange(MapView.DID_FINISH_LOADING_STYLE);
  }

  void onSourceChanged(String id) {
    if (!onSourceChangedListenerList.isEmpty()) {
      for (MapView.OnSourceChangedListener onSourceChangedListener : onSourceChangedListenerList) {
        onSourceChangedListener.onSourceChangedListener(id);
      }
    }
    onMapChange(MapView.SOURCE_DID_CHANGE);
  }

  //
  // Deprecated API since 5.2.0
  //

  void onMapChange(int onMapChange) {
    if (!onMapChangedListeners.isEmpty()) {
      for (MapView.OnMapChangedListener onMapChangedListener : onMapChangedListeners) {
        try {
          onMapChangedListener.onMapChanged(onMapChange);
        } catch (RuntimeException err) {
          Timber.e("Exception (%s) in MapView.OnMapChangedListener: %s", err.getClass(), err.getMessage());
        }
      }
    }
  }

  void addOnMapChangedListener(MapView.OnMapChangedListener listener) {
    onMapChangedListeners.add(listener);
  }

  void removeOnMapChangedListener(MapView.OnMapChangedListener listener) {
    onMapChangedListeners.remove(listener);
  }

  //
  // internal api methods related to mapboxMap#getMapAsync()
  //

  private void onMapReady() {
    if (onMapReadyCallbackList.size() > 0) {
      // Notify listeners, clear when done
      Iterator<OnMapReadyCallback> iterator = onMapReadyCallbackList.iterator();
      while (iterator.hasNext()) {
        OnMapReadyCallback callback = iterator.next();
        callback.onMapReady(mapboxMap);
        iterator.remove();
      }
    }
  }

  boolean isInitialLoad() {
    return initialLoad;
  }

  void addOnMapReadyCallback(OnMapReadyCallback callback) {
    onMapReadyCallbackList.add(callback);
  }

  void clearOnMapReadyCallbacks() {
    onMapReadyCallbackList.clear();
  }

  void bind(MapboxMap map) {
    mapboxMap = map;
  }
}