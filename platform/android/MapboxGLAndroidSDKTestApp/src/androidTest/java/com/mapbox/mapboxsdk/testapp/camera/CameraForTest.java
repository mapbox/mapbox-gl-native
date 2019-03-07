package com.mapbox.mapboxsdk.testapp.camera;

import android.support.annotation.NonNull;
import android.support.test.annotation.UiThreadTest;
import com.mapbox.geojson.Point;
import com.mapbox.geojson.Polygon;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.testapp.activity.BaseTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.DeviceIndependentTestActivity;
import org.junit.Test;

import java.util.ArrayList;
import java.util.List;

import static org.junit.Assert.assertEquals;

public class CameraForTest extends BaseTest {

  @Test
  @UiThreadTest
  public void testGetCameraForLatLngBounds() {
    validateTestSetup();
    CameraPosition actualPosition = mapboxMap.getCameraForLatLngBounds(
      LatLngBounds.from(10, 10, -10, -10));
    CameraPosition expectedPosition = new CameraPosition.Builder()
      .target(new LatLng()).zoom(4.16).tilt(0).bearing(0).build();
    assertEquals("Latitude should match",
      expectedPosition.target.getLatitude(), actualPosition.target.getLatitude(), 0.00001f);
    assertEquals("Longitude should match",
      expectedPosition.target.getLongitude(), actualPosition.target.getLongitude(), 0.00001f);
    assertEquals("Bearing should match",
      expectedPosition.zoom, actualPosition.zoom, 0.01f);
    assertEquals("Tilt should match", expectedPosition.tilt, actualPosition.tilt, 0.01f);
  }

  @Test
  @UiThreadTest
  public void testGetCameraForLatLngBoundsPadding() {
    validateTestSetup();
    CameraPosition actualPosition = mapboxMap.getCameraForLatLngBounds(
      LatLngBounds.from(10, 10, -10, -10), new int[] {5, 5, 5, 5});
    CameraPosition expectedPosition = new CameraPosition.Builder()
      .target(new LatLng()).zoom(4.13).tilt(0).bearing(0).build();
    assertEquals("Latitude should match",
      expectedPosition.target.getLatitude(), actualPosition.target.getLatitude(), 0.00001f);
    assertEquals("Longitude should match",
      expectedPosition.target.getLongitude(), actualPosition.target.getLongitude(), 0.00001f);
    assertEquals("Zoom should match",
      expectedPosition.zoom, actualPosition.zoom, 0.01f);
    assertEquals("Tilt should match",
      expectedPosition.tilt, actualPosition.tilt, 0.01f);
    assertEquals("Bearing should match",
      expectedPosition.bearing, actualPosition.bearing, 0.01f);
  }

  @Test
  @UiThreadTest
  public void testGetCameraForLatLngBoundsBearing() {
    validateTestSetup();
    CameraPosition actualPosition = mapboxMap.getCameraForLatLngBounds(
      LatLngBounds.from(10, 10, -10, -10), 45, 0);
    CameraPosition expectedPosition = new CameraPosition.Builder()
      .target(new LatLng()).zoom(3.66).tilt(0).bearing(45).build();
    assertEquals("Latitude should match",
      expectedPosition.target.getLatitude(), actualPosition.target.getLatitude(), 0.00001f);
    assertEquals("Longitude should match",
      expectedPosition.target.getLongitude(), actualPosition.target.getLongitude(), 0.00001f);
    assertEquals("Zoom should match",
      expectedPosition.zoom, actualPosition.zoom, 0.01f);
    assertEquals("Tilt should match",
      expectedPosition.tilt, actualPosition.tilt, 0.01f);
    assertEquals("Bearing should match",
      expectedPosition.bearing, actualPosition.bearing, 0.01f);
  }

  @Test
  @UiThreadTest
  public void testGetCameraForLatLngBoundsTilt() {
    validateTestSetup();
    CameraPosition actualPosition = mapboxMap.getCameraForLatLngBounds(
      LatLngBounds.from(10, 10, -10, -10), 0, 45);
    CameraPosition expectedPosition = new CameraPosition.Builder()
      .target(new LatLng(-0.264576975267, 0)).zoom(4.13).tilt(45).bearing(0).build();
    assertEquals("Latitude should match",
      expectedPosition.target.getLatitude(), actualPosition.target.getLatitude(), 0.00001f);
    assertEquals("Longitude should match",
      expectedPosition.target.getLongitude(), actualPosition.target.getLongitude(), 0.00001f);
    assertEquals("Zoom should match",
      expectedPosition.zoom, actualPosition.zoom, 0.01f);
    assertEquals("Tilt should match",
      expectedPosition.tilt, actualPosition.tilt, 0.01f);
    assertEquals("Bearing should match",
      expectedPosition.bearing, actualPosition.bearing, 0.01f);
  }

