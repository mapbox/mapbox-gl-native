package com.mapbox.mapboxsdk.maps;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;
import android.text.TextUtils;

import com.mapbox.mapboxsdk.constants.MapboxConstants;

class MapState extends MapThreadExecutor implements State {

  private String apiBaseUrl;
  private String styleUrl;
  private boolean debug;
  private long transitionDuration;
  private long transitionDelay;

  MapState(@NonNull NativeMapView nativeMapView, @NonNull ThreadExecutor invocation) {
    super(nativeMapView, invocation);
  }

  @UiThread
  void setApiBaseUrl(@NonNull final String apiBaseUrl) {
    this.apiBaseUrl = apiBaseUrl;
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(NativeMapView nativeMapView) {
        nativeMapView.setApiBaseUrl(apiBaseUrl);
      }
    });
  }

  @UiThread
  String getStyleUrl() {
    return styleUrl;
  }

  @UiThread
  void setStyleUrl(final String styleUrl) {
    this.styleUrl = styleUrl;
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(NativeMapView nativeMapView) {
        nativeMapView.setStyleUrl(styleUrl);
      }
    });
  }

  @UiThread
  boolean getDebug() {
    return debug;
  }

  @UiThread
  void setDebug(final boolean debug) {
    this.debug = debug;
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(NativeMapView nativeMapView) {
        nativeMapView.setDebug(debug);
      }
    });
  }

  @UiThread
  void cycleDebugOptions() {
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.cycleDebugOptions();
      }
    });
  }

  long getTransitionDuration() {
    return transitionDuration;
  }

  void setTransitionDuration(final long transitionDuration) {
    this.transitionDuration = transitionDuration;
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.setTransitionDuration(transitionDuration);
      }
    });
  }

  long getTransitionDelay() {
    return transitionDelay;
  }

  void setTransitionDelay(final long transitionDelay) {
    this.transitionDelay = transitionDelay;
    queueRenderEvent(new MapRunnable() {
      @Override
      public void execute(@NonNull NativeMapView nativeMapView) {
        nativeMapView.setTransitionDuration(transitionDelay);
      }
    });
  }

  @UiThread
  public void onSaveInstanceState(Bundle outState) {
    outState.putBoolean(MapboxConstants.STATE_DEBUG_ACTIVE, debug);
    outState.putString(MapboxConstants.STATE_STYLE_URL, styleUrl);
    outState.putString(MapboxConstants.STATE_API_BASE_URL, apiBaseUrl);
    outState.putLong(MapboxConstants.STATE_TRANSITION_DELAY, transitionDelay);
    outState.putLong(MapboxConstants.STATE_TRANSITION_DURATION, transitionDuration);
  }

  @UiThread
  public void onRestoreInstanceState(Bundle savedInstanceState) {
    boolean debug = savedInstanceState.getBoolean(MapboxConstants.STATE_DEBUG_ACTIVE);
    if (debug) {
      setDebug(debug);
    }

    String styleUrl = savedInstanceState.getString(MapboxConstants.STATE_STYLE_URL);
    if (!TextUtils.isEmpty(styleUrl)) {
      setStyleUrl(styleUrl);
    }

    String apiBaseUrl = savedInstanceState.getString(MapboxConstants.STATE_API_BASE_URL);
    if (!TextUtils.isEmpty(apiBaseUrl)) {
      setApiBaseUrl(apiBaseUrl);
    }

    long transitionDuration = savedInstanceState.getLong(MapboxConstants.STATE_TRANSITION_DURATION);
    if (transitionDuration != 0) {
      setTransitionDuration(transitionDuration);
    }

    long transitionDelay = savedInstanceState.getLong(MapboxConstants.STATE_TRANSITION_DELAY);
    if (transitionDelay != 0) {
      setTransitionDelay(transitionDelay);
    }
  }
}
