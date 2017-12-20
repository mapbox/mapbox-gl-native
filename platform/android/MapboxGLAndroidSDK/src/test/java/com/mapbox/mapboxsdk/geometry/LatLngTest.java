package com.mapbox.mapboxsdk.geometry;

import android.location.Location;
import android.os.Parcelable;

import com.mapbox.mapboxsdk.utils.MockParcel;

import org.junit.Test;
import org.junit.Rule;
import org.junit.rules.ExpectedException;

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

  @Rule
  public final ExpectedException exception = ExpectedException.none();

  @Test
  public void testConstructorChecksLatitudeNaN() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("latitude must not be NaN");
    new LatLng(Double.NaN, 0);
  }

  @Test
  public void testConstructorChecksLongitudeNaN() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("longitude must not be NaN");
    new LatLng(0, Double.NaN);
  }

  @Test
  public void testConstructorChecksLatitudeGreaterThan90() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("latitude must be between -90 and 90");
    new LatLng(95, 0);
  }

  @Test
  public void testConstructorChecksLatitudeLessThanThanNegative90() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("latitude must be between -90 and 90");
    new LatLng(-95, 0);
  }

  @Test
  public void testConstructorChecksLongitudeInfinity() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("longitude must not be infinite");
    new LatLng(0, Double.POSITIVE_INFINITY);
  }

  @Test
  public void testLatitudeSetterChecksNaN() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("latitude must not be NaN");
    new LatLng().setLatitude(Double.NaN);
  }

  @Test
  public void testLongitudeSetterChecksNaN() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("longitude must not be NaN");
    new LatLng().setLongitude(Double.NaN);
  }

  @Test
  public void testLatitudeSetterChecksGreaterThan90() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("latitude must be between -90 and 90");
    new LatLng().setLatitude(95);
  }

  @Test
  public void testLatitudeSetterChecksLessThanThanNegative90() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("latitude must be between -90 and 90");
    new LatLng().setLatitude(-95);
  }

  @Test
  public void testLongitudeSetterChecksInfinity() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("longitude must not be infinite");
    new LatLng().setLongitude(Double.NEGATIVE_INFINITY);
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
    LatLng originalLatLng = new LatLng(45.0, -185.0);
    LatLng newLatlng = originalLatLng.wrap();
    assertNotSame(" new wrapped LatLng is created", originalLatLng, newLatlng);
    assertEquals("longitude wrapped value", originalLatLng.getLongitude(), -185.0, DELTA);
    assertEquals("longitude wrapped value", newLatlng.getLongitude(), 175.0, DELTA);

    newLatlng = new LatLng(45.0, 180.0).wrap();
    assertEquals("longitude wrapped max value", newLatlng.getLongitude(), 180.0, DELTA);

    newLatlng = new LatLng(45.0, -180.0).wrap();
    assertEquals("longitude wrapped min value", newLatlng.getLongitude(), -180.0, DELTA);
  }

  @Test
  public void testUnnecessaryWrapped() {
    LatLng latLng = new LatLng(45.0, 50.0).wrap();
    assertEquals("longitude wrapped value", latLng.getLongitude(), 50.0, DELTA);
  }
}
