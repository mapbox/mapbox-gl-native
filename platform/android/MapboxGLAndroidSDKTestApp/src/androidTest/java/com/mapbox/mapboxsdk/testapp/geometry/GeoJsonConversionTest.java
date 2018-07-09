package com.mapbox.mapboxsdk.testapp.geometry;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.GeometryCollection;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.CustomGeometrySource;
import com.mapbox.mapboxsdk.style.sources.GeometryTileProvider;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import org.junit.Test;

import java.util.Collections;

import static junit.framework.Assert.assertNotNull;

/**
 * Instrumentation test to validate java geojson conversion to c++
 */
public class GeoJsonConversionTest extends BaseActivityTest {

  // Regression test for #12343
  @Test
  public void testGetCameraForLatLngBounds() {
    validateTestSetup();
    onMapView().perform(getMapboxMapAction((uiController, mapboxMap) -> {
      mapboxMap.addSource(new CustomGeometrySource("test-id", new CustomProvider()));
      mapboxMap.addLayer(new SymbolLayer("test-id", "test-id"));
      assertNotNull(mapboxMap);
    }));
  }

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
  }

  class CustomProvider implements GeometryTileProvider {
    @Override
    public FeatureCollection getFeaturesForBounds(LatLngBounds bounds, int zoom) {
      return FeatureCollection.fromFeatures(
        Collections.singletonList(
          Feature.fromGeometry(
            GeometryCollection.fromGeometries(
              Collections.emptyList()))));
    }
  }
}
