import android.location.Location;

import com.mapbox.mapboxgl.geometry.LatLng;

import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class LatLngTest {

    private static final double DELTA = 1e-15;

    @Test
    public void testSanity() {
        LatLng latLng = new LatLng(0.0, 0.0);
        assertNotNull("latLng  should not be null", latLng);
    }

    @Test
    public void testLatitudeConstructor() {
        double latitude = 1.2;
        LatLng latLng = new LatLng(latitude, 3.4);
        assertEquals("latitude should match", latLng.getLatitude(), latitude, DELTA);
    }

    @Test
    public void testLongitudeConstructor() {
        double longitude = 3.4;
        LatLng latLng = new LatLng(1.2, longitude);
        assertEquals("longitude should match", latLng.getLongitude(), longitude, DELTA);
    }

    @Test
    public void testAltitudeConstructor() {
        LatLng latLng1 = new LatLng(1.2, 3.4);
        assertEquals("altitude default value", latLng1.getAltitude(), 0.0, DELTA);

        double altitude = 5.6;
        LatLng latLng2 = new LatLng(1.2, 3.4, altitude);
        assertEquals("altitude default value", latLng2.getAltitude(), altitude, DELTA);
    }

    /*
     * A sample test where Mockito is necessary
     */

    @Test
    public void testLocationProvider() {
        double latitude = 1.2;
        double longitude = 3.4;
        double altitude = 5.6;

        // Mock the location class
        Location locationMocked = mock(Location.class);
        when(locationMocked.getLatitude()).thenReturn(latitude);
        when(locationMocked.getLongitude()).thenReturn(longitude);
        when(locationMocked.getAltitude()).thenReturn(altitude);

        // Test the constructor
        LatLng latLng = new LatLng(locationMocked);
        assertEquals("latitude should match", latLng.getLatitude(), latitude, DELTA);
        assertEquals("longitude should match", latLng.getLongitude(), longitude, DELTA);
        assertEquals("altitude should match", latLng.getAltitude(), altitude, DELTA);
    }

    @Test
    public void testHashCode() {
        double latitude = 1.2;
        double longitude = 3.4;
        double altitude = 5.6;
        LatLng latLng = new LatLng(latitude, longitude, altitude);
        assertEquals("hash code should match", latLng.hashCode(), -151519232);
    }

    @Test
    public void testToString() {
        double latitude = 1.2;
        double longitude = 3.4;
        double altitude = 5.6;
        LatLng latLng = new LatLng(latitude, longitude, altitude);
        assertEquals("string should match",
                latLng.toString(),
                "LatLng [longitude=3.4, latitude=1.2, altitude=5.6]");
    }

    @Test
    public void testDistanceTo() {
        LatLng latLng1 = new LatLng(0.0, 0.0);
        LatLng latLng2 = new LatLng(1.0, 1.0);
        assertEquals("distances should match",
                latLng1.distanceTo(latLng2),
                157425.53710839353, DELTA);
    }

}
