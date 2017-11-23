package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
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
    mapView.getMapAsync(map -> {
      mapboxMap = map;
      mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(
        new LatLng(55.754020, 37.620948), 12), 9000);
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
    private int currentStep;

    VisibilityRunner(MapView mapView, View viewParent, Handler handler) {
      this.mapView = mapView;
      this.viewParent = viewParent;
      this.handler = handler;
    }

    @Override
    public void run() {
      if (isViewHiearchyReady()) {
        if (isEvenStep()) {
          viewParent.setVisibility(View.VISIBLE);
          mapView.setVisibility(View.VISIBLE);
        } else if (isFirstOrThirdStep()) {
          mapView.setVisibility(getVisibilityForStep());
        } else if (isFifthOrSeventhStep()) {
          viewParent.setVisibility(getVisibilityForStep());
        }
        updateStep();
      }
      handler.postDelayed(this, 1500);
    }

    private void updateStep() {
      if (currentStep == 7) {
        currentStep = 0;
      } else {
        currentStep++;
      }
    }

    private int getVisibilityForStep() {
      return (currentStep == 1 || currentStep == 5) ? View.GONE : View.INVISIBLE;
    }

    private boolean isFifthOrSeventhStep() {
      return currentStep == 5 || currentStep == 7;
    }

    private boolean isFirstOrThirdStep() {
      return currentStep == 1 || currentStep == 3;
    }

    private boolean isEvenStep() {
      return currentStep == 0 || currentStep % 2 == 0;
    }

    private boolean isViewHiearchyReady() {
      return mapView != null && viewParent != null;
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
