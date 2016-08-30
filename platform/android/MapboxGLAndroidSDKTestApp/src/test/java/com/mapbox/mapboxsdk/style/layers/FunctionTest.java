package com.mapbox.mapboxsdk.style.layers;

import org.junit.Test;

import static com.mapbox.mapboxsdk.style.layers.Function.*;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.*;

import static org.junit.Assert.*;

/**
 * Tests Function
 */
public class FunctionTest {

    @Test
    public void testZoomFunction() {
        Function zoomF = zoom(
                stop(1f, lineBlur(1f)),
                stop(10f, lineBlur(20f))
        );

        assertNotNull(zoomF.toValueObject());
        assertArrayEquals(
                new Object[]{new Object[]{1f, 1f}, new Object[]{10f, 20f}},
                (Object[]) zoomF.toValueObject().get("stops")
        );
    }

}
