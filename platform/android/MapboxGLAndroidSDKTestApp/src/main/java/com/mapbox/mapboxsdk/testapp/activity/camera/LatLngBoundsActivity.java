package com.mapbox.mapboxsdk.testapp.activity.camera;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomSheetBehavior;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.http.HttpRequestUtil;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.view.LockableBottomSheetBehavior;

import java.util.ArrayList;
import java.util.List;

/**
 * Test activity showcasing using the LatLngBounds camera API.
 */
public class LatLngBoundsActivity extends AppCompatActivity implements View.OnClickListener {

  private static final List<LatLng> LOCATIONS = new ArrayList<LatLng>() {
    {
      add(new LatLng(37.806866, -122.422502));
      add(new LatLng(37.812905, -122.477605));
      add(new LatLng(37.826944, -122.423188));
      add(new LatLng(37.752676, -122.447736));
      add(new LatLng(37.769305, -122.479322));
      add(new LatLng(37.749834, -122.417867));
      add(new LatLng(37.756149, -122.405679));
      add(new LatLng(37.751403, -122.387397));
      add(new LatLng(37.793064, -122.391517));
      add(new LatLng(37.769122, -122.427394));
    }
  };
  private static final LatLngBounds BOUNDS = new LatLngBounds.Builder().includes(LOCATIONS).build();
  private static final int ANIMATION_DURATION_LONG = 450;
  private static final int ANIMATION_DURATION_SHORT = 250;
  private static final int BOUNDS_PADDING_DIVIDER_SMALL = 3;
  private static final int BOUNDS_PADDING_DIVIDER_LARGE = 9;

  private MapView mapView;
  private MapboxMap mapboxMap;
  private View bottomSheet;
  private LockableBottomSheetBehavior bottomSheetBehavior;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    HttpRequestUtil.setLogEnabled(false);
    setContentView(R.layout.activity_latlngbounds);
    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;
      initMap();
    });
  }

  private void initMap() {
    addMarkers();
    initFab();
    initBottomSheet();
    moveToBounds(bottomSheet.getMeasuredHeight(), BOUNDS_PADDING_DIVIDER_SMALL, 0);
  }

  private void addMarkers() {
    for (LatLng location : LOCATIONS) {
      mapboxMap.addMarker(new MarkerOptions().position(location));
    }
  }

  private void initFab() {
    findViewById(R.id.fab).setOnClickListener(this);
  }

  @Override
  public void onClick(View v) {
    bottomSheetBehavior.setState(BottomSheetBehavior.STATE_HIDDEN);
    v.animate().alpha(0.0f).setDuration(ANIMATION_DURATION_SHORT);
  }

  private void initBottomSheet() {
    bottomSheet = findViewById(R.id.bottom_sheet);
    bottomSheetBehavior = (LockableBottomSheetBehavior) BottomSheetBehavior.from(bottomSheet);
    bottomSheetBehavior.setLocked(true);
    bottomSheetBehavior.setBottomSheetCallback(new BottomSheetBehavior.BottomSheetCallback() {
      @Override
      public void onStateChanged(@NonNull View bottomSheet, int newState) {
        if (newState == BottomSheetBehavior.STATE_SETTLING) {
          moveToBounds(0, BOUNDS_PADDING_DIVIDER_LARGE, ANIMATION_DURATION_LONG);
        }
      }

      @Override
      public void onSlide(@NonNull View bottomSheet, float slideOffset) {

      }
    });
  }

  private void moveToBounds(int verticalOffset, int boundsPaddingDivider, int duration) {
    int paddingHorizontal = mapView.getMeasuredWidth() / boundsPaddingDivider;
    int paddingVertical = (mapView.getMeasuredHeight() - verticalOffset) / boundsPaddingDivider;
    mapboxMap.animateCamera(CameraUpdateFactory.newLatLngBounds(
      BOUNDS,
      paddingHorizontal,
      paddingVertical,
      paddingHorizontal,
      paddingVertical + verticalOffset),
      duration
    );
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
    HttpRequestUtil.setLogEnabled(true);
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }
}
