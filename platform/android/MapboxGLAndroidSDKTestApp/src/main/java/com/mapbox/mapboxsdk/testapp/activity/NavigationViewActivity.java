package com.mapbox.mapboxsdk.testapp.activity;

import android.location.Location;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.design.widget.BaseTransientBottomBar;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ProgressBar;
import android.widget.Switch;
import android.widget.Toast;

import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerViewOptions;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.exceptions.InvalidLatLngBoundsException;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.location.LocationSource;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.plugins.locationlayer.LocationLayerMode;
import com.mapbox.mapboxsdk.plugins.locationlayer.LocationLayerPlugin;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.services.Constants;
import com.mapbox.services.android.navigation.ui.v5.NavigationLauncher;
import com.mapbox.services.android.navigation.ui.v5.route.NavigationMapRoute;
import com.mapbox.services.android.navigation.v5.navigation.NavigationRoute;
import com.mapbox.services.android.telemetry.location.LocationEngine;
import com.mapbox.services.android.telemetry.location.LocationEngineListener;
import com.mapbox.services.api.directions.v5.models.DirectionsResponse;
import com.mapbox.services.api.directions.v5.models.DirectionsRoute;
import com.mapbox.services.commons.geojson.LineString;
import com.mapbox.services.commons.models.Position;

import java.util.ArrayList;
import java.util.List;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import timber.log.Timber;

import static com.mapbox.services.android.telemetry.location.LocationEnginePriority.HIGH_ACCURACY;

