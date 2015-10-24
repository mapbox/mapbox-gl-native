import com.mapbox.mapboxsdk.geometry.CoordinateBounds;
import com.mapbox.mapboxsdk.geometry.LatLng;

import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

public class CoordinateBoundsTest {

    @Test
    public void testSanity() {
        CoordinateBounds coordinateBounds = new CoordinateBounds(null, null);
        assertNotNull("coordinateBounds should not be null", coordinateBounds);
    }

    @Test
    public void testSouthWestConstructor() {
        LatLng southWest = new LatLng(12, 12);
        CoordinateBounds coordinateBounds = new CoordinateBounds(southWest, null);
        assertEquals("southWest should match", southWest, coordinateBounds.getSouthWest());
    }

    @Test
    public void testNorthEastConstructor() {
        LatLng northEast = new LatLng(12, 12);
        CoordinateBounds coordinateBounds = new CoordinateBounds(null, northEast);
        assertEquals("northEast should match", northEast, coordinateBounds.getNorthEast());
    }

    @Test
    public void testHashCode() {
        LatLng northEast = new LatLng(60, 60);
        LatLng southWest = new LatLng(43, 26);
        CoordinateBounds coordinateBounds = new CoordinateBounds(northEast, southWest);
        assertEquals("hash code should match", coordinateBounds.hashCode(), -1515487232);
    }

    @Test
    public void testToString() {
        LatLng northEast = new LatLng(60, 60);
        LatLng southWest = new LatLng(43, 26);
        CoordinateBounds coordinateBounds = new CoordinateBounds(northEast, southWest);
        assertEquals("string should match",
                coordinateBounds.toString(),
                "CoordinateBounds [northEast[" + coordinateBounds.getNorthEast() + "], southWest[]" + coordinateBounds.getSouthWest() + "]");
    }

    @Test
    public void testEquals() {
        LatLng northEast = new LatLng(60, 60);
        LatLng southWest = new LatLng(43, 26);
        CoordinateBounds firstBounds = new CoordinateBounds(northEast, southWest);
        CoordinateBounds sameBounds = new CoordinateBounds(northEast, southWest);
        assertTrue(firstBounds.equals(sameBounds));
    }
}
