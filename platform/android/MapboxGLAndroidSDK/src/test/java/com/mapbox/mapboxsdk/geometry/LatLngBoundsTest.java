package com.mapbox.mapboxsdk.geometry;

import android.os.Parcelable;

import com.mapbox.mapboxsdk.constants.GeometryConstants;
import com.mapbox.mapboxsdk.exceptions.InvalidLatLngBoundsException;
import com.mapbox.mapboxsdk.utils.MockParcel;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExpectedException;

import java.util.ArrayList;
import java.util.List;

import static junit.framework.Assert.assertNotNull;
import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
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
  public void noLatLngs() {
    new LatLngBounds.Builder().build();
  }

  @Test(expected = InvalidLatLngBoundsException.class)
  public void oneLatLngs() {
    new LatLngBounds.Builder().include(LAT_LNG_NULL_ISLAND).build();
  }

  @Test
  public void latitiudeSpan() {
    assertEquals("Span should be the same", 2, latLngBounds.getLatitudeSpan(), DELTA);
  }

  @Test
  public void longitudeSpan() {
    assertEquals("Span should be the same", 2, latLngBounds.getLongitudeSpan(), DELTA);
  }

  @Test
  public void coordinateSpan() {
    LatLngSpan latLngSpan = latLngBounds.getSpan();
    assertEquals("LatLngSpan should be the same", new LatLngSpan(2, 2), latLngSpan);
  }

  @Test
  public void center() {
    LatLng center = latLngBounds.getCenter();
    assertEquals("Center should match", new LatLng(1, 1), center);
  }

  @Test
  public void notEmptySpan() {
    latLngBounds = new LatLngBounds.Builder()
      .include(LAT_LNG_NOT_NULL_ISLAND)
      .include(LAT_LNG_NULL_ISLAND)
      .build();
    assertFalse("Should not be empty", latLngBounds.isEmptySpan());
  }

  @Test
  public void toLatLngs() {
    latLngBounds = new LatLngBounds.Builder()
      .include(LAT_LNG_NOT_NULL_ISLAND)
      .include(LAT_LNG_NULL_ISLAND)
      .build();

    assertArrayEquals("LatLngs should match",
      new LatLng[] {LAT_LNG_NOT_NULL_ISLAND, LAT_LNG_NULL_ISLAND},
      latLngBounds.toLatLngs());
  }

  @Test
  public void include() {
    assertTrue("LatLng should be included", latLngBounds.contains(new LatLng(1, 1)));
  }

  @Test
  public void includes() {
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
  public void containsNot() {
    assertFalse("LatLng should not be included", latLngBounds.contains(new LatLng(3, 1)));
  }

  @Test
  public void containsBoundsInWorld() {
    assertTrue("LatLngBounds should be contained in the world", LatLngBounds.world().contains(latLngBounds));
  }

  @Test
  public void containsBounds() {
    LatLngBounds inner = new LatLngBounds.Builder()
      .include(new LatLng(-5, -5))
      .include(new LatLng(5, 5))
      .build();
    LatLngBounds outer = new LatLngBounds.Builder()
      .include(new LatLng(-10, -10))
      .include(new LatLng(10, 10))
      .build();
    assertTrue(outer.contains(inner));
    assertFalse(inner.contains(outer));
  }

  @Test
  public void testHashCode() {
    assertEquals(2147483647, latLngBounds.hashCode(), -1946419200);
  }

  @Test
  public void equality() {
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
  public void intersect() {
    LatLngBounds latLngBounds = new LatLngBounds.Builder()
      .include(new LatLng(1, 1))
      .include(LAT_LNG_NULL_ISLAND)
      .build();
    assertEquals("intersect should match", latLngBounds, latLngBounds.intersect(this.latLngBounds.getLatNorth(),
      this.latLngBounds.getLonEast(), this.latLngBounds.getLatSouth(), this.latLngBounds.getLonWest()));
  }

  @Test
  public void intersectNot() {
    LatLngBounds latLngBounds = new LatLngBounds.Builder()
      .include(new LatLng(10, 10))
      .include(new LatLng(9, 8))
      .build();
    assertNull(latLngBounds.intersect(this.latLngBounds));
  }

  @Test
  public void innerUnion() {
    LatLngBounds latLngBounds = new LatLngBounds.Builder()
      .include(new LatLng(1, 1))
      .include(LAT_LNG_NULL_ISLAND)
      .build();
    assertEquals("union should match", latLngBounds, latLngBounds.intersect(this.latLngBounds));
  }

  @Test
  public void outerUnion() {
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
  public void northWest() {
    double minLat = 5;
    double minLon = 6;
    double maxLat = 20;
    double maxLon = 21;

    LatLngBounds latLngBounds = new LatLngBounds.Builder()
      .include(new LatLng(minLat, minLon))
      .include(new LatLng(maxLat, maxLon))
      .build();

    assertEquals("NorthWest should match", latLngBounds.getNorthWest(), new LatLng(maxLat, minLon));
  }

  @Test
  public void southWest() {
    double minLat = 5;
    double minLon = 6;
    double maxLat = 20;
    double maxLon = 21;

    LatLngBounds latLngBounds = new LatLngBounds.Builder()
      .include(new LatLng(minLat, minLon))
      .include(new LatLng(maxLat, maxLon))
      .build();

    assertEquals("SouthWest should match", latLngBounds.getSouthWest(), new LatLng(minLat, minLon));
  }

  @Test
  public void northEast() {
    double minLat = 5;
    double minLon = 6;
    double maxLat = 20;
    double maxLon = 21;

    LatLngBounds latLngBounds = new LatLngBounds.Builder()
      .include(new LatLng(minLat, minLon))
      .include(new LatLng(maxLat, maxLon))
      .build();

    assertEquals("NorthEast should match", latLngBounds.getNorthEast(), new LatLng(maxLat, maxLon));
  }

  @Test
  public void southEast() {
    double minLat = 5;
    double minLon = 6;
    double maxLat = 20;
    double maxLon = 21;

    LatLngBounds latLngBounds = new LatLngBounds.Builder()
      .include(new LatLng(minLat, minLon))
      .include(new LatLng(maxLat, maxLon))
      .build();

    assertEquals("SouthEast should match", latLngBounds.getSouthEast(), new LatLng(minLat, maxLon));
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

  @Test
  public void fromTileID() {
    LatLngBounds bounds = LatLngBounds.from(0, 0, 0);
    assertEquals(GeometryConstants.MIN_LONGITUDE, bounds.getLonWest(), DELTA);
    assertEquals(GeometryConstants.MIN_MERCATOR_LATITUDE, bounds.getLatSouth(), DELTA);
    assertEquals(GeometryConstants.MAX_LONGITUDE, bounds.getLonEast(), DELTA);
    assertEquals(GeometryConstants.MAX_MERCATOR_LATITUDE, bounds.getLatNorth(), DELTA);

    bounds = LatLngBounds.from(10, 288, 385);
    assertEquals(-78.75, bounds.getLonWest(), DELTA);
    assertEquals(40.446947059600497, bounds.getLatSouth(), DELTA);
    assertEquals(-78.3984375, bounds.getLonEast(), DELTA);
    assertEquals(40.713955826286039, bounds.getLatNorth(), DELTA);

  }

  @Rule
  public final ExpectedException exception = ExpectedException.none();

  @Test
  public void testConstructorChecksNorthLatitudeNaN() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("latitude must not be NaN");
    LatLngBounds.from(Double.NaN, 0, -20, -20);
  }

  @Test
  public void testConstructorChecksEastLongitudeNaN() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("longitude must not be NaN");
    LatLngBounds.from(0, Double.NaN, -20, -20);
  }

  @Test
  public void testConstructorChecksNorthLatitudeGreaterThan90() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("latitude must be between -90 and 90");
    LatLngBounds.from(95, 0, -20, -20);
  }

  @Test
  public void testConstructorChecksNorthLatitudeLessThanThanNegative90() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("latitude must be between -90 and 90");
    LatLngBounds.from(-95, 0, -20, -20);
  }

  @Test
  public void testConstructorChecksEastLongitudeInfinity() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("longitude must not be infinite");
    LatLngBounds.from(0, Double.POSITIVE_INFINITY, -20, -20);
  }


  @Test
  public void testConstructorChecksSouthLatitudeNaN() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("latitude must not be NaN");
    LatLngBounds.from(20, 20, Double.NaN, 0);
  }

  @Test
  public void testConstructorChecksWesttLongitudeNaN() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("longitude must not be NaN");
    LatLngBounds.from(20, 20, 0, Double.NaN);
  }

  @Test
  public void testConstructorChecksSouthLatitudeGreaterThan90() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("latitude must be between -90 and 90");
    LatLngBounds.from(20, 20,95, 0);
  }

  @Test
  public void testConstructorChecksSouthLatitudeLessThanThanNegative90() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("latitude must be between -90 and 90");
    LatLngBounds.from(20, 20, -95, 0);
  }

  @Test
  public void testConstructorChecksWestLongitudeInfinity() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("longitude must not be infinite");
    LatLngBounds.from(20, 20, 0, Double.POSITIVE_INFINITY);
  }

  @Test
  public void testConstructorCheckLatSouthGreaterLatNorth() {
    exception.expect(IllegalArgumentException.class);
    exception.expectMessage("LatSouth cannot be less than latNorth");
    LatLngBounds.from(0, 20, 20, 0);
  }
}
