package com.mapbox.mapboxsdk.annotations;

import android.os.Parcelable;

import com.mapbox.mapboxsdk.exceptions.InvalidMarkerPositionException;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.utils.MockParcel;

import org.junit.Before;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

public class MarkerViewTest {

  @Mock
  MapboxMap mapboxMap;

  @Mock
  MarkerViewManager markerViewManager;

  @Before
  public void beforeTest() {
    MockitoAnnotations.initMocks(this);
  }

  @Test
  public void testSanity() {
    MarkerViewOptions markerOptions = new MarkerViewOptions();
    assertNotNull("markerOptions should not be null", markerOptions);
  }

  @Test
  public void testMarker() {
    MarkerViewOptions markerOptions = new MarkerViewOptions().position(new LatLng());
    assertNotNull("marker should not be null", markerOptions.getMarker());
  }

  @Test(expected = InvalidMarkerPositionException.class)
  public void testInvalidMarker() {
    new MarkerViewOptions().getMarker();
  }

  @Test
  public void testPosition() {
    MarkerViewOptions markerOptions = new MarkerViewOptions().position(new LatLng(10, 12));
    MarkerView marker = markerOptions.getMarker();
    assertEquals(marker.getPosition(), new LatLng(10, 12));
    assertEquals(markerOptions.getPosition(), new LatLng(10, 12));
  }

  @Test
  public void testSnippet() {
    MarkerViewOptions markerOptions = new MarkerViewOptions().snippet("Mapbox").position(new LatLng());
    MarkerView marker = markerOptions.getMarker();
    assertEquals(marker.getSnippet(), "Mapbox");
  }

  @Test
  public void testTitle() {
    MarkerViewOptions markerOptions = new MarkerViewOptions().title("Mapbox").position(new LatLng());
    MarkerView marker = markerOptions.getMarker();
    assertEquals(marker.getTitle(), "Mapbox");
    assertEquals(markerOptions.getTitle(), "Mapbox");
  }

  @Test
  public void testFlat() {
    MarkerViewOptions markerOptions = new MarkerViewOptions().flat(true).position(new LatLng());
    MarkerView marker = markerOptions.getMarker();
    assertTrue("flat should be true", marker.isFlat());
  }

  @Test
  public void testFlatDefault() {
    assertFalse("default value of flat should be false", new MarkerViewOptions().position(
      new LatLng()).getMarker().isFlat());
  }

  @Test
  public void testAnchor() {
    float anchorU = 1;
    float anchorV = 1;
    MarkerViewOptions markerOptions = new MarkerViewOptions().anchor(anchorU, anchorV).position(new LatLng());
    MarkerView marker = markerOptions.getMarker();
    assertEquals("anchorU should match ", anchorU, marker.getAnchorU(), 0);
    assertEquals("anchorU should match ", anchorV, marker.getAnchorV(), 0);
  }

  @Test
  public void testAnchorDefault() {
    MarkerView marker = new MarkerViewOptions().position(new LatLng()).getMarker();
    assertEquals("anchorU should match ", 0.5, marker.getAnchorU(), 0);
    assertEquals("anchorU should match ", 1, marker.getAnchorV(), 0);
  }

  @Test
  public void testInfoWindowAnchor() {
    float anchorU = 1;
    float anchorV = 1;
    MarkerViewOptions markerOptions = new MarkerViewOptions().position(new LatLng()).infoWindowAnchor(anchorU, anchorV);
    MarkerView marker = markerOptions.getMarker();
    assertEquals("anchorU should match ", 1, marker.getInfoWindowAnchorU(), 0);
    assertEquals("anchorU should match ", 1, marker.getInfoWindowAnchorV(), 0);
  }

  @Test
  public void testInfoWindowAnchorDefault() {
    MarkerView marker = new MarkerViewOptions().position(new LatLng()).getMarker();
    assertEquals("anchorU should match ", 0.5, marker.getInfoWindowAnchorU(), 0);
    assertEquals("anchorU should match ", 0, marker.getInfoWindowAnchorV(), 0);
  }

