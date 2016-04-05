package com.mapbox.mapboxsdk.annotations;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Parcel;
import android.os.Parcelable;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.MockParcel;

import org.junit.Test;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotEquals;
import static org.junit.Assert.assertNotNull;
import static org.mockito.Mockito.mock;

public class MarkerTest {

    @Test
    public void testSanity() {
        MarkerOptions markerOptions = new MarkerOptions();
        assertNotNull("markerOptions should not be null", markerOptions);
    }

    @Test
    public void testMarker() {
        MarkerOptions markerOptions = new MarkerOptions();
        assertNotNull("marker should not be null", markerOptions.getMarker());
    }

    @Test
    public void testPosition() {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng(10, 12));
        Marker marker = markerOptions.getMarker();
        assertEquals(marker.getPosition(), new LatLng(10, 12));
        assertEquals(markerOptions.getPosition(), new LatLng(10, 12));
    }

    @Test
    public void testTitle() {
        MarkerOptions markerOptions = new MarkerOptions().title("Mapbox");
        Marker marker = markerOptions.getMarker();
        assertEquals(marker.getTitle(), "Mapbox");
        assertEquals(markerOptions.getTitle(), "Mapbox");
    }

    @Test
    public void testSnippet() {
        MarkerOptions markerOptions = new MarkerOptions().snippet("Mapbox");
        Marker marker = markerOptions.getMarker();
        assertEquals(marker.getSnippet(), "Mapbox");
    }

    @Test
    public void testBuilder() {
        Marker marker = new MarkerOptions().title("title").snippet("snippet").position(new LatLng(10, 12)).getMarker();
        assertEquals(marker.getSnippet(), "snippet");

        assertEquals(marker.getPosition(), new LatLng(10, 12));
    }

    @Test
    public void testIcon() {
        Bitmap bitmap = Bitmap.createBitmap(1, 1, Bitmap.Config.ARGB_4444);
        Icon icon = IconFactory.recreate("test", bitmap);
        MarkerOptions markerOptions = new MarkerOptions().icon(icon);
        Marker marker = markerOptions.getMarker();
        assertEquals("Icon should match", icon, marker.getIcon());
        assertEquals("Icon should match", icon, markerOptions.getIcon());
    }

    @Test
    public void testHashCode() {
        Marker marker = new MarkerOptions().position(new LatLng(10, 12)).getMarker();
        assertEquals("hash code should match", marker.hashCode(), -1946419200);
    }

    @Test
    public void testHashCodeBuilder() {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng(10, 12));
        assertEquals("hash code should match", markerOptions.hashCode(), 579999617);
    }

    @Test
    public void testEquals() {
        Marker markerOne = new MarkerOptions().position(new LatLng(0, 0)).getMarker();
        Marker markerTwo = new MarkerOptions().position(new LatLng(0, 0)).getMarker();
        assertEquals(markerOne, markerTwo);
    }

    @Test
    public void testEqualityDifferentLocation() {
        MarkerOptions marker = new MarkerOptions().position(new LatLng(0, 0));
        MarkerOptions other = new MarkerOptions().position(new LatLng(1, 0));
        assertNotEquals("Should not match", other, marker);
    }


    @Test
    public void testEqualityDifferentSnippet() {
        MarkerOptions marker = new MarkerOptions().snippet("s");
        MarkerOptions other = new MarkerOptions();
        assertNotEquals("Should not match", other, marker);
    }

    @Test
    public void testEqualityDifferentIcon() {
        MarkerOptions marker = new MarkerOptions().icon(mock(Icon.class));
        MarkerOptions other = new MarkerOptions();
        assertNotEquals("Should not match", other, marker);
    }

    @Test
    public void testEqualityDifferentTitle() {
        MarkerOptions marker = new MarkerOptions().title("t");
        MarkerOptions other = new MarkerOptions();
        assertNotEquals("Should not match", other, marker);
    }

    @Test
    public void testEqualsItself() {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng(0, 0));
        Marker marker = markerOptions.getMarker();
        assertEquals("Marker should match", marker, marker);
        assertEquals("MarkerOptions should match", markerOptions, markerOptions);
    }

    @Test
    public void testNotEquals() {
        MarkerOptions markerOptions = new MarkerOptions().position(new LatLng(0, 0));
        Marker marker = markerOptions.getMarker();
        assertNotEquals("MarkerOptions should match", markerOptions, new Object());
        assertNotEquals("Marker should match", marker, new Object());
    }

    @Test
    public void testEqualityBuilder() {
        MarkerOptions markerOne = new MarkerOptions().position(new LatLng(0, 0));
        MarkerOptions markerTwo = new MarkerOptions().position(new LatLng(0, 0));
        assertEquals(markerOne, markerTwo);
    }

    @Test
    public void testToString() {
        Marker marker = new MarkerOptions().position(new LatLng(0, 0)).getMarker();
        assertEquals(marker.toString(), "Marker [position[" + "LatLng [latitude=0.0, longitude=0.0, altitude=0.0]" + "]]");
    }

    @Test
    public void testParcelable() {
        MarkerOptions object = new MarkerOptions().position(new LatLng(0, 0)).snippet("s").title("t");
        Parcel parcel = MockParcel.obtain(object);
        object.writeToParcel(parcel, 0);
        parcel.setDataPosition(0);
        MarkerOptions parceable = MarkerOptions.CREATOR.createFromParcel(parcel);
        assertEquals("parcel should match initial object", object, parceable);
    }

    @Test
    public void testParcelableArray() {
        MarkerOptions[] objects = new MarkerOptions[]{new MarkerOptions().position(new LatLng(0, 0)), new MarkerOptions().position(new LatLng(1, 1))};
        Parcel parcel = MockParcel.obtain(objects);
        parcel.writeParcelableArray(objects, 0);
        parcel.setDataPosition(0);
        MarkerOptions[] parcelableArray = (MarkerOptions[]) parcel.readParcelableArray(LatLng.class.getClassLoader());
        assertArrayEquals("parcel should match initial object", objects, parcelableArray);
    }

    @Test
    public void testDescribeContents() {
        MarkerOptions object = new MarkerOptions().position(new LatLng(0, 0)).snippet("s").title("t");
        assertEquals("contents should be 0", 1034152.0, object.describeContents(), 0);
    }
}
