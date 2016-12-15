package com.mapbox.mapboxsdk.geometry;

import android.os.Parcelable;

import com.mapbox.mapboxsdk.utils.MockParcel;

import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

public class VisibleRegionTest {

  private static final LatLng FAR_LEFT = new LatLng(52, -12);
  private static final LatLng NEAR_LEFT = new LatLng(34, -12);
  private static final LatLng FAR_RIGHT = new LatLng(52, 26);
  private static final LatLng NEAR_RIGHT = new LatLng(34, 26);
  private static final LatLngBounds BOUNDS =
    new LatLngBounds.Builder().include(FAR_LEFT).include(FAR_RIGHT).include(NEAR_LEFT).include(NEAR_RIGHT).build();

  @Test
  public void testSanity() {
    VisibleRegion region = new VisibleRegion(FAR_LEFT, FAR_RIGHT, NEAR_LEFT, NEAR_RIGHT, BOUNDS);
    assertNotNull("region should not be null", region);
  }

  @Test
  public void testEquality() {
    VisibleRegion region = new VisibleRegion(FAR_LEFT, FAR_RIGHT, NEAR_LEFT, NEAR_RIGHT, BOUNDS);
    assertEquals("visibleRegion is not equal to a LatLng", region.equals(FAR_LEFT), false);
    assertEquals("visibleRegion is equal to itself", region.equals(region), true);
  }

  @Test
  public void testFarLeftConstructor() {
    VisibleRegion region = new VisibleRegion(FAR_LEFT, FAR_RIGHT, NEAR_LEFT, NEAR_RIGHT, BOUNDS);
    assertEquals("LatLng should match", region.farLeft, FAR_LEFT);
  }

  @Test
  public void testNearLeftConstructor() {
    VisibleRegion region = new VisibleRegion(FAR_LEFT, FAR_RIGHT, NEAR_LEFT, NEAR_RIGHT, BOUNDS);
    assertEquals("LatLng should match", region.nearLeft, NEAR_LEFT);
  }

  @Test
  public void testFarRightConstructor() {
    VisibleRegion region = new VisibleRegion(FAR_LEFT, FAR_RIGHT, NEAR_LEFT, NEAR_RIGHT, BOUNDS);
    assertEquals("LatLng should match", region.farRight, FAR_RIGHT);
  }

  @Test
  public void testNearRightConstructor() {
    VisibleRegion region = new VisibleRegion(FAR_LEFT, FAR_RIGHT, NEAR_LEFT, NEAR_RIGHT, BOUNDS);
    assertEquals("LatLng should match", region.nearRight, NEAR_RIGHT);
  }

  @Test
  public void testLatLngBoundsConstructor() {
    VisibleRegion region = new VisibleRegion(FAR_LEFT, FAR_RIGHT, NEAR_LEFT, NEAR_RIGHT, BOUNDS);
    assertEquals("LatLngBounds should match", region.latLngBounds, BOUNDS);
  }

  @Test
  public void testEquals() {
    VisibleRegion regionLeft = new VisibleRegion(FAR_LEFT, FAR_RIGHT, NEAR_LEFT, NEAR_RIGHT, BOUNDS);
    VisibleRegion regionRight = new VisibleRegion(FAR_LEFT, FAR_RIGHT, NEAR_LEFT, NEAR_RIGHT, BOUNDS);
    assertEquals("VisibleRegions should match", regionLeft, regionRight);
  }

  @Test
  public void testHashcode() {
    VisibleRegion region = new VisibleRegion(FAR_LEFT, FAR_RIGHT, NEAR_LEFT, NEAR_RIGHT, BOUNDS);
    assertEquals("hashcode should match", -923534102, region.hashCode());
  }

  @Test
  public void testToString() {
    VisibleRegion region = new VisibleRegion(FAR_LEFT, FAR_RIGHT, NEAR_LEFT, NEAR_RIGHT, BOUNDS);
    assertEquals("string should match",
      "[farLeft [LatLng [latitude=52.0, longitude=-12.0, altitude=0.0]], "
        + "farRight [LatLng [latitude=52.0, longitude=26.0, altitude=0.0]], "
        + "nearLeft [LatLng [latitude=34.0, longitude=-12.0, altitude=0.0]], "
        + "nearRight [LatLng [latitude=34.0, longitude=26.0, altitude=0.0]], "
        + "latLngBounds [N:52.0; E:26.0; S:34.0; W:-12.0]]",
      region.toString());
  }

  @Test
  public void testParcelable() {
    VisibleRegion region = new VisibleRegion(FAR_LEFT, FAR_RIGHT, NEAR_LEFT, NEAR_RIGHT, BOUNDS);
    Parcelable parcel = MockParcel.obtain(region);
    assertEquals("parcel should match initial object", region, parcel);
  }
}
