import com.mapbox.mapboxgl.geometry.BoundingBox;

import org.junit.Test;

import static org.junit.Assert.assertNotNull;

public class BoundingBoxTest {

    @Test
    public void testSanity() {
        BoundingBox boundingBox = new BoundingBox(0.0, 0.0, 0.0, 0.0);
        assertNotNull("boundingBox  should not be null", boundingBox);
    }

}
