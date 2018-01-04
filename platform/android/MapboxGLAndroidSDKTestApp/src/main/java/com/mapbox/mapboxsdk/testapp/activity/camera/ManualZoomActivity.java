package com.mapbox.mapboxsdk.testapp.activity.camera;

import android.graphics.Point;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing the zoom Camera API.
 * <p>
 * This includes zoomIn, zoomOut, zoomTo, zoomBy (center and custom focal point).
 * </p>
 */
public class ManualZoomActivity extends AppCompatActivity {

  private MapboxMap mapboxMap;
  private MapView mapView;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_manual_zoom);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.setStyleUrl(Style.SATELLITE);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(mapboxMap -> {
      ManualZoomActivity.this.mapboxMap = mapboxMap;

      UiSettings uiSettings = ManualZoomActivity.this.mapboxMap.getUiSettings();
      uiSettings.setAllGesturesEnabled(false);
    });
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_zoom, menu);
    return true;
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {

      case R.id.action_zoom_in:
        mapboxMap.animateCamera(CameraUpdateFactory.zoomIn());
        return true;

      case R.id.action_zoom_out:
        mapboxMap.animateCamera(CameraUpdateFactory.zoomOut());
        return true;

      case R.id.action_zoom_by:
        mapboxMap.animateCamera(CameraUpdateFactory.zoomBy(2));
        return true;
      case R.id.action_zoom_to:
        mapboxMap.animateCamera(CameraUpdateFactory.zoomTo(2));
        return true;

      case R.id.action_zoom_to_point:
        View view = getWindow().getDecorView();
        mapboxMap.animateCamera(
          CameraUpdateFactory.zoomBy(1, new Point(view.getMeasuredWidth() / 4, view.getMeasuredHeight() / 4)));
        return true;

      default:
        return super.onOptionsItemSelected(item);
    }
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
