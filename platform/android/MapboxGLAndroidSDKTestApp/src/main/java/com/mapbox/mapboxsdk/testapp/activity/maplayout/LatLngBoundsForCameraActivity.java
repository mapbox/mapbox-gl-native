package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.FrameLayout;
import com.mapbox.geojson.Point;
import com.mapbox.geojson.Polygon;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;
import java.util.List;

import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillOpacity;

/**
 * Test activity showcasing restricting user gestures to a bounds around Iceland, almost worldview and IDL.
 */
public class LatLngBoundsForCameraActivity extends AppCompatActivity implements OnMapReadyCallback {

  private static final String FILL_SOURCE_ID = "fill_source_id";
  private static final String FILL_LAYER_ID = "fill_layer_id";

  private static final LatLngBounds ICELAND_BOUNDS = new LatLngBounds.Builder()
    .include(new LatLng(66.852863, -25.985652))
    .include(new LatLng(62.985661, -12.626277))
    .build();

  private static final LatLngBounds ALMOST_WORLD_BOUNDS = new LatLngBounds.Builder()
    .include(new LatLng(20.0, 170.0))
    .include(new LatLng(-20, -170.0))
    .build();

  private static final LatLngBounds CROSS_IDL_BOUNDS = new LatLngBounds.Builder()
    .include(new LatLng(20.0, 170.0))
    .include(new LatLng(-20, 190.0))
    .build();

  private MapView mapView;
  private MapboxMap mapboxMap;
  private GeoJsonSource geoJsonSource;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_restricted_bounds);

    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
  }

  @Override
  public void onMapReady(@NonNull MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;

    geoJsonSource = new GeoJsonSource(FILL_SOURCE_ID);
    FillLayer fillLayer = new FillLayer(FILL_LAYER_ID, FILL_SOURCE_ID)
      .withProperties(
        fillOpacity(0.25f),
        fillColor(Color.RED)
      );

    mapboxMap.setStyle(new Style.Builder()
      .fromUrl(Style.SATELLITE_STREETS)
      .withSource(geoJsonSource)
      .withLayer(fillLayer)
    );

    mapboxMap.setMinZoomPreference(2);
    mapboxMap.getUiSettings().setFlingVelocityAnimationEnabled(false);
    showCrosshair();
    setupBounds(ICELAND_BOUNDS);
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_bounds, menu);
    return true;
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {
      case R.id.menu_action_allmost_world_bounds:
        setupBounds(ALMOST_WORLD_BOUNDS);
        return true;
      case R.id.menu_action_cross_idl:
        setupBounds(CROSS_IDL_BOUNDS);
        return true;
    }
    return super.onOptionsItemSelected(item);
  }

  private void setupBounds(LatLngBounds bounds) {
    mapboxMap.setLatLngBoundsForCameraTarget(bounds);
    showBoundsArea(bounds);
  }

  private void showBoundsArea(LatLngBounds bounds) {
    List<Point> points = new ArrayList<>();
    points.add(convertToPoint(bounds.getNorthWest()));
    points.add(convertToPoint(bounds.getNorthEast()));
    points.add(convertToPoint(bounds.getSouthEast()));
    points.add(convertToPoint(bounds.getSouthWest()));
    points.add(convertToPoint(bounds.getNorthWest()));

    List<List<Point>> polygon = new ArrayList<>();
    polygon.add(points);
    geoJsonSource.setGeoJson(Polygon.fromLngLats(polygon));
  }

  private Point convertToPoint(LatLng northWest) {
    return Point.fromLngLat(northWest.getLongitude(), northWest.getLatitude());
  }

  private void showCrosshair() {
    View crosshair = new View(this);
    crosshair.setLayoutParams(new FrameLayout.LayoutParams(10, 10, Gravity.CENTER));
    crosshair.setBackgroundColor(Color.BLUE);
    mapView.addView(crosshair);
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
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }
}
