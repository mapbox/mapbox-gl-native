package com.mapbox.mapboxsdk.location;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.SystemClock;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.Surface;
import android.view.WindowManager;

import java.util.ArrayList;
import java.util.List;

import timber.log.Timber;

/**
 * This manager class handles compass events such as starting the tracking of device bearing, or
 * when a new compass update occurs.
 */
class LocationComponentCompassEngine implements CompassEngine, SensorEventListener {

  // The rate sensor events will be delivered at. As the Android documentation states, this is only
  // a hint to the system and the events might actually be received faster or slower then this
  // specified rate. Since the minimum Android API levels about 9, we are able to set this value
  // ourselves rather than using one of the provided constants which deliver updates too quickly for
  // our use case. The default is set to 100ms
  private static final int SENSOR_DELAY_MICROS = 100 * 1000;
  // Filtering coefficient 0 < ALPHA < 1
  private static final float ALPHA = 0.45f;

  private final WindowManager windowManager;
  private final SensorManager sensorManager;
  private final List<CompassListener> compassListeners = new ArrayList<>();

  // Not all devices have a compassSensor
  @Nullable
  private Sensor compassSensor;
  @Nullable
  private Sensor gravitySensor;
  @Nullable
  private Sensor magneticFieldSensor;

  private float[] truncatedRotationVectorValue = new float[4];
  private float[] rotationMatrix = new float[9];
  private float[] rotationVectorValue;
  private float lastHeading;
  private int lastAccuracySensorStatus;

  private long compassUpdateNextTimestamp;
  private float[] gravityValues = new float[3];
  private float[] magneticValues = new float[3];

