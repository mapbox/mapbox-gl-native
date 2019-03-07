package com.mapbox.mapboxsdk.testapp.geometry;

import android.support.test.annotation.UiThreadTest;
import com.google.gson.JsonArray;
import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Point;
import com.mapbox.geojson.Polygon;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.PropertyFactory;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.CustomGeometrySource;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.style.sources.GeometryTileProvider;
import com.mapbox.mapboxsdk.testapp.action.MapboxMapAction;
import com.mapbox.mapboxsdk.testapp.activity.EspressoTest;
import com.mapbox.mapboxsdk.testapp.utils.TestingAsyncUtils;
import org.junit.Test;

import static android.support.test.espresso.Espresso.onView;
import static android.support.test.espresso.matcher.ViewMatchers.isRoot;
import static com.mapbox.geojson.Feature.fromGeometry;
import static com.mapbox.geojson.FeatureCollection.fromFeatures;
import static com.mapbox.geojson.GeometryCollection.fromGeometries;
import static com.mapbox.geojson.LineString.fromLngLats;
import static com.mapbox.geojson.MultiLineString.fromLineString;
import static com.mapbox.geojson.MultiPolygon.fromPolygon;
import static java.util.Collections.emptyList;
import static java.util.Collections.singletonList;
import static org.junit.Assert.assertFalse;

/**
 * Instrumentation test to validate java geojson conversion to c++
 */
public class GeoJsonConversionTest extends EspressoTest {

  // Regression test for #12343
  @Test
  @UiThreadTest
  public void testEmptyFeatureCollection() {
    validateTestSetup();
    mapboxMap.getStyle().addSource(
      new CustomGeometrySource("test-id",
        new CustomProvider(fromFeatures(singletonList(fromGeometry(fromGeometries(emptyList())))))
      )
    );
    mapboxMap.getStyle().addLayer(new SymbolLayer("test-id", "test-id"));
  }

  @Test
  @UiThreadTest
  public void testPointFeatureCollection() {
    validateTestSetup();
    mapboxMap.getStyle().addSource(
      new CustomGeometrySource("test-id",
        new CustomProvider(fromFeatures(singletonList(fromGeometry(Point.fromLngLat(0.0, 0.0)))))
      )
    );
    mapboxMap.getStyle().addLayer(new SymbolLayer("test-id", "test-id"));
  }

  @Test
  @UiThreadTest
  public void testMultiPointFeatureCollection() {
    validateTestSetup();
    mapboxMap.getStyle().addSource(
      new CustomGeometrySource("test-id",
        new CustomProvider(fromFeatures(singletonList(fromGeometry(fromLngLats(emptyList())))))
      )
    );
    mapboxMap.getStyle().addLayer(new SymbolLayer("test-id", "test-id"));
  }

  @Test
  @UiThreadTest
  public void testPolygonFeatureCollection() {
    validateTestSetup();
    mapboxMap.getStyle().addSource(
      new CustomGeometrySource("test-id",
        new CustomProvider(fromFeatures(singletonList(fromGeometry(Polygon.fromLngLats(emptyList())))))
      )
    );
    mapboxMap.getStyle().addLayer(new SymbolLayer("test-id", "test-id"));
  }

  @Test
  @UiThreadTest
  public void testMultiPolygonFeatureCollection() {
    validateTestSetup();
    mapboxMap.getStyle().addSource(
      new CustomGeometrySource("test-id",
        new CustomProvider(fromFeatures(singletonList(fromGeometry(fromPolygon(Polygon.fromLngLats(emptyList()))))))
      )
    );
    mapboxMap.getStyle().addLayer(new SymbolLayer("test-id", "test-id"));
  }

  @Test
  @UiThreadTest
  public void testLineStringFeatureCollection() {
    validateTestSetup();
    mapboxMap.getStyle().addSource(
      new CustomGeometrySource("test-id",
        new CustomProvider(fromFeatures(singletonList(fromGeometry(fromLngLats(emptyList())))))
      )
    );
    mapboxMap.getStyle().addLayer(new SymbolLayer("test-id", "test-id"));
  }

  @Test
  @UiThreadTest
  public void testMultiLineStringFeatureCollection() {
    validateTestSetup();
    mapboxMap.getStyle().addSource(
      new CustomGeometrySource("test-id",
        new CustomProvider(fromFeatures(singletonList(fromGeometry(fromLineString(fromLngLats(emptyList()))))))
      )
    );
    mapboxMap.getStyle().addLayer(new SymbolLayer("test-id", "test-id"));
  }


  @Test
  public void testNegativeNumberPropertyConversion() {
    validateTestSetup();
    onView(isRoot()).perform(new MapboxMapAction((uiController, mapboxMap) -> {
      LatLng latLng = new LatLng();
      Feature feature = Feature.fromGeometry(Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude()));

      JsonArray foregroundJsonArray = new JsonArray();
      foregroundJsonArray.add(0f);
      foregroundJsonArray.add(-3f);
      feature.addProperty("property", foregroundJsonArray);

      GeoJsonSource source = new GeoJsonSource("source", feature);
      mapboxMap.getStyle().addSource(source);

      SymbolLayer layer = new SymbolLayer("layer", "source")
        .withProperties(
          PropertyFactory.iconOffset(Expression.get("property")),
          PropertyFactory.iconImage("zoo-15")
        );
      mapboxMap.getStyle().addLayer(layer);

      TestingAsyncUtils.INSTANCE.waitForLayer(uiController, mapView);

      assertFalse(mapboxMap.queryRenderedFeatures(mapboxMap.getProjection().toScreenLocation(latLng)).isEmpty());
    }, mapboxMap));
  }

  class CustomProvider implements GeometryTileProvider {

    private FeatureCollection featureCollection;

    CustomProvider(FeatureCollection featureCollection) {
      this.featureCollection = featureCollection;
    }

    @Override
    public FeatureCollection getFeaturesForBounds(LatLngBounds bounds, int zoom) {
      return featureCollection;
    }
  }
}