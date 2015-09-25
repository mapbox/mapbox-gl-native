import com.mapbox.mapboxgl.geometry.CoordinateSpan;

import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

public class CoordinateSpanTest {

    private static final double DELTA = 1e-15;

    @Test
    public void testSanity() {
        CoordinateSpan coordinateSpan = new CoordinateSpan(0.0, 0.0);
        assertNotNull("coordinateSpan should not be null", coordinateSpan);
    }

    @Test
    public void testLatitudeConstructor() {
        double latitude = 1.23;
        CoordinateSpan coordinateSpan = new CoordinateSpan(latitude, 0.0);
        assertEquals("latitude in constructor", coordinateSpan.getLatitudeSpan(), latitude, DELTA);
    }

    @Test
    public void testLongitudeConstructor() {
        double longitude = 1.23;
        CoordinateSpan coordinateSpan = new CoordinateSpan(0.0, longitude);
        assertEquals("latitude in constructor", coordinateSpan.getLongitudeSpan(), longitude, DELTA);
    }

    @Test
    public void testLatitudeMethod() {
        double latitude = 1.23;
        CoordinateSpan coordinateSpan = new CoordinateSpan(0.0, 0.0);
        coordinateSpan.setLatitudeSpan(latitude);
        assertEquals("latitude in constructor", coordinateSpan.getLatitudeSpan(), latitude, DELTA);
    }

    @Test
    public void testLongitudeMethod() {
        double longitude = 1.23;
        CoordinateSpan coordinateSpan = new CoordinateSpan(0.0, 0.0);
        coordinateSpan.setLongitudeSpan(longitude);
        assertEquals("latitude in constructor", coordinateSpan.getLongitudeSpan(), longitude, DELTA);
    }

}
