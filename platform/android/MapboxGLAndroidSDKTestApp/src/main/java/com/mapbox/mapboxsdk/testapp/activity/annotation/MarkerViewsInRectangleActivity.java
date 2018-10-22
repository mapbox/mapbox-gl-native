package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.graphics.RectF;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.annotations.MarkerViewOptions;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.List;

import timber.log.Timber;

/**
 * Test activity showcasing counting MarkerViews in a rectangle.
 */
public class MarkerViewsInRectangleActivity extends AppCompatActivity implements OnMapReadyCallback,
  View.OnClickListener {

  public MapView mapView;
  private MapboxMap mapboxMap;
  private View selectionBox;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_marker_view_in_rect);

    selectionBox = findViewById(R.id.selection_box);

    // Initialize map as normal
    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
  }

  @Override
  public void onMapReady(MapboxMap mapboxMap) {
    MarkerViewsInRectangleActivity.this.mapboxMap = mapboxMap;
    selectionBox.setOnClickListener(this);
    mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(52.0907, 5.1214), 16));
    mapboxMap.addMarker(new MarkerViewOptions().position(new LatLng(52.0907, 5.1214)));
  }

  @Override
  public void onClick(View view) {
    // Query
    int top = selectionBox.getTop() - mapView.getTop();
    int left = selectionBox.getLeft() - mapView.getLeft();
    RectF box = new RectF(left, top, left + selectionBox.getWidth(), top + selectionBox.getHeight());
    Timber.i("Querying box %s", box);
    List<MarkerView> markers = mapboxMap.getMarkerViewsInRect(box);

    // Show count
    Toast.makeText(
      MarkerViewsInRectangleActivity.this,
      String.format("%s markers inside box", markers.size()),
      Toast.LENGTH_SHORT).show();
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
