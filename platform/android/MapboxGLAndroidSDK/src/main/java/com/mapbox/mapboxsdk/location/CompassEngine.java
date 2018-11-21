package com.mapbox.mapboxsdk.location;

import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.location.modes.CameraMode;
import com.mapbox.mapboxsdk.location.modes.RenderMode;

/**
 * Interface defining the source of compass heading data that is
 * consumed by the {@link LocationComponent} when in compass related
 * {@link RenderMode} or
 * {@link CameraMode}s.
 */
public interface CompassEngine {

  /**
   * Adds a {@link CompassListener} that can be used to
   * receive heading and state changes.
   *
   * @param compassListener to be added
   */
  void addCompassListener(@NonNull CompassListener compassListener);

  /**
   * Removes a {@link CompassListener} that can be used to
   * receive heading and state changes.
   *
   * @param compassListener to be removed
   */
  void removeCompassListener(@NonNull CompassListener compassListener);

  /**
   * Returns the last heading value produced and pushed via
   * a compass listener.
   *
   * @return last heading value
   */
  float getLastHeading();

  /**
   * Provides the last know accuracy status from the sensor manager.
   * <p>
   * An integer value which is identical to the {@code SensorManager} class constants:
   * <ul>
   * <li>{@link android.hardware.SensorManager#SENSOR_STATUS_NO_CONTACT}</li>
   * <li>{@link android.hardware.SensorManager#SENSOR_STATUS_UNRELIABLE}</li>
   * <li>{@link android.hardware.SensorManager#SENSOR_STATUS_ACCURACY_LOW}</li>
   * <li>{@link android.hardware.SensorManager#SENSOR_STATUS_ACCURACY_MEDIUM}</li>
   * <li>{@link android.hardware.SensorManager#SENSOR_STATUS_ACCURACY_HIGH}</li>
   * </ul>
   *
   * @return last accuracy status
   */
  int getLastAccuracySensorStatus();

  /**
   * Lifecycle method that can be used for adding or releasing resources.
   */
  void onStart();

  /**
   * Lifecycle method that can be used for adding or releasing resources.
   */
  void onStop();
}