  @Test
  @UiThreadTest
  public void testGetCameraForLatLngBoundsAll() {
    validateTestSetup();
    CameraPosition actualPosition = mapboxMap.getCameraForLatLngBounds(
      LatLngBounds.from(10, 10, -10, -10), new int[] {5, 5, 5, 5}, 45, 45);
    CameraPosition expectedPosition = new CameraPosition.Builder()
      .target(new LatLng(-0.3732134634, -0.3713191053)).zoom(3.63).tilt(45).bearing(45).build();
    assertEquals("Latitude should match",
      expectedPosition.target.getLatitude(), actualPosition.target.getLatitude(), 0.00001f);
    assertEquals("Longitude should match",
      expectedPosition.target.getLongitude(), actualPosition.target.getLongitude(), 0.00001f);
    assertEquals("Zoom should match",
      expectedPosition.zoom, actualPosition.zoom, 0.01f);
    assertEquals("Tilt should match",
      expectedPosition.tilt, actualPosition.tilt, 0.01f);
    assertEquals("Bearing should match",
      expectedPosition.bearing, actualPosition.bearing, 0.01f);
  }

  @Test
  @UiThreadTest
  public void testGetCameraForGeometry() {
    validateTestSetup();
    List<List<Point>> polygonDefinition = getPolygonDefinition();
    CameraPosition actualPosition = mapboxMap.getCameraForGeometry(Polygon.fromLngLats(polygonDefinition));
    CameraPosition expectedPosition = new CameraPosition.Builder()
      .target(new LatLng()).zoom(4.16).tilt(0).bearing(0).build();
    assertEquals("Latitude should match",
      expectedPosition.target.getLatitude(), actualPosition.target.getLatitude(), 0.00001f);
    assertEquals("Longitude should match",
      expectedPosition.target.getLongitude(), actualPosition.target.getLongitude(), 0.00001f);
    assertEquals("Bearing should match",
      expectedPosition.zoom, actualPosition.zoom, 0.01f);
    assertEquals("Tilt should match", expectedPosition.tilt, actualPosition.tilt, 0.01f);
  }

  @NonNull
  private List<List<Point>> getPolygonDefinition() {
    return new ArrayList<List<Point>>() {
      {
        add(new ArrayList<Point>() {
          {
            add(Point.fromLngLat(10, 10));
            add(Point.fromLngLat(-10, 10));
            add(Point.fromLngLat(-10, -10));
            add(Point.fromLngLat(10, -10));
          }
        });
      }
    };
  }

  @Test
  @UiThreadTest
  public void testGetCameraForGeometryPadding() {
    validateTestSetup();
    List<List<Point>> polygonDefinition = getPolygonDefinition();
    CameraPosition actualPosition = mapboxMap.getCameraForGeometry(Polygon.fromLngLats(polygonDefinition),
      new int[] {5, 5, 5, 5});
    CameraPosition expectedPosition = new CameraPosition.Builder()
      .target(new LatLng()).zoom(4.13).tilt(0).bearing(0).build();
    assertEquals("Latitude should match",
      expectedPosition.target.getLatitude(), actualPosition.target.getLatitude(), 0.00001f);
    assertEquals("Longitude should match",
      expectedPosition.target.getLongitude(), actualPosition.target.getLongitude(), 0.00001f);
    assertEquals("Zoom should match",
      expectedPosition.zoom, actualPosition.zoom, 0.01f);
    assertEquals("Tilt should match",
      expectedPosition.tilt, actualPosition.tilt, 0.01f);
    assertEquals("Bearing should match",
      expectedPosition.bearing, actualPosition.bearing, 0.01f);
  }

