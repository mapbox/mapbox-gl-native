import com.mapbox.mapboxgl.geometry.LatLngZoom;

import org.junit.Test;

import static org.junit.Assert.assertNotNull;

public class LatLngZoomTest {

    @Test
    public void testSanity() {
        LatLngZoom latLngZoom = new LatLngZoom(0.0, 0.0, 0.0);
        assertNotNull("latLngZoom should not be null", latLngZoom);
    }

}