public class NavigationViewActivity extends AppCompatActivity implements OnMapReadyCallback,
  MapboxMap.OnMapLongClickListener, LocationEngineListener, Callback<DirectionsResponse> {

  private static final int CAMERA_ANIMATION_DURATION = 1000;

  private LocationLayerPlugin locationLayer;
  private LocationEngine locationEngine;
  private NavigationMapRoute mapRoute;
  private MapboxMap mapboxMap;

  private MapView mapView;
  private Button launchRouteBtn;
  private ProgressBar loading;
  private Switch demoSwitch;

  private Marker currentMarker;
  private Position currentPosition;
  private Position destination;
  private DirectionsRoute route;

  private boolean locationFound;
  private boolean shouldSimulateRoute;

  @Override
  protected void onCreate(@Nullable Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_navigation_view);
    bind();
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
    demoSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
      @Override
      public void onCheckedChanged(CompoundButton compoundButton, boolean checked) {
        shouldSimulateRoute = checked;
      }
    });
    launchRouteBtn.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        launchNavigationWithRoute();
      }
    });
  }

  private void bind() {
    mapView = findViewById(R.id.mapView);
    launchRouteBtn = findViewById(R.id.launchRouteBtn);
    loading = findViewById(R.id.loading);
    demoSwitch = findViewById(R.id.demoSwitch);
  }

  @SuppressWarnings({"MissingPermission"})
  @Override
  protected void onStart() {
    super.onStart();
    mapView.onStart();
    if (locationLayer != null) {
      locationLayer.onStart();
    }
  }

  @SuppressWarnings({"MissingPermission"})
  @Override
  public void onResume() {
    super.onResume();
    mapView.onResume();
    if (locationEngine != null) {
      locationEngine.addLocationEngineListener(this);
      if (!locationEngine.isConnected()) {
        locationEngine.activate();
      }
    }
  }

  @Override
  public void onPause() {
    super.onPause();
    mapView.onPause();
    if (locationEngine != null) {
      locationEngine.removeLocationEngineListener(this);
    }
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  protected void onStop() {
    super.onStop();
    mapView.onStop();
    if (locationLayer != null) {
      locationLayer.onStop();
    }
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
    if (locationEngine != null) {
      locationEngine.removeLocationUpdates();
      locationEngine.deactivate();
    }
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }

  @Override
  public void onMapReady(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
    this.mapboxMap.setOnMapLongClickListener(this);
    initLocationEngine();
    initLocationLayer();
    initMapRoute();
  }

  @Override
  public void onMapLongClick(@NonNull LatLng point) {
    destination = Position.fromCoordinates(point.getLongitude(), point.getLatitude());
    launchRouteBtn.setEnabled(false);
    loading.setVisibility(View.VISIBLE);
    setCurrentMarkerPosition(point);
    if (currentPosition != null) {
      fetchRoute();
    }
  }

  @SuppressWarnings({"MissingPermission"})
  @Override
  public void onConnected() {
    locationEngine.requestLocationUpdates();
  }

  @Override
  public void onLocationChanged(Location location) {
    currentPosition = Position.fromCoordinates(location.getLongitude(), location.getLatitude());
    onLocationFound(location);
  }

  @Override
  public void onResponse(Call<DirectionsResponse> call, Response<DirectionsResponse> response) {
    if (validRouteResponse(response)) {
      route = response.body().getRoutes().get(0);
      launchRouteBtn.setEnabled(true);
      mapRoute.addRoute(route);
      boundCameraToRoute();
      hideLoading();
    }
  }

  @Override
  public void onFailure(Call<DirectionsResponse> call, Throwable throwable) {
    Timber.e(throwable.getMessage());
  }

  @SuppressWarnings({"MissingPermission"})
  private void initLocationEngine() {
    locationEngine = new LocationSource(this);
    locationEngine.setPriority(HIGH_ACCURACY);
    locationEngine.setInterval(0);
    locationEngine.setFastestInterval(1000);
    locationEngine.addLocationEngineListener(this);
    locationEngine.activate();

    if (locationEngine.getLastLocation() != null) {
      Location lastLocation = locationEngine.getLastLocation();
      currentPosition = Position.fromCoordinates(lastLocation.getLongitude(), lastLocation.getLatitude());
    }
  }

  @SuppressWarnings({"MissingPermission"})
  private void initLocationLayer() {
    locationLayer = new LocationLayerPlugin(mapView, mapboxMap, locationEngine);
    locationLayer.setLocationLayerEnabled(LocationLayerMode.COMPASS);
  }

  private void initMapRoute() {
    mapRoute = new NavigationMapRoute(mapView, mapboxMap);
  }

  private void fetchRoute() {
    NavigationRoute.builder()
      .accessToken(Mapbox.getAccessToken())
      .origin(currentPosition)
      .destination(destination)
      .build()
      .getRoute(this);
    loading.setVisibility(View.VISIBLE);
  }

  private void launchNavigationWithRoute() {
    if (route != null) {
      NavigationLauncher.startNavigation(this, route,
        null, shouldSimulateRoute);
    }
  }

  private boolean validRouteResponse(Response<DirectionsResponse> response) {
    return response.body() != null
      && response.body().getRoutes() != null
      && response.body().getRoutes().size() > 0;
  }

  private void hideLoading() {
    if (loading.getVisibility() == View.VISIBLE) {
      loading.setVisibility(View.INVISIBLE);
    }
  }

  private void onLocationFound(Location location) {
    if (!locationFound) {
      animateCamera(new LatLng(location.getLatitude(), location.getLongitude()));
      Snackbar.make(mapView, "Long press map to place waypoint", BaseTransientBottomBar.LENGTH_LONG).show();
      locationFound = true;
      hideLoading();
    }
  }

  public void boundCameraToRoute() {
    if (route != null) {
      List<Position> routeCoords = LineString.fromPolyline(route.getGeometry(),
        Constants.PRECISION_6).getCoordinates();
      List<LatLng> bboxPoints = new ArrayList<>();
      for (Position position : routeCoords) {
        bboxPoints.add(new LatLng(position.getLatitude(), position.getLongitude()));
      }
      if (bboxPoints.size() > 1) {
        try {
          LatLngBounds bounds = new LatLngBounds.Builder().includes(bboxPoints).build();
          animateCameraBbox(bounds, CAMERA_ANIMATION_DURATION, new int[] {50, 500, 50, 335});
        } catch (InvalidLatLngBoundsException exception) {
          Toast.makeText(this, "Valid route not found.", Toast.LENGTH_SHORT).show();
        }
      }
    }
  }

  private void animateCameraBbox(LatLngBounds bounds, int animationTime, int[] padding) {
    mapboxMap.animateCamera(CameraUpdateFactory.newLatLngBounds(bounds,
      padding[0], padding[1], padding[2], padding[3]), animationTime);
  }

  private void animateCamera(LatLng point) {
    mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(point, 16), CAMERA_ANIMATION_DURATION);
  }

  private void setCurrentMarkerPosition(LatLng position) {
    if (position != null) {
      if (currentMarker == null) {
        MarkerViewOptions markerViewOptions = new MarkerViewOptions()
          .position(position);
        currentMarker = mapboxMap.addMarker(markerViewOptions);
      } else {
        currentMarker.setPosition(position);
      }
    }
  }
}

