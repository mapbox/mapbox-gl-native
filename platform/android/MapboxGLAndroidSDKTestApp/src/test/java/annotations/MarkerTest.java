package annotations;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;

import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

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
        Marker marker = new MarkerOptions().position(new LatLng(10, 12)).getMarker();
        assertEquals(marker.getPosition(), new LatLng(10, 12));
    }

    @Test
    public void testTitle() {
        Marker marker = new MarkerOptions().title("Mapbox").getMarker();
        assertEquals(marker.getTitle(), "Mapbox");
    }

    @Test
    public void testSnippet() {
        Marker marker = new MarkerOptions().snippet("Mapbox").getMarker();
        assertEquals(marker.getSnippet(), "Mapbox");
    }

    @Test
    public void testBuilder() {
        Marker marker = new MarkerOptions().title("title").snippet("snippet").position(new LatLng(10, 12)).getMarker();
        assertEquals(marker.getTitle(), "title");
        assertEquals(marker.getSnippet(), "snippet");
        assertEquals(marker.getPosition(), new LatLng(10, 12));
    }

    @Test
    public void testSprite() {
        // find a way to test Sprite
    }

    @Test
    public void testHashCode() {
        Marker marker = new MarkerOptions().position(new LatLng(10, 12)).getMarker();
        assertEquals("hash code should match", marker.hashCode(), -1946419200);
    }

    @Test
    public void testEquality() {
        Marker markerOne = new MarkerOptions().position(new LatLng(0, 0)).getMarker();
        Marker markerTwo = new MarkerOptions().position(new LatLng(0, 0)).getMarker();
        assertEquals(markerOne, markerTwo);
    }

    @Test
    public void testToString() {
        Marker marker = new MarkerOptions().position(new LatLng(0, 0)).getMarker();
        assertEquals(marker.toString(), "Marker [position[" + "LatLng [longitude=0.0, latitude=0.0, altitude=0.0]" + "]]");
    }

}
