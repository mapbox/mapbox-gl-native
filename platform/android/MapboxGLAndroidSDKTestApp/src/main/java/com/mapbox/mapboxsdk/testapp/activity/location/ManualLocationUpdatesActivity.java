package com.mapbox.mapboxsdk.testapp.activity.location;

import android.location.Location;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

import com.mapbox.android.core.location.LocationEngine;
import com.mapbox.android.core.location.LocationEngineListener;
import com.mapbox.android.core.location.LocationEnginePriority;
import com.mapbox.android.core.location.LocationEngineProvider;
import com.mapbox.android.core.permissions.PermissionsListener;
import com.mapbox.android.core.permissions.PermissionsManager;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.location.LocationComponent;
import com.mapbox.mapboxsdk.location.modes.RenderMode;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.List;

import timber.log.Timber;

public class ManualLocationUpdatesActivity extends AppCompatActivity implements OnMapReadyCallback,
  LocationEngineListener {

  private MapView mapView;
  private LocationComponent locationComponent;
  private LocationEngine locationEngine;
  private PermissionsManager permissionsManager;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_location_manual_update);

    FloatingActionButton fabManualUpdate = findViewById(R.id.fabManualLocationChange);
    fabManualUpdate.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        if (locationComponent != null && locationComponent.getLocationEngine() == null) {
          locationComponent.forceLocationUpdate(
            Utils.getRandomLocation(LatLngBounds.from(60, 25, 40, -5)));
        }
      }
    });
    fabManualUpdate.setEnabled(false);

    FloatingActionButton fabToggle = findViewById(R.id.fabToggleManualLocation);
    fabToggle.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        if (locationComponent != null) {
          locationComponent.setLocationEngine(locationComponent.getLocationEngine() == null ? locationEngine :
            null);

          if (locationComponent.getLocationEngine() == null) {
            fabToggle.setImageResource(R.drawable.ic_layers_clear);
            fabManualUpdate.setEnabled(true);
            fabManualUpdate.setAlpha(1f);
            Toast.makeText(
              ManualLocationUpdatesActivity.this.getApplicationContext(),
              "LocationEngine disable, use manual updates",
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

  @Override
  public void onMapReady(MapboxMap mapboxMap) {
    locationEngine = new LocationEngineProvider(this).obtainBestLocationEngineAvailable();
    locationEngine.addLocationEngineListener(this);
    locationEngine.setPriority(LocationEnginePriority.HIGH_ACCURACY);
    locationEngine.activate();
    locationComponent = mapboxMap.getLocationComponent();
    locationComponent.activateLocationComponent(this, locationEngine);
    locationComponent.setLocationComponentEnabled(true);
    locationComponent.setRenderMode(RenderMode.COMPASS);
  }

  @Override
  @SuppressWarnings( {"MissingPermission"})
  public void onConnected() {
    locationEngine.requestLocationUpdates();
  }

  @Override
  public void onLocationChanged(Location location) {
    Timber.d("Location change occurred: %s", location.toString());
  }

  @Override
  @SuppressWarnings( {"MissingPermission"})
  protected void onStart() {
    super.onStart();
    mapView.onStart();
    if (locationEngine != null) {
      locationEngine.requestLocationUpdates();
      locationEngine.addLocationEngineListener(this);
    }
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
    if (locationEngine != null) {
      locationEngine.removeLocationEngineListener(this);
      locationEngine.removeLocationUpdates();
    }
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
    if (locationEngine != null) {
      locationEngine.deactivate();
    }
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }
}