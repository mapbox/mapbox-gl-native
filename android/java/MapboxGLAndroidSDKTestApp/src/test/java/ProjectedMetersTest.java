import com.mapbox.mapboxgl.geometry.ProjectedMeters;

import org.junit.Test;

import static org.junit.Assert.assertNotNull;

public class ProjectedMetersTest {

    @Test
    public void testSanity() {
        ProjectedMeters projectedMeters = new ProjectedMeters(0.0, 0.0);
        assertNotNull("projectedMeters should not be null", projectedMeters);
    }

}
