package com.mapbox.mapboxsdk.camera;

import android.content.res.TypedArray;
import android.os.Parcelable;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.MockParcel;

import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotEquals;
import static org.junit.Assert.assertNotNull;
import static org.mockito.Mockito.doNothing;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class CameraPositionTest {

  private static final double DELTA = 1e-15;

  @Test
  public void testSanity() {
    LatLng latLng = new LatLng(1, 2);
    CameraPosition cameraPosition = new CameraPosition(latLng, 3, 4, 5);
    assertNotNull("cameraPosition should not be null", cameraPosition);
  }

  @Test
  public void testDefaultTypedArrayBuilder() {
    TypedArray typedArray = null;
    CameraPosition cameraPosition = new CameraPosition.Builder(typedArray).build();
    assertEquals("bearing should match", -1, cameraPosition.bearing, DELTA);
    assertEquals("latlng should match", null, cameraPosition.target);
    assertEquals("tilt should match", -1, cameraPosition.tilt, DELTA);
    assertEquals("zoom should match", -1, cameraPosition.zoom, DELTA);
  }

  @Test
  public void testTypedArrayBuilder() {
    float bearing = 180;
    float zoom = 12;
    float latitude = 10;
    float longitude = 11;
    float tilt = 44;

    TypedArray typedArray = mock(TypedArray.class);
    when(typedArray.getFloat(R.styleable.mapbox_MapView_mapbox_cameraBearing, 0.0f)).thenReturn(bearing);
    when(typedArray.getFloat(R.styleable.mapbox_MapView_mapbox_cameraTargetLat, 0.0f)).thenReturn(latitude);
    when(typedArray.getFloat(R.styleable.mapbox_MapView_mapbox_cameraTargetLng, 0.0f)).thenReturn(longitude);
    when(typedArray.getFloat(R.styleable.mapbox_MapView_mapbox_cameraZoom, 0.0f)).thenReturn(zoom);
    when(typedArray.getFloat(R.styleable.mapbox_MapView_mapbox_cameraTilt, 0.0f)).thenReturn(tilt);
    doNothing().when(typedArray).recycle();

    CameraPosition cameraPosition = new CameraPosition.Builder(typedArray).build();
    assertEquals("bearing should match", bearing, cameraPosition.bearing, DELTA);
    assertEquals("latlng should match", new LatLng(latitude, longitude), cameraPosition.target);
    assertEquals("tilt should match", tilt, cameraPosition.tilt, DELTA);
    assertEquals("zoom should match", zoom, cameraPosition.zoom, DELTA);
  }

  @Test
  public void testToString() {
    LatLng latLng = new LatLng(1, 2);
    CameraPosition cameraPosition = new CameraPosition(latLng, 3, 4, 5);
    assertEquals("toString should match", "Target: LatLng [latitude=1.0, longitude=2.0, altitude=0.0], Zoom:3.0, "
      + "Bearing:5.0, Tilt:4.0", cameraPosition.toString());
  }

  @Test
  public void testHashcode() {
    LatLng latLng = new LatLng(1, 2);
    CameraPosition cameraPosition = new CameraPosition(latLng, 3, 4, 5);
    assertEquals("hashCode should match", -1007681505, cameraPosition.hashCode());
  }

  @Test
  public void testZoomUpdateBuilder() {
    float zoomLevel = 5;
    CameraPosition.Builder builder = new CameraPosition.Builder(
      (CameraUpdateFactory.ZoomUpdate) CameraUpdateFactory.zoomTo(zoomLevel));
    assertEquals("zoom should match", zoomLevel, builder.build().zoom, 0);
  }

  @Test
  public void testEquals() {
    LatLng latLng = new LatLng(1, 2);
    CameraPosition cameraPosition = new CameraPosition(latLng, 3, 4, 5);
    CameraPosition cameraPositionBearing = new CameraPosition(latLng, 3, 4, 9);
    CameraPosition cameraPositionTilt = new CameraPosition(latLng, 3, 9, 5);
    CameraPosition cameraPositionZoom = new CameraPosition(latLng, 9, 4, 5);
    CameraPosition cameraPositionTarget = new CameraPosition(new LatLng(), 3, 4, 5);

    assertEquals("cameraPosition should match itself", cameraPosition, cameraPosition);
    assertNotEquals("cameraPosition should not match null", null, cameraPosition);
    assertNotEquals("cameraPosition should not match object", new Object(), cameraPosition);
    assertNotEquals("cameraPosition should not match for bearing", cameraPositionBearing, cameraPosition);
    assertNotEquals("cameraPosition should not match for tilt", cameraPositionTilt, cameraPosition);
    assertNotEquals("cameraPosition should not match for zoom", cameraPositionZoom, cameraPosition);
    assertNotEquals("cameraPosition should not match for target", cameraPositionTarget, cameraPosition);
  }

  @Test
  public void testParcelable() {
    CameraPosition object = new CameraPosition(new LatLng(1, 2), 3, 4, 5);
    Parcelable parcelable = MockParcel.obtain(object);
    assertEquals("Parcel should match original object", parcelable, object);
  }
}
