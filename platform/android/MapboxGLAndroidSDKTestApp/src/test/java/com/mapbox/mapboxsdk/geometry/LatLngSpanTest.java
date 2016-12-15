package com.mapbox.mapboxsdk.geometry;

import android.os.Parcelable;

import com.mapbox.mapboxsdk.utils.MockParcel;

import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

public class LatLngSpanTest {

  private static final double DELTA = 1e-15;
  private static final LatLng LAT_LNG_NULL_ISLAND = new LatLng(0, 0);

  @Test
  public void testSanity() {
    LatLngSpan latLngSpan = new LatLngSpan(0.0, 0.0);
    assertNotNull("latLngSpan should not be null", latLngSpan);
  }

  @Test
  public void testEquality() {
    LatLngSpan latLngSpan = new LatLngSpan(0.0, 0.0);
    assertEquals("latLngSpan is not equal to a LatLng", latLngSpan.equals(LAT_LNG_NULL_ISLAND), false);
  }

  @Test
  public void testLatitudeConstructor() {
    double latitude = 1.23;
    LatLngSpan latLngSpan = new LatLngSpan(latitude, 0.0);
    assertEquals("latitude in constructor", latLngSpan.getLatitudeSpan(), latitude, DELTA);
  }

  @Test
  public void testLongitudeConstructor() {
    double longitude = 1.23;
    LatLngSpan latLngSpan = new LatLngSpan(0.0, longitude);
    assertEquals("latitude in constructor", latLngSpan.getLongitudeSpan(), longitude, DELTA);
  }

  @Test
  public void testLatitudeMethod() {
    double latitude = 1.23;
    LatLngSpan latLngSpan = new LatLngSpan(0.0, 0.0);
    latLngSpan.setLatitudeSpan(latitude);
    assertEquals("latitude in constructor", latLngSpan.getLatitudeSpan(), latitude, DELTA);
  }

  @Test
  public void testLongitudeMethod() {
    double longitude = 1.23;
    LatLngSpan latLngSpan = new LatLngSpan(0.0, 0.0);
    latLngSpan.setLongitudeSpan(longitude);
    assertEquals("latitude in constructor", latLngSpan.getLongitudeSpan(), longitude, DELTA);
  }

  @Test
  public void testParcelable() {
    LatLngSpan object = new LatLngSpan(1, 2);
    Parcelable parcel = MockParcel.obtain(object);
    assertEquals("parcel should match initial object", object, parcel);
  }
}
