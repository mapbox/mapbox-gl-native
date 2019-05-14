package com.mapbox.mapboxsdk.testapp.activity.textureview;

import android.os.Bundle;
import androidx.coordinatorlayout.widget.CoordinatorLayout;
import com.google.android.material.floatingactionbutton.FloatingActionButton;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;
import android.view.View;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test resizing a {@link android.view.TextureView} backed map on the fly.
 */
public class TextureViewResizeActivity extends AppCompatActivity {

  private MapView mapView;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_textureview_resize);
    setupToolbar();
    setupMapView(savedInstanceState);
    setupFab();
  }

  private void setupToolbar() {
    ActionBar actionBar = getSupportActionBar();
    if (actionBar != null) {
      getSupportActionBar().setDisplayHomeAsUpEnabled(true);
      getSupportActionBar().setHomeButtonEnabled(true);
    }
  }

  private void setupMapView(Bundle savedInstanceState) {
    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this::setupMap);
  }

  private void setupMap(MapboxMap mapboxMap) {
    mapboxMap.setStyle(Style.MAPBOX_STREETS);
  }

  private void setupFab() {
    FloatingActionButton fabDebug = findViewById(R.id.fabResize);
    fabDebug.setOnClickListener(view -> {
      if (mapView != null) {
        View parent = findViewById(R.id.coordinator_layout);
        int width = parent.getWidth() == mapView.getWidth() ? parent.getWidth() / 2 : parent.getWidth();
        int height = parent.getHeight() == mapView.getHeight() ? parent.getHeight() / 2 : parent.getHeight();
        mapView.setLayoutParams(new CoordinatorLayout.LayoutParams(width, height));
      }
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