  @Test
  @UiThreadTest
  public void testGetCameraForGeometryBearing() {
    validateTestSetup();
    List<List<Point>> polygonDefinition = getPolygonDefinition();
    CameraPosition actualPosition = mapboxMap.getCameraForGeometry(Polygon.fromLngLats(polygonDefinition), 45, 0);
    CameraPosition expectedPosition = new CameraPosition.Builder()
      .target(new LatLng()).zoom(3.66).tilt(0).bearing(45).build();
    assertEquals("Latitude should match",
      expectedPosition.target.getLatitude(), actualPosition.target.getLatitude(), 0.00001f);
    assertEquals("Longitude should match",
      expectedPosition.target.getLongitude(), actualPosition.target.getLongitude(), 0.00001f);
    assertEquals("Zoom should match",
      expectedPosition.zoom, actualPosition.zoom, 0.01f);
    assertEquals("Tilt should match",
      expectedPosition.tilt, actualPosition.tilt, 0.01f);
    assertEquals("Bearing should match",
      expectedPosition.bearing, actualPosition.bearing, 0.01f);
  }

  @Test
  @UiThreadTest
  public void testGetCameraForGeometryTilt() {
    validateTestSetup();
    List<List<Point>> polygonDefinition = getPolygonDefinition();
    CameraPosition actualPosition = mapboxMap.getCameraForGeometry(Polygon.fromLngLats(polygonDefinition), 0, 45);
    CameraPosition expectedPosition = new CameraPosition.Builder()
      .target(new LatLng(-0.2645769752, 0)).zoom(4.13).tilt(45).bearing(0).build();
    assertEquals("Latitude should match",
      expectedPosition.target.getLatitude(), actualPosition.target.getLatitude(), 0.00001f);
    assertEquals("Longitude should match",
      expectedPosition.target.getLongitude(), actualPosition.target.getLongitude(), 0.00001f);
    assertEquals("Zoom should match",
      expectedPosition.zoom, actualPosition.zoom, 0.01f);
    assertEquals("Tilt should match",
      expectedPosition.tilt, actualPosition.tilt, 0.01f);
    assertEquals("Bearing should match",
      expectedPosition.bearing, actualPosition.bearing, 0.01f);
  }

  @Test
  @UiThreadTest
  public void testGetCameraForGeometryAll() {
    validateTestSetup();
    List<List<Point>> polygonDefinition = getPolygonDefinition();
    CameraPosition actualPosition = mapboxMap.getCameraForGeometry(Polygon.fromLngLats(polygonDefinition),
      new int[] {5, 5, 5, 5}, 45, 45);
    CameraPosition expectedPosition = new CameraPosition.Builder()
      .target(new LatLng(-0.373213463, -0.37131910534)).zoom(3.63).tilt(45).bearing(45).build();
    assertEquals("Latitude should match",
      expectedPosition.target.getLatitude(), actualPosition.target.getLatitude(), 0.00001f);
    assertEquals("Longitude should match",
      expectedPosition.target.getLongitude(), actualPosition.target.getLongitude(), 0.00001f);
    assertEquals("Zoom should match",
      expectedPosition.zoom, actualPosition.zoom, 0.01f);
    assertEquals("Tilt should match",
      expectedPosition.tilt, actualPosition.tilt, 0.01f);
    assertEquals("Bearing should match",
      expectedPosition.bearing, actualPosition.bearing, 0.01f);
  }

  @Test
  @UiThreadTest
  public void testGetCameraForGeometryDeprecatedApi() {
    validateTestSetup();
    List<List<Point>> polygonDefinition = getPolygonDefinition();
    CameraPosition actualPosition = mapboxMap.getCameraForGeometry(
      Polygon.fromLngLats(polygonDefinition),
      new int[] {5, 5, 5, 5},
      45, 0);
    CameraPosition expectedPosition = new CameraPosition.Builder()
      .target(new LatLng()).zoom(3.63).tilt(0).bearing(45).build();
    assertEquals("Latitude should match",
      expectedPosition.target.getLatitude(), actualPosition.target.getLatitude(), 0.00001f);
    assertEquals("Longitude should match",
      expectedPosition.target.getLongitude(), actualPosition.target.getLongitude(), 0.00001f);
    assertEquals("Zoom should match",
      expectedPosition.zoom, actualPosition.zoom, 0.01f);
    assertEquals("Tilt should match",
      expectedPosition.tilt, actualPosition.tilt, 0.01f);
    assertEquals("Bearing should match",
      expectedPosition.bearing, actualPosition.bearing, 0.01f);
  }

  @Override
  protected Class getActivityClass() {
    return DeviceIndependentTestActivity.class;
  }
}
