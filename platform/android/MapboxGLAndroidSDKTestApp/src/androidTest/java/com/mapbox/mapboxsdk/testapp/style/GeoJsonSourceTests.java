package com.mapbox.mapboxsdk.testapp.style;

import android.support.annotation.RawRes;
import android.support.test.espresso.ViewAction;
import android.support.test.runner.AndroidJUnit4;
import android.view.View;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction;
import com.mapbox.mapboxsdk.testapp.activity.EspressoTest;
import com.mapbox.mapboxsdk.testapp.utils.ResourceUtils;
import com.mapbox.mapboxsdk.testapp.utils.TestingAsyncUtils;

import org.hamcrest.Matcher;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.IOException;

import timber.log.Timber;

import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static org.junit.Assert.assertEquals;

/**
 * Tests for {@link GeoJsonSource}
 */
@RunWith(AndroidJUnit4.class)
public class GeoJsonSourceTests extends EspressoTest {

  @Test
  public void testFeatureCollection() {
    validateTestSetup();
    MapboxMapAction.invoke(mapboxMap, (uiController, mapboxMap) -> {
      GeoJsonSource source = null;
      try {
        source = new GeoJsonSource("source", FeatureCollection
          .fromJson(ResourceUtils.readRawResource(rule.getActivity(), R.raw.test_feature_collection)));
      } catch (IOException exception) {
        Timber.e(exception);
      }
      mapboxMap.getStyle().addSource(source);
      mapboxMap.getStyle().addLayer(new CircleLayer("layer", source.getId()));
    });
  }

  @Test
  public void testPointGeometry() {
    validateTestSetup();
    MapboxMapAction.invoke(mapboxMap, (uiController, mapboxMap) -> {
      GeoJsonSource source = new GeoJsonSource("source", Point.fromLngLat(0d, 0d));
      mapboxMap.getStyle().addSource(source);
      mapboxMap.getStyle().addLayer(new CircleLayer("layer", source.getId()));
    });
  }

  @Test
  public void testFeatureProperties() {
    validateTestSetup();
    MapboxMapAction.invoke(mapboxMap, (uiController, mapboxMap) -> {
      GeoJsonSource source = null;
      try {
        source = new GeoJsonSource("source",
          ResourceUtils.readRawResource(rule.getActivity(), R.raw.test_feature_properties));
      } catch (IOException exception) {
        Timber.e(exception);
      }
      mapboxMap.getStyle().addSource(source);
      mapboxMap.getStyle().addLayer(new CircleLayer("layer", source.getId()));
    });
  }

  @Test
  public void testUpdateCoalescing() {
    validateTestSetup();
    MapboxMapAction.invoke(mapboxMap, (uiController, mapboxMap) -> {
      GeoJsonSource source = new GeoJsonSource("source");
      mapboxMap.getStyle().addSource(source);
      mapboxMap.getStyle().addLayer(new CircleLayer("layer", source.getId()));

      source.setGeoJson(Point.fromLngLat(0, 0));
      source.setGeoJson(Point.fromLngLat(-25, -25));
      try {
        source.setGeoJson(ResourceUtils.readRawResource(rule.getActivity(), R.raw.test_feature_properties));
      } catch (IOException exception) {
        Timber.e(exception);
      }

      source.setGeoJson(Point.fromLngLat(20, 55));
      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);
      assertEquals(1, mapboxMap.queryRenderedFeatures(
        mapboxMap.getProjection().toScreenLocation(
          new LatLng(55, 20)), "layer").size());
    });
  }

  @Test
  public void testPointFeature() {
    testFeatureFromResource(R.raw.test_point_feature);
  }

  @Test
  public void testLineStringFeature() {
    testFeatureFromResource(R.raw.test_line_string_feature);
  }

  @Test
  public void testPolygonFeature() {
    testFeatureFromResource(R.raw.test_polygon_feature);
  }

  @Test
  public void testPolygonWithHoleFeature() {
    testFeatureFromResource(R.raw.test_polygon_with_hole_feature);
  }

  @Test
  public void testMultiPointFeature() {
    testFeatureFromResource(R.raw.test_multi_point_feature);
  }

  @Test
  public void testMultiLineStringFeature() {
    testFeatureFromResource(R.raw.test_multi_line_string_feature);
  }

  @Test
  public void testMultiPolygonFeature() {
    testFeatureFromResource(R.raw.test_multi_polygon_feature);
  }

  protected void testFeatureFromResource(final @RawRes int resource) {
    validateTestSetup();
    MapboxMapAction.invoke(mapboxMap, (uiController, mapboxMap) -> {
      GeoJsonSource source = new GeoJsonSource("source");
      mapboxMap.getStyle().addSource(source);
      Layer layer = new CircleLayer("layer", source.getId());
      mapboxMap.getStyle().addLayer(layer);

      try {
        source.setGeoJson(Feature.fromJson(ResourceUtils.readRawResource(rule.getActivity(), resource)));
      } catch (IOException exception) {
        Timber.e(exception);
      }

      mapboxMap.getStyle().removeLayer(layer);
      mapboxMap.getStyle().removeSource(source);
    });
  }

  public abstract class BaseViewAction implements ViewAction {

    @Override
    public Matcher<View> getConstraints() {
      return isDisplayed();
    }

    @Override
    public String getDescription() {
      return getClass().getSimpleName();
    }

  }
}
