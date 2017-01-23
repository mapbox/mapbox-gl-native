package com.mapbox.mapboxsdk.geometry;

import android.location.Location;
import android.os.Parcelable;

import com.mapbox.mapboxsdk.utils.MockParcel;

import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNotSame;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class LatLngTest {

  private static final double DELTA = 1e-15;

  @Test
  public void testSanity() {
    LatLng latLng = new LatLng(0.0, 0.0);
    assertNotNull("latLng  should not be null", latLng);
  }

  @Test
  public void testLatitudeEmptyConstructor() {
    LatLng latLng = new LatLng();
    assertEquals("latitude default value", latLng.getLatitude(), 0, DELTA);
  }

  @Test
  public void testLongitudeEmptyConstructor() {
    LatLng latLng = new LatLng();
    assertEquals("longitude default value", latLng.getLongitude(), 0, DELTA);
  }

  @Test
  public void testAltitudeEmptyConstructor() {
    LatLng latLng1 = new LatLng();
    assertEquals("altitude default value", latLng1.getAltitude(), 0.0, DELTA);
  }

  @Test
  public void testLatitudeConstructor() {
    double latitude = 1.2;
    LatLng latLng = new LatLng(latitude, 3.4);
    assertEquals("latitude should match", latLng.getLatitude(), latitude, DELTA);
  }

  @Test
  public void testLongitudeConstructor() {
    double longitude = 3.4;
    LatLng latLng = new LatLng(1.2, longitude);
    assertEquals("longitude should match", latLng.getLongitude(), longitude, DELTA);
  }

  @Test
  public void testAltitudeConstructor() {
    LatLng latLng1 = new LatLng(1.2, 3.4);
    assertEquals("altitude default value", latLng1.getAltitude(), 0.0, DELTA);

    double altitude = 5.6;
    LatLng latLng2 = new LatLng(1.2, 3.4, altitude);
    assertEquals("altitude default value", latLng2.getAltitude(), altitude, DELTA);
  }

  @Test
  public void testLatitudeSetter() {
    LatLng latLng = new LatLng(1.2, 3.4);
    latLng.setLatitude(3);
    assertEquals("latitude should match", 3, latLng.getLatitude(), DELTA);
  }

  @Test
  public void testLongitudeSetter() {
    LatLng latLng = new LatLng(1.2, 3.4);
    latLng.setLongitude(3);
    assertEquals("longitude should match", 3, latLng.getLongitude(), DELTA);
  }

  @Test
  public void testAltitudeSetter() {
    LatLng latLng = new LatLng(1.2, 3.4);
    latLng.setAltitude(3);
    assertEquals("altitude should match", 3, latLng.getAltitude(), DELTA);
  }

  @Test
  public void testLatLngConstructor() {
    LatLng latLng1 = new LatLng(1.2, 3.4);
    LatLng latLng2 = new LatLng(latLng1);
    assertEquals("latLng should match", latLng1, latLng2);
  }

  @Test
  public void testDistanceTo() {
    LatLng latLng1 = new LatLng(0.0, 0.0);
    LatLng latLng2 = new LatLng(1.0, 1.0);
    assertEquals("distances should match",
      latLng1.distanceTo(latLng2),
      157425.53710839353, DELTA);
  }

  @Test
  public void testDistanceToSamePoint() {
    LatLng latLng1 = new LatLng(40.71199035644531, -74.0081);
    LatLng latLng2 = new LatLng(40.71199035644531, -74.0081);
    double distance = latLng1.distanceTo(latLng2);
    assertEquals("distance should match", 0.0, distance, DELTA);
  }

  @Test
  public void testLocationProvider() {
    double latitude = 1.2;
    double longitude = 3.4;
    double altitude = 5.6;

    // Mock the location class
    Location locationMocked = mock(Location.class);
    when(locationMocked.getLatitude()).thenReturn(latitude);
    when(locationMocked.getLongitude()).thenReturn(longitude);
    when(locationMocked.getAltitude()).thenReturn(altitude);

    // Test the constructor
    LatLng latLng = new LatLng(locationMocked);
    assertEquals("latitude should match", latLng.getLatitude(), latitude, DELTA);
    assertEquals("longitude should match", latLng.getLongitude(), longitude, DELTA);
    assertEquals("altitude should match", latLng.getAltitude(), altitude, DELTA);
  }

  @Test
  public void testHashCode() {
    double latitude = 1.2;
    double longitude = 3.4;
    double altitude = 5.6;
    LatLng latLng = new LatLng(latitude, longitude, altitude);
    assertEquals("hash code should match", latLng.hashCode(), -151519232);
  }

  @Test
  public void testToString() {
    double latitude = 1.2;
    double longitude = 3.4;
    double altitude = 5.6;
    LatLng latLng = new LatLng(latitude, longitude, altitude);
    assertEquals("string should match",
      latLng.toString(),
      "LatLng [latitude=1.2, longitude=3.4, altitude=5.6]");
  }

  @Test
  public void testEqualsOther() {
    double latitude = 1.2;
    double longitude = 3.4;
    double altitude = 5.6;
    LatLng latLng1 = new LatLng(latitude, longitude, altitude);
    LatLng latLng2 = new LatLng(latitude, longitude, altitude);
    assertEquals("LatLng should match", latLng1, latLng2);
  }

  @Test
  public void testEqualsItself() {
    LatLng latLng = new LatLng(1, 2, 3);
    assertEquals("LatLng should match", latLng, latLng);
  }

  @Test
  public void testNotEquals() {
    LatLng latLng = new LatLng(1, 2);
    assertNotEquals("LatLng should match", latLng, new Object());
  }

  @Test
  public void testParcelable() {
    LatLng latLng = new LatLng(45.0, -185.0);
    Parcelable parcel = MockParcel.obtain(latLng);
    assertEquals("parcel should match initial object", latLng, parcel);
  }

  @Test
  public void testWrapped() {
    LatLng latLng = new LatLng(45.0, -185.0);
    LatLng wrapped = latLng.wrap();
    assertNotSame(latLng, wrapped);
    assertEquals("longitude wrapped value", wrapped.getLongitude(), 175.0, DELTA);
  }

  @Test
  public void testUnnecessaryWrapped() {
    LatLng latLng = new LatLng(45.0, 50.0);
    LatLng wrapped = latLng.wrap();
    assertNotSame(latLng, wrapped);
    assertEquals("longitude wrapped value", wrapped.getLongitude(), 50.0, DELTA);
  }
}
