package com.mapbox.mapboxsdk.style.layers;

import com.mapbox.mapboxsdk.style.functions.Function;

import org.junit.Test;

import static com.mapbox.mapboxsdk.style.functions.Function.zoom;
import static com.mapbox.mapboxsdk.style.functions.stops.Stop.stop;
import static com.mapbox.mapboxsdk.style.functions.stops.Stops.interval;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineBlur;
import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertNotNull;

/**
 * Tests Function
 */
public class FunctionTest {

  @Test
  public void testZoomFunction() {
    Function<Float, Float> zoomF = zoom(interval(
      stop(1f, lineBlur(1f)),
      stop(10f, lineBlur(20f))
      )
    );

    assertNotNull(zoomF.toValueObject());
    assertArrayEquals(
      new Object[] {new Object[] {1f, 1f}, new Object[] {10f, 20f}},
      (Object[]) zoomF.toValueObject().get("stops")
    );
  }

}
