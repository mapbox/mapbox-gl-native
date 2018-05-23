package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.PropertyFactory;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

/**
 * Test activity showcasing a simple MapView without any MapboxMap interaction.
 */
public class SimpleMapActivity extends AppCompatActivity {

  private MapView mapView;
  private int previousSelectedIndex;
  private Random random = new Random();

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_simple);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(mapboxMap -> {
      List<Feature> features = new ArrayList<>();
      Feature feature1 = Feature.fromGeometry(Point.fromLngLat(21.032070, 52.2401));
      feature1.addBooleanProperty("selected", false);
      Feature feature2 = Feature.fromGeometry(Point.fromLngLat(21.032070, 52.2402));
      feature2.addBooleanProperty("selected", false);
      Feature feature3 = Feature.fromGeometry(Point.fromLngLat(21.032070, 52.2403));
      feature3.addBooleanProperty("selected", false);

      features.add(feature1);
      features.add(feature2);
      features.add(feature3);

      FeatureCollection featureCollection = FeatureCollection.fromFeatures(features);

      GeoJsonSource source = new GeoJsonSource("source", featureCollection);
      mapboxMap.addSource(source);

      SymbolLayer layer = new SymbolLayer("layer", "source")
        .withProperties(
          PropertyFactory.iconImage("cafe-15"),
          PropertyFactory.iconSize(2f),
          PropertyFactory.iconAllowOverlap(true)
        );
      layer.setFilter(Expression.eq(Expression.get("selected"), false));
      mapboxMap.addLayer(layer);

      SymbolLayer layerSelected = new SymbolLayer("layerSelected", "source")
        .withProperties(
          PropertyFactory.iconImage("restaurant-15"),
          PropertyFactory.iconSize(2f),
          PropertyFactory.iconAllowOverlap(true)
        );
      layerSelected.setFilter(Expression.eq(Expression.get("selected"), true));
      mapboxMap.addLayerAbove(layerSelected, "layer"); //noting, that we are explicitly z-indexing the selected layer

      mapboxMap.addOnMapClickListener(point -> {
        int index;
        do {
          index = random.nextInt(3);
        } while (index == previousSelectedIndex);

        Feature selectedFeature = features.get(index);
        selectedFeature.addBooleanProperty("selected", true);
        for (Feature deselectedFeature : features) {
          if (deselectedFeature != selectedFeature) {
            deselectedFeature.addBooleanProperty("selected", false);
          }
        }
        source.setGeoJson(featureCollection);
        previousSelectedIndex = index;
      });

      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(
        new LatLng(52.2401, 21.032070),
        15.5
      ));
    });
  }

  @Override
  protected void onStart() {
    super.onStart();
    mapView.onStart();
  }

  @Override
  protected void onResume() {
    super.onResume();
    mapView.onResume();
  }

  @Override
  protected void onPause() {
    super.onPause();
    mapView.onPause();
  }

  @Override
  protected void onStop() {
    super.onStop();
    mapView.onStop();
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }
}
