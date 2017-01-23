package com.mapbox.mapboxsdk.geometry;

import android.os.Parcelable;

import com.mapbox.mapboxsdk.exceptions.InvalidLatLngBoundsException;
import com.mapbox.mapboxsdk.utils.MockParcel;

import org.junit.Before;
import org.junit.Test;

import java.util.ArrayList;
import java.util.List;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

public class LatLngBoundsTest {

  private static final double DELTA = 1e-15;

  private LatLngBounds latLngBounds;
  private static final LatLng LAT_LNG_NULL_ISLAND = new LatLng(0, 0);
  private static final LatLng LAT_LNG_NOT_NULL_ISLAND = new LatLng(2, 2);

  @Before
  public void beforeTest() {
    latLngBounds = new LatLngBounds.Builder()
      .include(LAT_LNG_NULL_ISLAND)
      .include(LAT_LNG_NOT_NULL_ISLAND)
      .build();
  }

  @Test
  public void testSanity() {
    LatLngBounds.Builder latLngBoundsBuilder = new LatLngBounds.Builder();
    latLngBoundsBuilder.include(LAT_LNG_NULL_ISLAND).include(LAT_LNG_NOT_NULL_ISLAND);
    assertNotNull("latLng  should not be null", latLngBoundsBuilder.build());
  }

  @Test(expected = InvalidLatLngBoundsException.class)
  public void testNoLatLngs() {
    new LatLngBounds.Builder().build();
  }

  @Test(expected = InvalidLatLngBoundsException.class)
  public void testOneLatLngs() {
    new LatLngBounds.Builder().include(LAT_LNG_NULL_ISLAND).build();
  }

  @Test
  public void testLatitiudeSpan() {
    assertEquals("Span should be the same", 2, latLngBounds.getLatitudeSpan(), DELTA);
  }

  @Test
  public void testLongitudeSpan() {
    assertEquals("Span should be the same", 2, latLngBounds.getLongitudeSpan(), DELTA);
  }

  @Test
  public void testCoordinateSpan() {
    LatLngSpan latLngSpan = latLngBounds.getSpan();
    assertEquals("LatLngSpan should be the same", new LatLngSpan(2, 2), latLngSpan);
  }

  @Test
  public void testCenter() {
    LatLng center = latLngBounds.getCenter();
    assertEquals("Center should match", new LatLng(1, 1), center);
  }

  @Test
  public void testEmptySpan() {
    latLngBounds = new LatLngBounds.Builder()
      .include(LAT_LNG_NOT_NULL_ISLAND)
      .include(LAT_LNG_NOT_NULL_ISLAND)
      .build();
    assertTrue("Should be empty", latLngBounds.isEmptySpan());
  }

  @Test
  public void testNotEmptySpan() {
    latLngBounds = new LatLngBounds.Builder()
      .include(LAT_LNG_NOT_NULL_ISLAND)
      .include(LAT_LNG_NULL_ISLAND)
      .build();
    assertFalse("Should not be empty", latLngBounds.isEmptySpan());
  }

  @Test
  public void testToLatLngs() {
    latLngBounds = new LatLngBounds.Builder()
      .include(LAT_LNG_NOT_NULL_ISLAND)
      .include(LAT_LNG_NULL_ISLAND)
      .build();

    assertArrayEquals("LatLngs should match",
      new LatLng[] {LAT_LNG_NOT_NULL_ISLAND, LAT_LNG_NULL_ISLAND},
      latLngBounds.toLatLngs());
  }

  @Test
  public void testIncluding() {
    assertTrue("LatLng should be included", latLngBounds.contains(new LatLng(1, 1)));
  }

  @Test
  public void testIncludes() {
    List<LatLng> points = new ArrayList<>();
    points.add(LAT_LNG_NULL_ISLAND);
    points.add(LAT_LNG_NOT_NULL_ISLAND);

    LatLngBounds latLngBounds1 = new LatLngBounds.Builder()
      .includes(points)
      .build();

    LatLngBounds latLngBounds2 = new LatLngBounds.Builder()
      .include(LAT_LNG_NULL_ISLAND)
      .include(LAT_LNG_NOT_NULL_ISLAND)
      .build();

    assertEquals("LatLngBounds should match", latLngBounds1, latLngBounds2);
  }

  @Test
  public void testNoIncluding() {
    assertFalse("LatLng should not be included", latLngBounds.contains(new LatLng(3, 1)));
  }

  @Test
  public void testHashCode() {
    assertEquals(2147483647, latLngBounds.hashCode(), -1946419200);
  }

  @Test
  public void testEquality() {
    LatLngBounds latLngBounds = new LatLngBounds.Builder()
      .include(LAT_LNG_NULL_ISLAND)
      .include(LAT_LNG_NOT_NULL_ISLAND)
      .build();
    assertEquals("equality should match", this.latLngBounds, latLngBounds);
    assertEquals("not equal to a different object type", this.latLngBounds.equals(LAT_LNG_NOT_NULL_ISLAND), false);
  }

  @Test
  public void testToString() {
    assertEquals(latLngBounds.toString(), "N:2.0; E:2.0; S:0.0; W:0.0");
  }

  @Test
  public void testIntersect() {
    LatLngBounds latLngBounds = new LatLngBounds.Builder()
      .include(new LatLng(1, 1))
      .include(LAT_LNG_NULL_ISLAND)
      .build();
    assertEquals("intersect should match", latLngBounds, latLngBounds.intersect(this.latLngBounds.getLatNorth(),
      this.latLngBounds.getLonEast(), this.latLngBounds.getLatSouth(), this.latLngBounds.getLonWest()));
  }

  @Test
  public void testNoIntersect() {
    LatLngBounds latLngBounds = new LatLngBounds.Builder()
      .include(new LatLng(10, 10))
      .include(new LatLng(9, 8))
      .build();
    assertNull(latLngBounds.intersect(this.latLngBounds));
  }

  @Test
  public void testInnerUnion() {
    LatLngBounds latLngBounds = new LatLngBounds.Builder()
      .include(new LatLng(1, 1))
      .include(LAT_LNG_NULL_ISLAND)
      .build();
    assertEquals("union should match", latLngBounds, latLngBounds.intersect(this.latLngBounds));
  }

  @Test
  public void testOuterUnion() {
    LatLngBounds latLngBounds = new LatLngBounds.Builder()
      .include(new LatLng(10, 10))
      .include(new LatLng(9, 8))
      .build();
    assertEquals("outer union should match",
      latLngBounds.union(this.latLngBounds),
      new LatLngBounds.Builder()
        .include(new LatLng(10, 10))
        .include(LAT_LNG_NULL_ISLAND)
        .build());
  }

  @Test
  public void testParcelable() {
    LatLngBounds latLngBounds = new LatLngBounds.Builder()
      .include(new LatLng(10, 10))
      .include(new LatLng(9, 8))
      .build();
    Parcelable parcel = MockParcel.obtain(latLngBounds);
    assertEquals("Parcel should match original object", parcel, latLngBounds);
  }
}