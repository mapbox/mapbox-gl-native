package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.UiThread;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.services.android.telemetry.location.LocationEngine;
import com.mapbox.services.android.telemetry.permissions.PermissionsManager;

public class CustomLocationEngineActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;
  private FloatingActionButton locationToggleFab;

  private LocationEngine locationServices;

  private static final int PERMISSIONS_LOCATION = 0;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_custom_location_engine);

    locationServices = new MockLocationEngine();

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(MapboxMap map) {
        mapboxMap = map;
        mapboxMap.setLocationSource(locationServices);
      }
    });

    locationToggleFab = (FloatingActionButton) findViewById(R.id.fabLocationToggle);
    locationToggleFab.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view) {
        if (mapboxMap != null) {
          toggleGps(!mapboxMap.isMyLocationEnabled());
        }
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

  @UiThread
  public void toggleGps(boolean enableGps) {
    if (enableGps) {
      if (!PermissionsManager.areLocationPermissionsGranted(this)) {
        ActivityCompat.requestPermissions(this, new String[] {Manifest.permission.ACCESS_COARSE_LOCATION,
          Manifest.permission.ACCESS_FINE_LOCATION}, PERMISSIONS_LOCATION);
      } else {
        enableLocation(true);
      }
    } else {
      enableLocation(false);
    }
  }

  private void enableLocation(boolean enabled) {
    mapboxMap.setMyLocationEnabled(enabled);
    if (enabled) {
      locationToggleFab.setImageResource(R.drawable.ic_location_disabled);
    } else {
      locationToggleFab.setImageResource(R.drawable.ic_my_location);
    }
  }

  @Override
  public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
    if (requestCode == PERMISSIONS_LOCATION) {
      if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
        enableLocation(true);
      }
    }
  }
}
