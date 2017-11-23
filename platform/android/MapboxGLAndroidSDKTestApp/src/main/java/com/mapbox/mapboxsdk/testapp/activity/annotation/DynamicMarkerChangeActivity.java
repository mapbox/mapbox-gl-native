package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.content.ContextCompat;
import android.support.v4.content.res.ResourcesCompat;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.IconUtils;

/**
 * Test activity showcasing updating a Marker position, title, icon and snippet.
 */
public class DynamicMarkerChangeActivity extends AppCompatActivity {

  private static final LatLng LAT_LNG_CHELSEA = new LatLng(51.481670, -0.190849);
  private static final LatLng LAT_LNG_ARSENAL = new LatLng(51.555062, -0.108417);

  private MapView mapView;
  private MapboxMap mapboxMap;
  private Marker marker;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_dynamic_marker);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.setTag(false);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(mapboxMap -> {
      DynamicMarkerChangeActivity.this.mapboxMap = mapboxMap;
      // Create marker
      MarkerOptions markerOptions = new MarkerOptions()
        .position(LAT_LNG_CHELSEA)
        .icon(IconUtils.drawableToIcon(DynamicMarkerChangeActivity.this, R.drawable.ic_stars,
          ResourcesCompat.getColor(getResources(), R.color.blueAccent, getTheme())))
        .title(getString(R.string.dynamic_marker_chelsea_title))
        .snippet(getString(R.string.dynamic_marker_chelsea_snippet));
      marker = mapboxMap.addMarker(markerOptions);
    });

    FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
    fab.setColorFilter(ContextCompat.getColor(this, R.color.primary));
    fab.setOnClickListener(view -> {
      if (mapboxMap != null) {
        updateMarker();
      }
    });
  }

  private void updateMarker() {
    // update model
    boolean first = (boolean) mapView.getTag();
    mapView.setTag(!first);

    // update marker
    marker.setPosition(first ? LAT_LNG_CHELSEA : LAT_LNG_ARSENAL);
    marker.setIcon(IconUtils.drawableToIcon(this, R.drawable.ic_stars, first
      ? ResourcesCompat.getColor(getResources(), R.color.blueAccent, getTheme()) :
      ResourcesCompat.getColor(getResources(), R.color.redAccent, getTheme())
    ));

    marker.setTitle(first
      ? getString(R.string.dynamic_marker_chelsea_title) : getString(R.string.dynamic_marker_arsenal_title));
    marker.setSnippet(first
      ? getString(R.string.dynamic_marker_chelsea_snippet) : getString(R.string.dynamic_marker_arsenal_snippet));
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