  /**
   * Construct a new instance of the this class. A internal compass listeners needed to separate it
   * from the cleared list of public listeners.
   */
  LocationComponentCompassEngine(WindowManager windowManager, SensorManager sensorManager) {
    this.windowManager = windowManager;
    this.sensorManager = sensorManager;
    compassSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);
    if (compassSensor == null) {
      if (isGyroscopeAvailable()) {
        Timber.d("Rotation vector sensor not supported on device, falling back to orientation.");
        compassSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ORIENTATION);
      } else {
        Timber.d("Rotation vector sensor not supported on device, falling back to accelerometer and magnetic field.");
        gravitySensor = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        magneticFieldSensor = sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
      }
    }
  }

  @Override
  public void addCompassListener(@NonNull CompassListener compassListener) {
    if (compassListeners.isEmpty()) {
      onStart();
    }
    compassListeners.add(compassListener);
  }

  @Override
  public void removeCompassListener(@NonNull CompassListener compassListener) {
    compassListeners.remove(compassListener);
    if (compassListeners.isEmpty()) {
      onStop();
    }
  }

  @Override
  public int getLastAccuracySensorStatus() {
    return lastAccuracySensorStatus;
  }

  @Override
  public float getLastHeading() {
    return lastHeading;
  }

  @Override
  public void onStart() {
    registerSensorListeners();
  }

  @Override
  public void onStop() {
    unregisterSensorListeners();
  }

  @Override
  public void onSensorChanged(SensorEvent event) {
    // check when the last time the compass was updated, return if too soon.
    long currentTime = SystemClock.elapsedRealtime();
    if (currentTime < compassUpdateNextTimestamp) {
      return;
    }
    if (lastAccuracySensorStatus == SensorManager.SENSOR_STATUS_UNRELIABLE) {
      Timber.d("Compass sensor is unreliable, device calibration is needed.");
      return;
    }
    if (event.sensor.getType() == Sensor.TYPE_ROTATION_VECTOR) {
      rotationVectorValue = getRotationVectorFromSensorEvent(event);
      updateOrientation();

      // Update the compassUpdateNextTimestamp
      compassUpdateNextTimestamp = currentTime + LocationComponentConstants.COMPASS_UPDATE_RATE_MS;
    } else if (event.sensor.getType() == Sensor.TYPE_ORIENTATION) {
      notifyCompassChangeListeners((event.values[0] + 360) % 360);
    } else if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
      gravityValues = lowPassFilter(getRotationVectorFromSensorEvent(event), gravityValues);
      updateOrientation();
    } else if (event.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD) {
      magneticValues = lowPassFilter(getRotationVectorFromSensorEvent(event), magneticValues);
      updateOrientation();
    }
  }

  @Override
  public void onAccuracyChanged(Sensor sensor, int accuracy) {
    if (lastAccuracySensorStatus != accuracy) {
      for (CompassListener compassListener : compassListeners) {
        compassListener.onCompassAccuracyChange(accuracy);
      }
      lastAccuracySensorStatus = accuracy;
    }
  }

  private boolean isGyroscopeAvailable() {
    return sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE) != null;
  }

  @SuppressWarnings("SuspiciousNameCombination")
  private void updateOrientation() {
    if (rotationVectorValue != null) {
      SensorManager.getRotationMatrixFromVector(rotationMatrix, rotationVectorValue);
    } else {
      // Get rotation matrix given the gravity and geomagnetic matrices
      SensorManager.getRotationMatrix(rotationMatrix, null, gravityValues, magneticValues);
    }

    final int worldAxisForDeviceAxisX;
    final int worldAxisForDeviceAxisY;

    // Remap the axes as if the device screen was the instrument panel,
    // and adjust the rotation matrix for the device orientation.
    switch (windowManager.getDefaultDisplay().getRotation()) {
      case Surface.ROTATION_90:
        worldAxisForDeviceAxisX = SensorManager.AXIS_Z;
        worldAxisForDeviceAxisY = SensorManager.AXIS_MINUS_X;
        break;
      case Surface.ROTATION_180:
        worldAxisForDeviceAxisX = SensorManager.AXIS_MINUS_X;
        worldAxisForDeviceAxisY = SensorManager.AXIS_MINUS_Z;
        break;
      case Surface.ROTATION_270:
        worldAxisForDeviceAxisX = SensorManager.AXIS_MINUS_Z;
        worldAxisForDeviceAxisY = SensorManager.AXIS_X;
        break;
      case Surface.ROTATION_0:
      default:
        worldAxisForDeviceAxisX = SensorManager.AXIS_X;
        worldAxisForDeviceAxisY = SensorManager.AXIS_Z;
        break;
    }

    float[] adjustedRotationMatrix = new float[9];
    SensorManager.remapCoordinateSystem(rotationMatrix, worldAxisForDeviceAxisX,
      worldAxisForDeviceAxisY, adjustedRotationMatrix);

    // Transform rotation matrix into azimuth/pitch/roll
    float[] orientation = new float[3];
    SensorManager.getOrientation(adjustedRotationMatrix, orientation);

    // The x-axis is all we care about here.
    notifyCompassChangeListeners((float) Math.toDegrees(orientation[0]));
  }

  private void notifyCompassChangeListeners(float heading) {
    for (CompassListener compassListener : compassListeners) {
      compassListener.onCompassChanged(heading);
    }
    lastHeading = heading;
  }

  private void registerSensorListeners() {
    if (isCompassSensorAvailable()) {
      // Does nothing if the sensors already registered.
      sensorManager.registerListener(this, compassSensor, SENSOR_DELAY_MICROS);
    } else {
      sensorManager.registerListener(this, gravitySensor, SENSOR_DELAY_MICROS);
      sensorManager.registerListener(this, magneticFieldSensor, SENSOR_DELAY_MICROS);
    }
  }

  private void unregisterSensorListeners() {
    if (isCompassSensorAvailable()) {
      sensorManager.unregisterListener(this, compassSensor);
    } else {
      sensorManager.unregisterListener(this, gravitySensor);
      sensorManager.unregisterListener(this, magneticFieldSensor);
    }
  }

  private boolean isCompassSensorAvailable() {
    return compassSensor != null;
  }

  /**
   * Helper function, that filters newValues, considering previous values
   *
   * @param newValues      array of float, that contains new data
   * @param smoothedValues array of float, that contains previous state
   * @return float filtered array of float
   */
  private float[] lowPassFilter(float[] newValues, float[] smoothedValues) {
    if (smoothedValues == null) {
      return newValues;
    }
    for (int i = 0; i < newValues.length; i++) {
      smoothedValues[i] = smoothedValues[i] + ALPHA * (newValues[i] - smoothedValues[i]);
    }
    return smoothedValues;
  }

  /**
   * Pulls out the rotation vector from a SensorEvent, with a maximum length
   * vector of four elements to avoid potential compatibility issues.
   *
   * @param event the sensor event
   * @return the events rotation vector, potentially truncated
   */
  @NonNull
  private float[] getRotationVectorFromSensorEvent(@NonNull SensorEvent event) {
    if (event.values.length > 4) {
      // On some Samsung devices SensorManager.getRotationMatrixFromVector
      // appears to throw an exception if rotation vector has length > 4.
      // For the purposes of this class the first 4 values of the
      // rotation vector are sufficient (see crbug.com/335298 for details).
      // Only affects Android 4.3
      System.arraycopy(event.values, 0, truncatedRotationVectorValue, 0, 4);
      return truncatedRotationVectorValue;
    } else {
      return event.values;
    }
  }
}