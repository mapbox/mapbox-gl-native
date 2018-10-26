package com.mapbox.mapboxsdk.testapp.activity.location;

import android.location.Location;
import android.os.Bundle;
import android.os.Looper;
import android.support.annotation.NonNull;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

import com.mapbox.android.core.location.LocationEngine;
import com.mapbox.android.core.location.LocationEngineCallback;
import com.mapbox.android.core.location.LocationEngineProvider;
import com.mapbox.android.core.location.LocationEngineRequest;
import com.mapbox.android.core.location.LocationEngineResult;
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
        LocationEngineCallback<LocationEngineResult> {

  private static final long DEFAULT_INTERVAL_MILLIS = 1000;
  private static final long DEFAULT_FASTEST_INTERVAL_MILLIS = 1000;

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
  public void onMapReady(@NonNull MapboxMap mapboxMap) {
    locationEngine = LocationEngineProvider.getBestLocationEngine(getApplicationContext(),
            false);
    locationComponent = mapboxMap.getLocationComponent();
    locationComponent.activateLocationComponent(this, locationEngine);
    locationComponent.setLocationComponentEnabled(true);
    locationComponent.setRenderMode(RenderMode.COMPASS);
  }

  private static LocationEngineRequest getLocationRequst(long interval) {
    return new LocationEngineRequest.Builder(interval)
            .setFastestInterval(DEFAULT_FASTEST_INTERVAL_MILLIS)
            .setPriority(LocationEngineRequest.PRIORITY_HIGH_ACCURACY)
            .build();
  }

  @Override
  protected void onStart() {
    super.onStart();
    mapView.onStart();
    if (locationEngine != null) {
      try {
        locationEngine.requestLocationUpdates(getLocationRequst(DEFAULT_INTERVAL_MILLIS),
                this, Looper.getMainLooper());
      } catch (SecurityException se) {
        se.printStackTrace();
      }
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
      locationEngine.removeLocationUpdates(this);
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
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  public void onSuccess(LocationEngineResult result) {
    Location location = result.getLastLocation();
    if (location != null) {
      Timber.d("Location change occurred: %s", location.toString());
    }
  }

  @Override
  public void onFailure(@NonNull Exception exception) {
    Timber.d("Location engine error occurred: %s", exception.getMessage());
  }
}