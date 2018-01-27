package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.MultiLineString;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.layers.LineLayer;
import com.mapbox.mapboxsdk.style.sources.CustomGeometrySource;
import com.mapbox.mapboxsdk.style.sources.GeometryTileProvider;
import com.mapbox.mapboxsdk.testapp.R;


import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineColor;

/**
 * Test activity showcasing using CustomGeometrySource to create a grid overlay on the map.
 */
public class GridSourceActivity extends AppCompatActivity implements OnMapReadyCallback {

  private static final String ID_GRID_SOURCE = "grid_source";
  private static final String ID_GRID_LAYER = "grid_layer";

  private MapView mapView;
  private MapboxMap mapboxMap;

  /**
   * Implementation of GeometryTileProvider that returns features representing a zoom-dependent
   * grid.
   */
  static class GridProvider implements GeometryTileProvider {
    public FeatureCollection getFeaturesForBounds(LatLngBounds bounds, int zoom) {
      List<Feature> features = new ArrayList<>();
      double gridSpacing;
      if (zoom >= 13) {
        gridSpacing = 0.01;
      } else if (zoom >= 11) {
        gridSpacing = 0.05;
      } else if (zoom == 10) {
        gridSpacing = .1;
      } else if (zoom == 9) {
        gridSpacing = 0.25;
      } else if (zoom == 8) {
        gridSpacing = 0.5;
      } else if (zoom >= 6) {
        gridSpacing = 1;
      } else if (zoom == 5) {
        gridSpacing = 2;
      } else if (zoom >= 4) {
        gridSpacing = 5;
      } else if (zoom == 2) {
        gridSpacing = 10;
      } else {
        gridSpacing = 20;
      }

      List gridLines = new ArrayList();
      for (double y = Math.ceil(bounds.getLatNorth() / gridSpacing) * gridSpacing;
           y >= Math.floor(bounds.getLatSouth() / gridSpacing) * gridSpacing; y -= gridSpacing) {
        gridLines.add(Arrays.asList(Point.fromLngLat(bounds.getLonWest(), y),
          Point.fromLngLat(bounds.getLonEast(), y)));
      }
      features.add(Feature.fromGeometry(MultiLineString.fromLngLats(gridLines)));

      gridLines = new ArrayList();
      for (double x = Math.floor(bounds.getLonWest() / gridSpacing) * gridSpacing;
           x <= Math.ceil(bounds.getLonEast() / gridSpacing) * gridSpacing; x += gridSpacing) {
        gridLines.add(Arrays.asList(Point.fromLngLat(x, bounds.getLatSouth()),
          Point.fromLngLat(x, bounds.getLatNorth())));
      }
      features.add(Feature.fromGeometry(MultiLineString.fromLngLats(gridLines)));

      return FeatureCollection.fromFeatures(features);
    }
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_grid_source);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
  }

  @Override
  public void onMapReady(@NonNull final MapboxMap map) {
    mapboxMap = map;

    // add source
    CustomGeometrySource source = new CustomGeometrySource(ID_GRID_SOURCE, new GridProvider());
    mapboxMap.addSource(source);

    // add layer
    LineLayer layer = new LineLayer(ID_GRID_LAYER, ID_GRID_SOURCE);
    layer.setProperties(
      lineColor(Color.parseColor("#000000"))
    );

    mapboxMap.addLayer(layer);
  }

  @Override
  protected void onStart() {
    super.onStart();
    mapView.onStart();
  }

  @Override
  public void onResume() {
    super.onResume();
    mapView.onResume();
  }

  @Override
  public void onPause() {
    super.onPause();
    mapView.onPause();
  }

  @Override
  protected void onStop() {
    super.onStop();
    mapView.onStop();
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
