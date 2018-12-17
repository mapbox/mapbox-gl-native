package com.mapbox.mapboxsdk.testapp.geometry;

import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Point;
import com.mapbox.geojson.Polygon;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.CustomGeometrySource;
import com.mapbox.mapboxsdk.style.sources.GeometryTileProvider;
import com.mapbox.mapboxsdk.testapp.activity.BaseActivityTest;
import com.mapbox.mapboxsdk.testapp.activity.espresso.EspressoTestActivity;
import org.junit.Test;

import static com.mapbox.geojson.Feature.fromGeometry;
import static com.mapbox.geojson.FeatureCollection.fromFeatures;
import static com.mapbox.geojson.GeometryCollection.fromGeometries;
import static com.mapbox.geojson.LineString.fromLngLats;
import static com.mapbox.geojson.MultiLineString.fromLineString;
import static com.mapbox.geojson.MultiPolygon.fromPolygon;
import static java.util.Collections.emptyList;
import static java.util.Collections.singletonList;

/**
 * Instrumentation test to validate java geojson conversion to c++
 */
public class GeoJsonConversionTest extends BaseActivityTest {

  // Regression test for #12343
  @Test
  public void testEmptyFeatureCollection() {
    validateTestSetup();
    onMapView().perform(getMapboxMapAction((uiController, mapboxMap) -> {
      mapboxMap.getStyle().addSource(
        new CustomGeometrySource("test-id",
          new CustomProvider(fromFeatures(singletonList(fromGeometry(fromGeometries(emptyList())))))
        )
      );
      mapboxMap.getStyle().addLayer(new SymbolLayer("test-id", "test-id"));
    }));
  }

  @Test
  public void testPointFeatureCollection() {
    validateTestSetup();
    onMapView().perform(getMapboxMapAction((uiController, mapboxMap) -> {
      mapboxMap.getStyle().addSource(
        new CustomGeometrySource("test-id",
          new CustomProvider(fromFeatures(singletonList(fromGeometry(Point.fromLngLat(0.0,0.0)))))
        )
      );
      mapboxMap.getStyle().addLayer(new SymbolLayer("test-id", "test-id"));
    }));
  }

  @Test
  public void testMultiPointFeatureCollection() {
    validateTestSetup();
    onMapView().perform(getMapboxMapAction((uiController, mapboxMap) -> {
      mapboxMap.getStyle().addSource(
        new CustomGeometrySource("test-id",
          new CustomProvider(fromFeatures(singletonList(fromGeometry(fromLngLats(emptyList())))))
        )
      );
      mapboxMap.getStyle().addLayer(new SymbolLayer("test-id", "test-id"));
    }));
  }


  @Test
  public void testPolygonFeatureCollection() {
    validateTestSetup();
    onMapView().perform(getMapboxMapAction((uiController, mapboxMap) -> {
      mapboxMap.getStyle().addSource(
        new CustomGeometrySource("test-id",
          new CustomProvider(fromFeatures(singletonList(fromGeometry(Polygon.fromLngLats(emptyList())))))
        )
      );
      mapboxMap.getStyle().addLayer(new SymbolLayer("test-id", "test-id"));
    }));
  }

  @Test
  public void testMultiPolygonFeatureCollection() {
    validateTestSetup();
    onMapView().perform(getMapboxMapAction((uiController, mapboxMap) -> {
      mapboxMap.getStyle().addSource(
        new CustomGeometrySource("test-id",
          new CustomProvider(fromFeatures(singletonList(fromGeometry(fromPolygon(Polygon.fromLngLats(emptyList()))))))
        )
      );
      mapboxMap.getStyle().addLayer(new SymbolLayer("test-id", "test-id"));
    }));
  }

  @Test
  public void testLineStringFeatureCollection() {
    validateTestSetup();
    onMapView().perform(getMapboxMapAction((uiController, mapboxMap) -> {
      mapboxMap.getStyle().addSource(
        new CustomGeometrySource("test-id",
          new CustomProvider(fromFeatures(singletonList(fromGeometry(fromLngLats(emptyList())))))
        )
      );
      mapboxMap.getStyle().addLayer(new SymbolLayer("test-id", "test-id"));
    }));
  }

  @Test
  public void testMultiLineStringFeatureCollection() {
    validateTestSetup();
    onMapView().perform(getMapboxMapAction((uiController, mapboxMap) -> {
      mapboxMap.getStyle().addSource(
        new CustomGeometrySource("test-id",
          new CustomProvider(fromFeatures(singletonList(fromGeometry(fromLineString(fromLngLats(emptyList()))))))
        )
      );
      mapboxMap.getStyle().addLayer(new SymbolLayer("test-id", "test-id"));
    }));
  }

  @Override
  protected Class getActivityClass() {
    return EspressoTestActivity.class;
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
