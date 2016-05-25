package com.mapbox.mapboxsdk.annotations;

import android.graphics.Bitmap;
import android.os.Parcelable;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.utils.MockParcel;

import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

public class MarkerViewTest {

    @Test
    public void testSanity() {
        MarkerViewOptions markerOptions = new MarkerViewOptions();
        assertNotNull("markerOptions should not be null", markerOptions);
    }

    @Test
    public void testMarker() {
        MarkerViewOptions markerOptions = new MarkerViewOptions();
        assertNotNull("marker should not be null", markerOptions.getMarker());
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
        MarkerViewOptions markerOptions = new MarkerViewOptions().snippet("Mapbox");
        MarkerView marker = markerOptions.getMarker();
        assertEquals(marker.getSnippet(), "Mapbox");
    }

    @Test
    public void testTitle() {
        MarkerViewOptions markerOptions = new MarkerViewOptions().title("Mapbox");
        MarkerView marker = markerOptions.getMarker();
        assertEquals(marker.getTitle(), "Mapbox");
        assertEquals(markerOptions.getTitle(), "Mapbox");
    }

    @Test
    public void testIcon() {
        Bitmap bitmap = Bitmap.createBitmap(1, 1, Bitmap.Config.ARGB_4444);
        Icon icon = IconFactory.recreate("test", bitmap);
        MarkerViewOptions markerOptions = new MarkerViewOptions().icon(icon);
        MarkerView marker = markerOptions.getMarker();
        assertEquals("Icon should match", icon, marker.getIcon());
        assertEquals("Icon should match", icon, markerOptions.getIcon());
    }

    @Test
    public void testFlat() {
        MarkerViewOptions markerOptions = new MarkerViewOptions().flat(true);
        MarkerView marker = markerOptions.getMarker();
        assertTrue("flat should be true", marker.isFlat());
    }

    @Test
    public void testFlatDefault() {
        assertFalse("default value of flat should be false", new MarkerViewOptions().getMarker().isFlat());
    }

    @Test
    public void testAnchor() {
        float anchorU = 1;
        float anchorV = 1;
        MarkerViewOptions markerOptions = new MarkerViewOptions().anchor(anchorU, anchorV);
        MarkerView marker = markerOptions.getMarker();
        assertEquals("anchorU should match ", anchorU, marker.getAnchorU(), 0);
        assertEquals("anchorU should match ", anchorV, marker.getAnchorV(), 0);
    }

    @Test
    public void testAnchorDefault() {
        MarkerView marker = new MarkerViewOptions().getMarker();
        assertEquals("anchorU should match ", 0.5, marker.getAnchorU(), 0);
        assertEquals("anchorU should match ", 1, marker.getAnchorV(), 0);
    }

    @Test
    public void testInfoWindowAnchor() {
        float anchorU = 1;
        float anchorV = 1;
        MarkerViewOptions markerOptions = new MarkerViewOptions().infoWindowAnchor(anchorU, anchorV);
        MarkerView marker = markerOptions.getMarker();
        assertEquals("anchorU should match ", 1, marker.getInfoWindowAnchorU(), 0);
        assertEquals("anchorU should match ", 1, marker.getInfoWindowAnchorV(), 0);
    }

    @Test
    public void testInfoWindowAnchorDefault() {
        MarkerView marker = new MarkerViewOptions().getMarker();
        assertEquals("anchorU should match ", 0.5, marker.getInfoWindowAnchorU(), 0);
        assertEquals("anchorU should match ", 0, marker.getInfoWindowAnchorV(), 0);
    }

    @Test
    public void testSelectAnimRes() {
        int animatorRes = R.animator.rotate_360;
        MarkerViewOptions markerOptions = new MarkerViewOptions().selectAnimatorResource(animatorRes);
        MarkerView marker = markerOptions.getMarker();
        assertEquals("select animator resource should match ", animatorRes, marker.getSelectAnimRes(), 0);
    }

    @Test
    public void testDeselectAnimRes() {
        int animatorRes = R.animator.rotate_360;
        MarkerViewOptions markerOptions = new MarkerViewOptions().deselectAnimatorResource(animatorRes);
        MarkerView marker = markerOptions.getMarker();
        assertEquals("deselect animator resource should match ", animatorRes, marker.getDeselectAnimRes(), 0);
    }

    @Test
    public void testRotation() {
        int rotation = 90;
        MarkerViewOptions markerOptions = new MarkerViewOptions().rotation(rotation);
        MarkerView marker = markerOptions.getMarker();
        assertEquals("rotation should match ", rotation, marker.getRotation(), 0);
    }

    @Test
    public void testVisible() {
        boolean visible = false;
        MarkerViewOptions markerOptions = new MarkerViewOptions().visible(visible);
        MarkerView marker = markerOptions.getMarker();
        assertEquals("visible should match ", visible, marker.isVisible());
    }

    @Test
    public void testVisibleDefault() {
        assertTrue(new MarkerViewOptions().getMarker().isVisible());
    }

    @Test
    public void testBuilder() {
        MarkerView marker = new MarkerViewOptions().title("title").snippet("snippet").position(new LatLng(10, 12)).getMarker();
        assertEquals(marker.getSnippet(), "snippet");

        assertEquals(marker.getPosition(), new LatLng(10, 12));
    }

    @Test
    public void testHashCode() {
        MarkerView marker = new MarkerViewOptions().getMarker();
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
        assertEquals(marker.toString(), "MarkerView [position[" + "LatLng [latitude=0.0, longitude=0.0, altitude=0.0]" + "]]");
    }

    @Test
    public void testParcelable() {
        MarkerViewOptions markerOptions = new MarkerViewOptions().position(new LatLng()).title("t").snippet("s");
        Parcelable parcelable = MockParcel.obtain(markerOptions);
        assertEquals("Parcel should match original object", parcelable, markerOptions);
    }
}
