package com.mapbox.mapboxsdk.annotations;

import com.mapbox.mapboxsdk.geometry.LatLng;

import org.junit.Test;

import java.util.ArrayList;
import java.util.List;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

public class PolylineTest {

  @Test
  public void testSanity() {
    PolylineOptions polylineOptions = new PolylineOptions();
    assertNotNull("polylineOptions should not be null", polylineOptions);
  }

  @Test
  public void testPolyline() {
    Polyline polyline = new PolylineOptions().getPolyline();
    assertNotNull("polyline should not be null", polyline);
  }

  @Test
  public void testAlpha() {
    Polyline polyline = new PolylineOptions().alpha(0.2f).getPolyline();
    assertEquals(0.2f, polyline.getAlpha(), 0.0f);
  }

  @Test
  public void testWidth() {
    Polyline polyline = new PolylineOptions().width(1).getPolyline();
    assertEquals(1.0f, polyline.getWidth(), 0);
  }

  @Test
  public void testColor() {
    Polyline polyline = new PolylineOptions().color(1).getPolyline();
    assertEquals(1, polyline.getColor());
  }

  @Test
  public void testAddLatLng() {
    Polyline polyline = new PolylineOptions().add(new LatLng(0, 0)).getPolyline();
    assertNotNull("Points should not be null", polyline.getPoints());
    assertEquals(new LatLng(0, 0), polyline.getPoints().get(0));
  }

  @Test
  public void testAddAllLatLng() {
    List<LatLng> coordinates = new ArrayList<>();
    coordinates.add(new LatLng(0, 0));
    Polyline polyline = new PolylineOptions().addAll(coordinates).getPolyline();
    assertNotNull(polyline.getPoints());
    assertEquals(new LatLng(0, 0), polyline.getPoints().get(0));
  }

  @Test
  public void testBuilder() {
    PolylineOptions polylineOptions = new PolylineOptions();
    polylineOptions.width(1.0f);
    polylineOptions.color(2);
    polylineOptions.add(new LatLng(0, 0));

    Polyline polyline = polylineOptions.getPolyline();
    assertEquals(1.0f, polyline.getWidth(), 0);
    assertEquals(2, polyline.getColor());
    assertNotNull("Points should not be null", polyline.getPoints());
    assertEquals(new LatLng(0, 0), polyline.getPoints().get(0));
  }

}
