package com.mapbox.mapboxsdk.testapp.activity.location;

import android.annotation.SuppressLint;
import android.os.Bundle;
import androidx.annotation.NonNull;
import com.google.android.material.floatingactionbutton.FloatingActionButton;
import androidx.appcompat.app.AppCompatActivity;
import android.widget.Toast;

import com.mapbox.android.core.location.LocationEngine;
import com.mapbox.android.core.location.LocationEngineProvider;
import com.mapbox.android.core.location.LocationEngineRequest;
import com.mapbox.android.core.permissions.PermissionsListener;
import com.mapbox.android.core.permissions.PermissionsManager;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.location.LocationComponent;
import com.mapbox.mapboxsdk.location.LocationComponentActivationOptions;
import com.mapbox.mapboxsdk.location.modes.RenderMode;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.List;

public class ManualLocationUpdatesActivity extends AppCompatActivity implements OnMapReadyCallback {

  private MapView mapView;
  private LocationComponent locationComponent;
  private LocationEngine locationEngine;
  private PermissionsManager permissionsManager;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_location_manual_update);

    locationEngine = LocationEngineProvider.getBestLocationEngine(this, false);

    FloatingActionButton fabManualUpdate = findViewById(R.id.fabManualLocationChange);
    fabManualUpdate.setOnClickListener(v -> {
      if (locationComponent != null && locationComponent.getLocationEngine() == null) {
        locationComponent.forceLocationUpdate(
          Utils.getRandomLocation(LatLngBounds.from(60, 25, 40, -5)));
      }
    });
    fabManualUpdate.setEnabled(false);

    FloatingActionButton fabToggle = findViewById(R.id.fabToggleManualLocation);
    fabToggle.setOnClickListener(v -> {
      if (locationComponent != null) {
        locationComponent.setLocationEngine(locationComponent.getLocationEngine() == null ? locationEngine : null);

        if (locationComponent.getLocationEngine() == null) {
          fabToggle.setImageResource(R.drawable.ic_layers_clear);
          fabManualUpdate.setEnabled(true);
          fabManualUpdate.setAlpha(1f);
          Toast.makeText(
            ManualLocationUpdatesActivity.this.getApplicationContext(),
            "LocationEngine disabled, use manual updates",
            Toast.LENGTH_SHORT).show();
        } else {
          fabToggle.setImageResource(R.drawable.ic_layers);
          fabManualUpdate.setEnabled(false);
          fabManualUpdate.setAlpha(0.5f);
          Toast.makeText(
            ManualLocationUpdatesActivity.this.getApplicationContext(),
            "LocationEngine enabled",
            Toast.LENGTH_SHORT).show();
        }
      }
    });

    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);

    if (PermissionsManager.areLocationPermissionsGranted(this)) {
      mapView.getMapAsync(this);
    } else {
      permissionsManager = new PermissionsManager(new PermissionsListener() {
        @Override
        public void onExplanationNeeded(List<String> permissionsToExplain) {
          Toast.makeText(ManualLocationUpdatesActivity.this.getApplicationContext(),
            "You need to accept location permissions.",
            Toast.LENGTH_SHORT
          ).show();
        }

        @Override
        public void onPermissionResult(boolean granted) {
          if (granted) {
            mapView.getMapAsync(ManualLocationUpdatesActivity.this);
          } else {
            finish();
          }
        }
      });
      permissionsManager.requestLocationPermissions(this);
    }
  }

  @Override
  public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
    super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    permissionsManager.onRequestPermissionsResult(requestCode, permissions, grantResults);
  }

  @SuppressLint("MissingPermission")
  @Override
  public void onMapReady(@NonNull MapboxMap mapboxMap) {
    mapboxMap.setStyle(new Style.Builder().fromUri(Style.MAPBOX_STREETS), style -> {
      locationComponent = mapboxMap.getLocationComponent();

      locationComponent.activateLocationComponent(
        LocationComponentActivationOptions
          .builder(this, style)
          .locationEngine(locationEngine)
          .locationEngineRequest(new LocationEngineRequest.Builder(500)
            .setFastestInterval(500)
            .setPriority(LocationEngineRequest.PRIORITY_HIGH_ACCURACY).build())
          .build());

      locationComponent.setLocationComponentEnabled(true);
      locationComponent.setRenderMode(RenderMode.COMPASS);
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