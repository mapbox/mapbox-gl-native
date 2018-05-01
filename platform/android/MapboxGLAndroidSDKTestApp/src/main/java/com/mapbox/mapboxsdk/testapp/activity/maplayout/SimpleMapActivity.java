package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;

import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.expressions.Expression.interpolate;
import static com.mapbox.mapboxsdk.style.expressions.Expression.linear;
import static com.mapbox.mapboxsdk.style.expressions.Expression.literal;
import static com.mapbox.mapboxsdk.style.expressions.Expression.match;
import static com.mapbox.mapboxsdk.style.expressions.Expression.stop;
import static com.mapbox.mapboxsdk.style.expressions.Expression.zoom;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconSize;

/**
 * Test activity showcasing a simple MapView without any MapboxMap interaction.
 */
public class SimpleMapActivity extends AppCompatActivity {

  private MapView mapView;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_simple);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(mapboxMap -> {
      Point point1 = Point.fromLngLat(2.176550, 41.377432);
      Feature feature1 = Feature.fromGeometry(point1);
      feature1.addStringProperty("type", "restaurant");

      Point point2 = Point.fromLngLat(2.169974, 41.374830);
      Feature feature2 = Feature.fromGeometry(point2);
      feature2.addStringProperty("type", "other");

      FeatureCollection featureCollection = FeatureCollection.fromFeatures(new Feature[] {feature1, feature2});

      GeoJsonSource source = new GeoJsonSource("source", featureCollection);
      mapboxMap.addSource(source);

      SymbolLayer layer = new SymbolLayer("layer", "source");
      layer.setProperties(
        iconAllowOverlap(true),
        iconImage(
          match(
            get("type"), //argument
            literal("restaurant"), literal("restaurant-15"), //first case
            literal("cafe"), literal("cafe-15"), //second case
            literal("circle-stroked-15") //default
          )
        ),
        iconSize(
          interpolate(linear(), zoom(), //At the moment, Expression.zoom() can only be used as a top property, that's why we need to duplicate some code
            stop(8, match(
              get("type"),
              literal("other"), literal(0f),
              literal(1f)
            )),
            stop(14, match(
              get("type"),
              literal("other"), literal(2f),
              literal(1f)
            ))
          )
        )
      );
      mapboxMap.addLayer(layer);

      mapboxMap.addOnMapClickListener(point -> {
        if (feature1.getStringProperty("type").equals("restaurant")) {
          feature1.addStringProperty("type", "cafe");
        } else {
          feature1.addStringProperty("type", "restaurant");
        }
        source.setGeoJson(featureCollection);
      });

      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(
        new LatLng((point1.latitude() + point2.latitude()) / 2, (point1.longitude() + point2.longitude()) / 2),
        11
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
