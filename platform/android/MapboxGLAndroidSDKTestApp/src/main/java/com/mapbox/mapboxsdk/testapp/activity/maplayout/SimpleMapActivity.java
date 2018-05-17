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
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.PropertyFactory;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;
import java.util.List;

/**
 * Test activity showcasing a simple MapView without any MapboxMap interaction.
 */
public class SimpleMapActivity extends AppCompatActivity {

  private MapView mapView;
  private boolean selected;

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

      Layer layer = new SymbolLayer("layer", "source")
        .withProperties(PropertyFactory.iconImage(
          Expression.switchCase(
            Expression.get("selected"), Expression.literal(""),
            Expression.literal("cafe-15")
          )),
          PropertyFactory.iconSize(2f),
          PropertyFactory.iconAllowOverlap(true));
      mapboxMap.addLayer(layer);

      Layer layerSelected = new SymbolLayer("layerSelected", "source")
        .withProperties(PropertyFactory.iconImage(
          Expression.switchCase(
            Expression.get("selected"), Expression.literal("restaurant-15"),
            Expression.literal("")
          )),
          PropertyFactory.iconSize(2f),
          PropertyFactory.iconAllowOverlap(true));
      mapboxMap.addLayerAbove(layerSelected, "layer"); //noting, that we are explicitly z-indexing the selected layer

      mapboxMap.addOnMapClickListener(point -> {
        if (selected) {
          feature3.addBooleanProperty("selected", false);
        } else {
          feature3.addBooleanProperty("selected", true);
        }
        source.setGeoJson(featureCollection);
        selected = !selected;
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
