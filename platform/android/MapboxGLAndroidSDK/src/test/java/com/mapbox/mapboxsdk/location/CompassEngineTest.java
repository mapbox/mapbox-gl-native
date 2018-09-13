package com.mapbox.mapboxsdk.location;

import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.view.WindowManager;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.junit.MockitoJUnitRunner;

import static junit.framework.Assert.assertEquals;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@RunWith(MockitoJUnitRunner.class)
public class CompassEngineTest {

  private LocationComponentCompassEngine compassEngine;

  @Mock
  private WindowManager windowManager;

  @Mock
  private SensorManager sensorManager;

  @Before
  public void setUp() throws Exception {
    compassEngine = new LocationComponentCompassEngine(windowManager, sensorManager);
  }

  @Test
  public void lastKnownCompassBearingAccuracyDefault() {
    assertEquals("Last accuracy should match", compassEngine.getLastAccuracySensorStatus(), 0);
  }

  @Test
  public void lastKnownCompassAccuracyStatusValue() {
    Sensor sensor = mock(Sensor.class);
    compassEngine.onAccuracyChanged(sensor, 2);
    assertEquals("Last accuracy should match", compassEngine.getLastAccuracySensorStatus(), 2);
  }

  @Test
  public void whenGyroscopeIsNull_fallbackToGravity() {
    SensorManager sensorManager = mock(SensorManager.class);
    when(sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE)).thenReturn(null);
    new LocationComponentCompassEngine(windowManager, sensorManager);

    verify(sensorManager, times(1)).getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
  }

  @Test
  public void whenGyroscopeIsNull_fallbackToMagneticField() {
    SensorManager sensorManager = mock(SensorManager.class);
    when(sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE)).thenReturn(null);
    new LocationComponentCompassEngine(windowManager, sensorManager);

    verify(sensorManager, times(1)).getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
  }
}
