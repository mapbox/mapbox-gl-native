import com.mapbox.mapboxgl.geometry.CoordinateRegion;
import com.mapbox.mapboxgl.geometry.CoordinateSpan;
import com.mapbox.mapboxgl.geometry.LatLng;

import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

public class CoordinateRegionTest {

    @Test
    public void testSanity() {
        LatLng center = new LatLng(1.2, 3.4, 5.6);
        CoordinateSpan span = new CoordinateSpan(9.8, 7.6);
        CoordinateRegion coordinateRegion = new CoordinateRegion(center, span);
        assertNotNull("coordinateRegion should not be null", coordinateRegion);
    }

    @Test
    public void testCenterConstructor() {
        LatLng center = new LatLng(1.2, 3.4, 5.6);
        CoordinateSpan span = new CoordinateSpan(9.8, 7.6);
        CoordinateRegion coordinateRegion = new CoordinateRegion(center, span);
        assertEquals("center should match", coordinateRegion.getCenter(), center);
    }

    @Test
    public void testSpanConstructor() {
        LatLng center = new LatLng(1.2, 3.4, 5.6);
        CoordinateSpan span = new CoordinateSpan(9.8, 7.6);
        CoordinateRegion coordinateRegion = new CoordinateRegion(center, span);
        assertEquals("span should match", coordinateRegion.getSpan(), span);
    }

    @Test
    public void testCenterMethod() {
        LatLng center = new LatLng(1.2, 3.4, 5.6);
        CoordinateSpan span = new CoordinateSpan(9.8, 7.6);
        CoordinateRegion coordinateRegion = new CoordinateRegion(null, span);
        coordinateRegion.setCenter(center);
        assertEquals("center should match", coordinateRegion.getCenter(), center);
    }

    @Test
    public void testSpanMethod() {
        LatLng center = new LatLng(1.2, 3.4, 5.6);
        CoordinateSpan span = new CoordinateSpan(9.8, 7.6);
        CoordinateRegion coordinateRegion = new CoordinateRegion(center, null);
        coordinateRegion.setSpan(span);
        assertEquals("span should match", coordinateRegion.getSpan(), span);
    }

}
