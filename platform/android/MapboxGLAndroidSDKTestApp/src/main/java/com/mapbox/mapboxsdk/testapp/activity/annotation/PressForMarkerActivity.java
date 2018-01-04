package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.graphics.PointF;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;

import java.text.DecimalFormat;
import java.util.ArrayList;

/**
 * Test activity showcasing to add a Marker on click.
 * <p>
 * Shows how to use a OnMapClickListener and a OnMapLongClickListener
 * </p>
 */
public class PressForMarkerActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;
  private ArrayList<MarkerOptions> markerList = new ArrayList<>();

  private static final DecimalFormat LAT_LON_FORMATTER = new DecimalFormat("#.#####");

  private static String STATE_MARKER_LIST = "markerList";

  @Override
  protected void onCreate(@Nullable final Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_press_for_marker);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;
      resetMap();

      mapboxMap.setOnMapLongClickListener(point -> addMarker(point));

      mapboxMap.setOnMapClickListener(point -> addMarker(point));

      if (savedInstanceState != null) {
        markerList = savedInstanceState.getParcelableArrayList(STATE_MARKER_LIST);
        mapboxMap.addMarkers(markerList);
      }
    });
  }

  private void addMarker(LatLng point) {
    final PointF pixel = mapboxMap.getProjection().toScreenLocation(point);

    String title = LAT_LON_FORMATTER.format(point.getLatitude()) + ", "
      + LAT_LON_FORMATTER.format(point.getLongitude());
    String snippet = "X = " + (int) pixel.x + ", Y = " + (int) pixel.y;

    MarkerOptions marker = new MarkerOptions()
      .position(point)
      .title(title)
      .snippet(snippet);

    markerList.add(marker);
    mapboxMap.addMarker(marker);
  }

  private void resetMap() {
    if (mapboxMap == null) {
      return;
    }
    mapboxMap.removeAnnotations();
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_press_for_marker, menu);
    return true;
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);

    mapView.onSaveInstanceState(outState);
    outState.putParcelableArrayList(STATE_MARKER_LIST, markerList);
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
  protected void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {
      case R.id.menuItemReset:
        resetMap();
        return true;
      default:
        return super.onOptionsItemSelected(item);
    }
  }
}
