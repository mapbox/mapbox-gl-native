package com.mapbox.mapboxsdk.maps;

import android.arch.lifecycle.LifecycleOwner;
import android.arch.lifecycle.LiveData;
import android.arch.lifecycle.MutableLiveData;
import android.arch.lifecycle.ViewModel;
import android.content.Context;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.text.TextUtils;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.Style;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import timber.log.Timber;

/**
 * Internal class used to manage map settings.
 */
public final class MapSettings extends ViewModel {
  private CameraPosition lastCameraPosition = CameraPosition.DEFAULT;
  private MutableLiveData<Double> maxZoom = new MutableLiveData<>();
  private MutableLiveData<Double> minZoom = new MutableLiveData<>();
  private MutableLiveData<Boolean> isDebugMode = new MutableLiveData<>();
  private MutableLiveData<Boolean> prefetchesTiles = new MutableLiveData<>();
  private MutableLiveData<String> styleUrl = new MutableLiveData<>();
  private MutableLiveData<String> apiBaseUrl = new MutableLiveData<>();

  void initialiseOptions(@NonNull MapboxMapOptions options) {
    setLastCameraPosition(options.getCamera());
    setMinZoom(options.getMinZoomPreference());
    setMaxZoom(options.getMaxZoomPreference());
    setDebugMode(options.getDebugActive());
    setPrefetchesTiles(options.getPrefetchesTiles());

    String apiBaseUrl = options.getApiBaseUrl();
    if (!TextUtils.isEmpty(apiBaseUrl)) {
      setApiBaseUrl(apiBaseUrl);
    }

    String style = options.getStyle();
    if (!TextUtils.isEmpty(style)) {
      setStyleUrl(style);
    } else {
      setStyleUrl(Style.MAPBOX_STREETS);
    }
  }

  @Nullable
  public CameraPosition getLastCameraPosition() {
    return lastCameraPosition;
  }

  public void setLastCameraPosition(@Nullable CameraPosition lastCameraPosition) {
    this.lastCameraPosition = lastCameraPosition;
  }

  public double getMinZoom() {
    return minZoom.getValue();
  }

  public MutableLiveData<Double> getMinZoomObservable() {
    return minZoom;
  }

  public void setMinZoom(double minZoom) {
    this.minZoom.setValue(minZoom);
  }

  public double getMaxZoom() {
    return maxZoom.getValue();
  }

  public MutableLiveData<Double> getMaxZoomObservable() {
    return maxZoom;
  }

  public void setMaxZoom(double maxZoom) {
    this.maxZoom.setValue(maxZoom);
  }

  public MutableLiveData<Boolean> isDebugModeObservable() {
    return isDebugMode;
  }

  public boolean isDebugMode() {
    return isDebugMode.getValue();
  }

  public void setDebugMode(boolean isDebugMode) {
    this.isDebugMode.setValue(isDebugMode);
  }

  public MutableLiveData<Boolean> getPrefetchesTilesObservable() {
    return prefetchesTiles;
  }

  public boolean getPrefetchesTiles() {
    return prefetchesTiles.getValue();
  }

  public void setPrefetchesTiles(boolean prefetchesTiles) {
    this.prefetchesTiles.setValue(prefetchesTiles);
  }

  public MutableLiveData<String> getStyleUrlObservable() {
    return styleUrl;
  }

  public String getStyleUrl() {
    return styleUrl.getValue();
  }

  public void setStyleUrl(String styleUrl) {
    if (!TextUtils.isEmpty(styleUrl)) {
      this.styleUrl.setValue(styleUrl);
    }
  }

  public MutableLiveData<String> getApiBaseUrlObservable() {
    return apiBaseUrl;
  }

  public void setApiBaseUrl(String apiBaseUrl) {
    this.apiBaseUrl.setValue(apiBaseUrl);
  }

  void onMapDestroy(@NonNull Context context) {
    clearObservers((LifecycleOwner) context);
  }

  private void clearObservers(@NonNull LifecycleOwner lifecycleOwner) {
    try {
      Field[] fields = this.getClass().getDeclaredFields();
      Method method = LiveData.class.getDeclaredMethod("removeObservers", LifecycleOwner.class);
      for (Field field : fields) {
        Class type = field.getType();
        if (type == MutableLiveData.class) {
          Object value = field.get(this);
          method.invoke(value, lifecycleOwner);
        }
      }
    } catch (NoSuchMethodException ex) {
      Timber.e("Unable to clear %s observers, %s.", this.getClass(), ex.getClass().getSimpleName());
      ex.printStackTrace();
    } catch (IllegalAccessException ex) {
      Timber.e("Unable to clear %s observers, %s.", this.getClass(), ex.getClass().getSimpleName());
      ex.printStackTrace();
    } catch (InvocationTargetException ex) {
      Timber.e("Unable to clear %s observers, %s.", this.getClass(), ex.getClass().getSimpleName());
      ex.printStackTrace();
    }
  }
}
