package com.mapbox.mapboxsdk.annotations;

import com.mapbox.mapboxsdk.geometry.LatLng;

import org.junit.Test;

import java.util.ArrayList;
import java.util.List;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

public class PolygonTest {

  @Test
  public void testSanity() {
    PolygonOptions polygonOptions = new PolygonOptions();
    assertNotNull("polygonOptions should not be null", polygonOptions);
  }

  @Test
  public void testPolygon() {
    Polygon polygon = new PolygonOptions().getPolygon();
    assertNotNull("polyline should not be null", polygon);
  }

  @Test
  public void testAlpha() {
    Polygon polygon = new PolygonOptions().alpha(0.5f).getPolygon();
    assertEquals(0.5f, polygon.getAlpha(), 0.0f);
  }

  @Test
  public void testStrokeColor() {
    Polygon polygon = new PolygonOptions().strokeColor(1).getPolygon();
    assertEquals(1, polygon.getStrokeColor());
  }

  @Test
  public void testFillColor() {
    Polygon polygon = new PolygonOptions().fillColor(1).getPolygon();
    assertEquals(1, polygon.getFillColor());
  }

  @Test
  public void testLatLng() {
    Polygon polygon = new PolygonOptions().add(new LatLng(0, 0)).getPolygon();
    assertNotNull("points should not be null", polygon.getPoints());
    assertEquals(new LatLng(0, 0), polygon.getPoints().get(0));
  }

  @Test
  public void testAddAllLatLng() {
    List<LatLng> coordinates = new ArrayList<>();
    coordinates.add(new LatLng(0, 0));
    Polygon polygon = new PolygonOptions().addAll(coordinates).getPolygon();
    assertNotNull(polygon.getPoints());
    assertEquals(new LatLng(0, 0), polygon.getPoints().get(0));
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
