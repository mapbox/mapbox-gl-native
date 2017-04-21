package com.mapbox.mapboxsdk.testapp.style;

import android.content.res.Resources;
import android.support.annotation.RawRes;
import android.support.test.espresso.UiController;
import android.support.test.espresso.ViewAction;
import android.support.test.runner.AndroidJUnit4;
import android.view.View;

import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.style.RuntimeStyleTestActivity;
import com.mapbox.services.commons.geojson.Feature;
import com.mapbox.services.commons.geojson.FeatureCollection;
import com.mapbox.services.commons.geojson.Point;

import org.hamcrest.Matcher;
import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;
import java.io.Writer;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.matcher.ViewMatchers.isDisplayed;
import static android.support.test.espresso.matcher.ViewMatchers.withId;
import static org.junit.Assert.fail;

/**
 * Tests for {@link GeoJsonSource}
 */
@RunWith(AndroidJUnit4.class)
public class GeoJsonSourceTests extends BaseActivityTest {

  @Override
  protected Class getActivityClass() {
    return RuntimeStyleTestActivity.class;
  }

  @Test
  public void testFeatureCollection() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new BaseViewAction() {

      @Override
      public void perform(UiController uiController, View view) {
        GeoJsonSource source = new GeoJsonSource("source", FeatureCollection
          .fromJson(readRawResource(rule.getActivity().getResources(), R.raw.test_feature_collection)));
        mapboxMap.addSource(source);

        mapboxMap.addLayer(new CircleLayer("layer", source.getId()));
      }

    });
  }

  @Test
  public void testPointGeometry() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new BaseViewAction() {

      @Override
      public void perform(UiController uiController, View view) {
        GeoJsonSource source = new GeoJsonSource("source", Point.fromCoordinates(new double[] {0d, 0d}));
        mapboxMap.addSource(source);

        mapboxMap.addLayer(new CircleLayer("layer", source.getId()));
      }

    });
  }

  @Test
  public void testFeatureProperties() {
    validateTestSetup();
    onView(withId(R.id.mapView)).perform(new BaseViewAction() {

      @Override
      public void perform(UiController uiController, View view) {
        GeoJsonSource source = new GeoJsonSource("source",
          readRawResource(rule.getActivity().getResources(), R.raw.test_feature_properties));
        mapboxMap.addSource(source);

        mapboxMap.addLayer(new CircleLayer("layer", source.getId()));
      }

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
    onView(withId(R.id.mapView)).perform(new BaseViewAction() {

      @Override
      public void perform(UiController uiController, View view) {
        GeoJsonSource source = new GeoJsonSource("source");
        mapboxMap.addSource(source);
        Layer layer = new CircleLayer("layer", source.getId());
        mapboxMap.addLayer(layer);

        source.setGeoJson(Feature.fromJson(
          readRawResource(rule.getActivity().getResources(), resource)));

        mapboxMap.removeLayer(layer);
        mapboxMap.removeSource(source);
      }

    });
  }

  private String readRawResource(Resources resources, @RawRes int rawResource) {
    InputStream is = resources.openRawResource(rawResource);
    Writer writer = new StringWriter();
    char[] buffer = new char[1024];
    try {
      try {
        Reader reader = new BufferedReader(new InputStreamReader(is, "UTF-8"));
        int numRead;
        while ((numRead = reader.read(buffer)) != -1) {
          writer.write(buffer, 0, numRead);
        }
      } finally {
        is.close();
      }
    } catch (IOException err) {
      fail(err.getMessage());
    }

    return writer.toString();
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
