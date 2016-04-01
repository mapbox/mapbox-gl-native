package com.mapbox.mapboxsdk.camera;

import android.content.res.TypedArray;
import android.os.Parcel;

import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.MathConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.utils.MathUtils;
import com.mapbox.mapboxsdk.utils.MockParcel;

import org.junit.Test;

import static org.junit.Assert.assertArrayEquals;
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
        when(typedArray.getFloat(R.styleable.MapView_direction, 0.0f)).thenReturn(bearing);
        when(typedArray.getFloat(R.styleable.MapView_center_latitude, 0.0f)).thenReturn(latitude);
        when(typedArray.getFloat(R.styleable.MapView_center_longitude, 0.0f)).thenReturn(longitude);
        when(typedArray.getFloat(R.styleable.MapView_zoom, 0.0f)).thenReturn(zoom);
        when(typedArray.getFloat(R.styleable.MapView_tilt, 0.0f)).thenReturn(tilt);
        doNothing().when(typedArray).recycle();

        CameraPosition cameraPosition = new CameraPosition.Builder(typedArray).build();
        assertEquals("bearing should match", bearing, cameraPosition.bearing, DELTA);
        assertEquals("latlng should match", new LatLng(latitude, longitude), cameraPosition.target);
        assertEquals("tilt should match", tilt, cameraPosition.tilt, DELTA);
        assertEquals("zoom should match", zoom, cameraPosition.zoom, DELTA);
    }

    @Test
    public void testJniBuilder() {
        double bearing = 180;
        double zoom = 12;
        double latitude = 10;
        double longitude = 11;
        double tilt = 44;

        double[] cameraVars = new double[]{latitude, longitude, bearing, tilt, zoom};
        CameraPosition cameraPosition = new CameraPosition.Builder(cameraVars).build();
        assertEquals("bearing should match", bearing, cameraPosition.bearing, DELTA);
        assertEquals("latlng should match", new LatLng(latitude, longitude), cameraPosition.target);
        assertEquals("tilt should match", tilt, cameraPosition.tilt, DELTA);
        assertEquals("zoom should match", zoom, cameraPosition.zoom, DELTA);
    }

    @Test
    public void testToString() {
        LatLng latLng = new LatLng(1, 2);
        CameraPosition cameraPosition = new CameraPosition(latLng, 3, 4, 5);
        assertEquals("toString should match", "Target: LatLng [latitude=1.0, longitude=2.0, altitude=0.0], Zoom:3.0, Bearing:5.0, Tilt:4.0", cameraPosition.toString());
    }

    @Test
    public void testHashcode() {
        LatLng latLng = new LatLng(1, 2);
        CameraPosition cameraPosition = new CameraPosition(latLng, 3, 4, 5);
        assertEquals("toString should match", -1007681505, cameraPosition.hashCode());
    }

    @Test
    public void testRadiantBuilder() {
        LatLng latLng = new LatLng(1, 2);
        CameraPosition.Builder builder = new CameraPosition.Builder(true);
        builder.target(latLng);
        builder.zoom(3);
        builder.tilt(4);
        builder.bearing(5);
        CameraPosition cameraPosition = new CameraPosition(latLng, 3, 4, 5);
        assertEquals("CameraPosition should match", cameraPosition, builder.build());
    }

    @Test
    public void testDegreesRadiantBuilder() {
        LatLng latLng = new LatLng(1, 2);
        float tilt = 4;
        float bearing = 5;
        float bearingRadiant = (float) (-bearing * MathConstants.DEG2RAD);
        float tiltRadiant = (float) (MathUtils.clamp(tilt, MapboxConstants.MINIMUM_TILT, MapboxConstants.MAXIMUM_TILT) * MathConstants.DEG2RAD);

        CameraPosition.Builder degreeBuilder = new CameraPosition.Builder(false);
        degreeBuilder.target(latLng);
        degreeBuilder.zoom(3);
        degreeBuilder.tilt(tilt);
        degreeBuilder.bearing(bearing);

        CameraPosition.Builder radiantBuilder = new CameraPosition.Builder(true);
        radiantBuilder.target(latLng);
        radiantBuilder.zoom(3);
        radiantBuilder.tilt(tiltRadiant);
        radiantBuilder.bearing(bearingRadiant);
        assertEquals("CameraPosition should match", radiantBuilder.build(), degreeBuilder.build());
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
        LatLng latLng = new LatLng(1, 2);
        CameraPosition object = new CameraPosition(latLng, 3, 4, 5);
        Parcel parcel = MockParcel.obtain(object);
        object.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);
        CameraPosition parceable = CameraPosition.CREATOR.createFromParcel(parcel);
        assertEquals("parcel should match initial object", object, parceable);
    }

    @Test
    public void testParcelableArray() {
        LatLng latLng = new LatLng(1, 2);
        CameraPosition object1 = new CameraPosition(latLng, 3, 4, 5);
        CameraPosition object2 = new CameraPosition(latLng, 9, 9, 9);
        CameraPosition[] objects = new CameraPosition[]{object1, object2};
        Parcel parcel = MockParcel.obtain(objects);
        parcel.writeParcelableArray(objects, 0);
        parcel.setDataPosition(0);
        CameraPosition[] parcelableArray = (CameraPosition[]) parcel.readParcelableArray(CameraPosition.class.getClassLoader());
        assertArrayEquals("parcel should match initial object", objects, parcelableArray);
    }

    @Test
    public void testDescribeContents() {
        LatLng latLng = new LatLng(1, 2);
        CameraPosition object = new CameraPosition(latLng, 3, 4, 5);
        assertEquals("contents should be 0", 0, object.describeContents());
    }
}
