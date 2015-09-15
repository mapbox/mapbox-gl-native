import com.mapbox.mapboxgl.geometry.CoordinateSpan;

import org.junit.Test;

import static org.junit.Assert.assertNotNull;

public class CoordinateSpanTest {

    @Test
    public void testSanity() {
        CoordinateSpan coordinateSpan = new CoordinateSpan(0.0, 0.0);
        assertNotNull("coordinateSpan should not be null", coordinateSpan);
    }

}
