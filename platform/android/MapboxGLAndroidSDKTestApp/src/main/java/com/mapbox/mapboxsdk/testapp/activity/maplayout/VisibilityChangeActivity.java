package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing visibility changes to the mapview.
 */
public class VisibilityChangeActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;
  private Handler handler = new Handler();
  private Runnable runnable;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_visibility);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(MapboxMap map) {
        mapboxMap = map;
        mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(
          new LatLng(55.754020, 37.620948), 12), 9000);
      }
    });
  }

  @Override
  protected void onStart() {
    super.onStart();
    mapView.onStart();
    handler.post(runnable = new VisibilityRunner(mapView, findViewById(R.id.viewParent), handler));
  }

  @Override
  protected void onResume() {
    super.onResume();
    mapView.onResume();
  }

  private static class VisibilityRunner implements Runnable {

    private MapView mapView;
    private View viewParent;
    private Handler handler;
    private int counter;

    VisibilityRunner(MapView mapView, View viewParent, Handler handler) {
      this.mapView = mapView;
      this.viewParent = viewParent;
      this.handler = handler;
    }

    @Override
    public void run() {
      if (mapView != null && viewParent != null) {
        if (counter == 0 || counter % 2 == 0) {
          viewParent.setVisibility(View.VISIBLE);
          mapView.setVisibility(View.VISIBLE);
        } else {
          if (counter == 1 || counter == 3) {
            mapView.setVisibility(counter == 1 ? View.GONE : View.INVISIBLE);
          } else if (counter == 5 || counter == 7) {
            viewParent.setVisibility(counter == 5 ? View.GONE : View.INVISIBLE);
          }
        }

        // update counter
        if (counter == 7) {
          counter = 0;
        } else {
          counter++;
        }
      }
      handler.postDelayed(this, 1500);
    }
  }

  @Override
  protected void onPause() {
    super.onPause();
    mapView.onPause();
  }

  @Override
  protected void onStop() {
    super.onStop();
    if (runnable != null) {
      handler.removeCallbacks(runnable);
      runnable = null;
    }
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