  @Test
  public void testRotation() {
    int rotation = 90;
    MarkerViewOptions markerOptions = new MarkerViewOptions().position(new LatLng()).rotation(rotation);
    MarkerView marker = markerOptions.getMarker();
    assertEquals("rotation should match ", rotation, marker.getRotation(), 0);
  }

  @Test
  public void testRotationAboveMax() {
    MarkerViewOptions markerOptions = new MarkerViewOptions().rotation(390).position(new LatLng());
    MarkerView marker = markerOptions.getMarker();
    assertEquals(marker.getRotation(), 30, 0);
  }

  @Test
  public void testRotationBelowMin() {
    MarkerViewOptions markerOptions = new MarkerViewOptions().rotation(-10).position(new LatLng());
    MarkerView marker = markerOptions.getMarker();
    assertEquals(marker.getRotation(), 350, 0);
  }

  @Test
  public void testVisible() {
    boolean visible = false;
    MarkerViewOptions markerOptions = new MarkerViewOptions().visible(visible).position(new LatLng());
    MarkerView marker = markerOptions.getMarker();
    assertEquals("visible should match ", visible, marker.isVisible());
  }

  @Test
  public void testVisibleDefault() {
    assertTrue(new MarkerViewOptions().position(new LatLng()).getMarker().isVisible());
  }

  @Test
  public void testBuilder() {
    MarkerView marker = new MarkerViewOptions().title("title").snippet("snippet").position(
      new LatLng(10, 12)).getMarker();
    assertEquals(marker.getSnippet(), "snippet");
    assertEquals(marker.getPosition(), new LatLng(10, 12));
  }

  @Test
  public void testHashCode() {
    MarkerView marker = new MarkerViewOptions().position(new LatLng()).getMarker();
    assertEquals("hash code should match", marker.hashCode(), 0);
  }

  @Test
  public void testHashCodeBuilder() {
    MarkerViewOptions markerOptions = new MarkerViewOptions().position(new LatLng(10, 12));
    assertEquals("hash code should match", markerOptions.hashCode(), 0);
  }

  @Test
  public void testEquals() {
    MarkerView markerOne = new MarkerViewOptions().position(new LatLng(0, 0)).getMarker();
    MarkerView markerTwo = new MarkerViewOptions().position(new LatLng(0, 0)).getMarker();
    assertEquals(markerOne, markerTwo);
  }

  @Test
  public void testEqualsItself() {
    MarkerViewOptions markerOptions = new MarkerViewOptions().position(new LatLng(0, 0));
    MarkerView marker = markerOptions.getMarker();
    assertEquals("MarkerView should match", marker, marker);
    assertEquals("MarkerViewOptions should match", markerOptions, markerOptions);
  }

  @Test
  public void testNotEquals() {
    MarkerViewOptions markerOptions = new MarkerViewOptions().position(new LatLng(0, 0));
    MarkerView marker = markerOptions.getMarker();
    assertNotEquals("MarkerViewOptions should match", markerOptions, new Object());
    assertNotEquals("MarkerView should match", marker, new Object());
  }

  @Test
  public void testEqualityBuilder() {
    MarkerViewOptions markerOne = new MarkerViewOptions().position(new LatLng(0, 0));
    MarkerViewOptions markerTwo = new MarkerViewOptions().position(new LatLng(0, 0));
    assertEquals(markerOne, markerTwo);
  }

  @Test
  public void testToString() {
    MarkerView marker = new MarkerViewOptions().position(new LatLng(0, 0)).getMarker();
    assertEquals(marker.toString(), "MarkerView [position["
      + "LatLng [latitude=0.0, longitude=0.0, altitude=0.0]" + "]]");
  }

  @Test
  public void testParcelable() {
    MarkerViewOptions markerOptions = new MarkerViewOptions().position(new LatLng()).title("t").snippet("s");
    Parcelable parcelable = MockParcel.obtain(markerOptions);
    assertEquals("Parcel should match original object", parcelable, markerOptions);
  }
}
