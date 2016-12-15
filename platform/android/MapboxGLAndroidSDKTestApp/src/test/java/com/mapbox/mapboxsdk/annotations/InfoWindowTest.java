package com.mapbox.mapboxsdk.annotations;

import android.graphics.PointF;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Projection;

import org.junit.Test;
import org.mockito.InjectMocks;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class InfoWindowTest {

  @InjectMocks
  MapView mMapView = mock(MapView.class);

  @InjectMocks
  MapboxMap mMapboxMap = mock(MapboxMap.class);

  @Test
  public void testSanity() {
    InfoWindow infoWindow = new InfoWindow(mMapView, mMapboxMap);
    assertNotNull("infoWindow should exist", infoWindow);
  }

  @Test
  public void testBoundMarker() {
    MarkerOptions markerOptions = new MarkerOptions();
    Marker marker = markerOptions.position(new LatLng()).getMarker();
    InfoWindow infoWindow = new InfoWindow(mMapView, mMapboxMap).setBoundMarker(marker);
    assertEquals("marker should match", marker, infoWindow.getBoundMarker());
  }

  @Test
  public void testClose() {
    InfoWindow infoWindow = new InfoWindow(mMapView, mMapboxMap);
    infoWindow.close();
    assertEquals("infowindow should not be visible", false, infoWindow.isVisible());
  }


  @Test
  public void testOpen() {
    LatLng latLng = new LatLng(0, 0);
    Projection projection = mock(Projection.class);
    when(mMapboxMap.getProjection()).thenReturn(projection);
    when(projection.toScreenLocation(latLng)).thenReturn(new PointF(0, 0));

    InfoWindow infoWindow = new InfoWindow(mMapView, mMapboxMap);
    infoWindow.open(mMapView, new MarkerOptions().position(new LatLng()).getMarker(), latLng, 0, 0);
    assertEquals("infowindow should not be visible", true, infoWindow.isVisible());
  }

  @Test
  public void testOpenClose() {
    LatLng latLng = new LatLng(0, 0);
    Projection projection = mock(Projection.class);
    when(mMapboxMap.getProjection()).thenReturn(projection);
    when(projection.toScreenLocation(latLng)).thenReturn(new PointF(0, 0));

    InfoWindow infoWindow = new InfoWindow(mMapView, mMapboxMap);
    infoWindow.open(mMapView, new MarkerOptions().position(new LatLng()).getMarker(), latLng, 0, 0);
    infoWindow.close();
    assertEquals("infowindow should not be visible", false, infoWindow.isVisible());
  }


  @Test
  public void testUpdate() {
    LatLng latLng = new LatLng(0, 0);
    Projection projection = mock(Projection.class);
    when(mMapboxMap.getProjection()).thenReturn(projection);
    when(projection.toScreenLocation(latLng)).thenReturn(new PointF(0, 0));

    InfoWindow infoWindow = new InfoWindow(mMapView, mMapboxMap);
    infoWindow.open(mMapView, new MarkerOptions().position(latLng).getMarker(), latLng, 0, 0);
    infoWindow.update();
  }

}
