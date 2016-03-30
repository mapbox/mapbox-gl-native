package com.mapbox.mapboxsdk.geometry;

import android.os.Parcel;

import com.mapbox.mapboxsdk.utils.MockParcel;

import org.junit.Test;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

public class ProjectedMetersTest {

    private static final LatLng LAT_LNG_NULL_ISLAND = new LatLng(0, 0);

    @Test
    public void testSanity() {
        ProjectedMeters projectedMeters = new ProjectedMeters(0.0, 0.0);
        assertNotNull("projectedMeters should not be null", projectedMeters);
    }

    @Test
    public void testEquality() {
        ProjectedMeters projectedMeters = new ProjectedMeters(0.0, 0.0);
        assertEquals("projectedMeters is not equal to a LatLng", projectedMeters.equals(LAT_LNG_NULL_ISLAND), false);
        assertEquals("projectedMeters is equal to itself", projectedMeters.equals(projectedMeters), true);
    }

    @Test
    public void testNorthing() {
        ProjectedMeters projectedMeters = new ProjectedMeters(1.0, 0.0);
        assertEquals("northing should be 1", 1, projectedMeters.getNorthing(), 0);
    }

    @Test
    public void testEasting() {
        ProjectedMeters projectedMeters = new ProjectedMeters(0.0, 1.0);
        assertEquals("easting should be 1", 1, projectedMeters.getEasting(), 0);
    }

    @Test
    public void testConstructor() {
        ProjectedMeters projectedMeters1 = new ProjectedMeters(1, 2);
        ProjectedMeters projectedMeters2 = new ProjectedMeters(projectedMeters1);
        assertEquals("projectedmeters should match", projectedMeters1, projectedMeters2);
    }

    @Test
    public void testHashcode() {
        ProjectedMeters meters = new ProjectedMeters(1, 2);
        assertEquals("hashcode should match", -1048576, meters.hashCode());
    }

    @Test
    public void testToString() {
        ProjectedMeters meters = new ProjectedMeters(1, 1);
        assertEquals("toString should match", "ProjectedMeters [northing=1.0, easting=1.0]", meters.toString());
    }

    @Test
    public void testParcelable() {
        ProjectedMeters object = new ProjectedMeters(1, 1);
        Parcel parcel = MockParcel.obtain(object);
        object.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);
        ProjectedMeters parceable = ProjectedMeters.CREATOR.createFromParcel(parcel);
        assertEquals("parcel should match initial object", object, parceable);
    }

    @Test
    public void testParcelableArray() {
        ProjectedMeters object1 = new ProjectedMeters(1, 1);
        ProjectedMeters object2 = new ProjectedMeters(2, 2);
        ProjectedMeters[] objects = new ProjectedMeters[]{object1, object2};
        Parcel parcel = MockParcel.obtain(objects);
        parcel.writeParcelableArray(objects, 0);
        parcel.setDataPosition(0);
        ProjectedMeters[] parcelableArray = (ProjectedMeters[]) parcel.readParcelableArray(ProjectedMeters.class.getClassLoader());
        assertArrayEquals("parcel should match initial object", objects, parcelableArray);
    }

    @Test
    public void testDescribeContents() {
        ProjectedMeters object = new ProjectedMeters(1, 1);
        assertEquals("contents should be 0", 0, object.describeContents());
    }

}
