package com.mapbox.mapboxsdk.testapp.style;

import android.support.test.InstrumentationRegistry;
import android.support.test.runner.AndroidJUnit4;
import android.test.ActivityInstrumentationTestCase2;

import timber.log.Timber;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.layers.BackgroundLayer;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.PropertyFactory;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;

/**
 * Basic smoke tests for BackgroundLayer
 */
@RunWith(AndroidJUnit4.class)
public class RuntimeStyleBackgroundLayerTest
  extends ActivityInstrumentationTestCase2<RuntimeStyleTestActivity> {

  public RuntimeStyleBackgroundLayerTest() {
    super(RuntimeStyleTestActivity.class);
  }

  @Before
  public void setUp() throws Exception {
    super.setUp();
    injectInstrumentation(InstrumentationRegistry.getInstrumentation());
  }

  @Test
  public void testSetVisibility() {
    getActivity().mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(MapboxMap mapboxMap) {
        Timber.i("visibility");
        BackgroundLayer layer = mapboxMap.getLayerAs("background");
        assertNotNull(layer);

        //Get initial
        assertEquals(layer.getVisibility().getValue(), Property.VISIBLE);

        //Set
        layer.setProperties(PropertyFactory.visibility(Property.NONE));
        assertEquals(layer.getVisibility().getValue(), Property.NONE);
      }
    });
  }

  @After
  public void tearDown() throws Exception {
    super.tearDown();
  }
}
