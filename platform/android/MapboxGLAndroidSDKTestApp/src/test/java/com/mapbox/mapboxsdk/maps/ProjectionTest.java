package com.mapbox.mapboxsdk.maps;

import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;
import org.mockito.MockitoAnnotations;

import static org.junit.Assert.assertNotNull;
import static org.mockito.Mockito.mock;

public class ProjectionTest {

    @InjectMocks
    MapView mMapView = mock(MapView.class);

    @Before
    public void beforeTest() {
        MockitoAnnotations.initMocks(this);
    }

    @Test
    public void testSanity() {
        Projection projection = new Projection(mMapView);
        assertNotNull("projection should not be null", projection);
    }
}
