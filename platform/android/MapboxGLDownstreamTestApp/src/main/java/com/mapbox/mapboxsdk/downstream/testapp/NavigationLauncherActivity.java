package com.mapbox.mapboxsdk.downstream.testapp;

import android.content.Intent;
import android.content.SharedPreferences;
import android.location.Location;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ProgressBar;
import android.widget.Toast;

import com.mapbox.android.core.location.LocationEngine;
import com.mapbox.android.core.location.LocationEngineCallback;
import com.mapbox.android.core.location.LocationEngineProvider;
import com.mapbox.android.core.location.LocationEngineRequest;
import com.mapbox.android.core.location.LocationEngineResult;
import com.mapbox.api.directions.v5.DirectionsCriteria;
import com.mapbox.api.directions.v5.models.DirectionsResponse;
import com.mapbox.api.directions.v5.models.DirectionsRoute;
import com.mapbox.core.constants.Constants;
import com.mapbox.geojson.LineString;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.exceptions.InvalidLatLngBoundsException;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.location.LocationComponent;
import com.mapbox.mapboxsdk.location.modes.RenderMode;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.services.android.navigation.ui.v5.NavigationLauncher;
import com.mapbox.services.android.navigation.ui.v5.NavigationLauncherOptions;
import com.mapbox.services.android.navigation.ui.v5.route.NavigationMapRoute;
import com.mapbox.services.android.navigation.ui.v5.route.OnRouteSelectionChangeListener;
import com.mapbox.services.android.navigation.v5.navigation.NavigationRoute;
import com.mapbox.services.android.navigation.v5.utils.LocaleUtils;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
import retrofit2.Call;
import retrofit2.Response;
import timber.log.Timber;

public class NavigationLauncherActivity extends AppCompatActivity implements OnMapReadyCallback,
  MapboxMap.OnMapLongClickListener, OnRouteSelectionChangeListener {

  private static final int CAMERA_ANIMATION_DURATION = 1000;
  private static final int DEFAULT_CAMERA_ZOOM = 16;
  private static final int CHANGE_SETTING_REQUEST_CODE = 1;
  private static final int INITIAL_ZOOM = 16;
  private static final long UPDATE_INTERVAL_IN_MILLISECONDS = 1000;
  private static final long FASTEST_UPDATE_INTERVAL_IN_MILLISECONDS = 500;

  private final NavigationLauncherLocationCallback callback = new NavigationLauncherLocationCallback(this);
  private LocationEngine locationEngine;
  private NavigationMapRoute mapRoute;
  private MapboxMap mapboxMap;
  private Marker currentMarker;
  private Point currentLocation;
  private Point destination;
  private DirectionsRoute route;
  private LocaleUtils localeUtils;
  private boolean locationFound;

  @BindView(R.id.mapView)
  MapView mapView;
  @BindView(R.id.launch_route_btn)
  Button launchRouteBtn;
  @BindView(R.id.loading)
  ProgressBar loading;
  @BindView(R.id.launch_btn_frame)
  FrameLayout launchBtnFrame;

  @Override
  protected void onCreate(@Nullable Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    Mapbox.getInstance(getApplicationContext(), getString(R.string.mapbox_access_token));
    setContentView(R.layout.activity_navigation_launcher);
    ButterKnife.bind(this);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
    localeUtils = new LocaleUtils();
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    // Inflate the menu; this adds items to the action bar if it is present.
    getMenuInflater().inflate(R.menu.navigation_view_activity_menu, menu);
    return true;
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {
      case R.id.settings:
        showSettings();
        return true;
      default:
        return super.onOptionsItemSelected(item);
    }
  }

  @Override
  protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    super.onActivityResult(requestCode, resultCode, data);
    if (requestCode == CHANGE_SETTING_REQUEST_CODE && resultCode == RESULT_OK) {
      boolean shouldRefetch = data.getBooleanExtra(NavigationSettingsActivity.UNIT_TYPE_CHANGED, false)
        || data.getBooleanExtra(NavigationSettingsActivity.LANGUAGE_CHANGED, false);
      if (destination != null && shouldRefetch) {
        fetchRoute();
      }
    }
  }

  @Override
  protected void onStart() {
    super.onStart();
    mapView.onStart();
  }

  @SuppressWarnings( {"MissingPermission"})
  @Override
  public void onResume() {
    super.onResume();
    mapView.onResume();
    if (locationEngine != null) {
      locationEngine.requestLocationUpdates(buildEngineRequest(), callback, null);
    }
  }

  @Override
  public void onPause() {
    super.onPause();
    mapView.onPause();
    if (locationEngine != null) {
      locationEngine.removeLocationUpdates(callback);
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

  @OnClick(R.id.launch_route_btn)
  public void onRouteLaunchClick() {
    launchNavigationWithRoute();
  }

  @Override
  public void onMapReady(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
    mapboxMap.setStyle(Style.MAPBOX_STREETS, style -> {
      mapboxMap.addOnMapLongClickListener(this);
      initializeLocationEngine();
      initializeLocationComponent(style);
      initializeMapRoute();
    });
  }

  @Override
  public boolean onMapLongClick(@NonNull LatLng point) {
    destination = Point.fromLngLat(point.getLongitude(), point.getLatitude());
    launchRouteBtn.setEnabled(false);
    loading.setVisibility(View.VISIBLE);
    setCurrentMarkerPosition(point);
    if (currentLocation != null) {
      fetchRoute();
    }
    return false;
  }

  @Override
  public void onNewPrimaryRouteSelected(DirectionsRoute directionsRoute) {
    route = directionsRoute;
  }

  void updateCurrentLocation(Point currentLocation) {
    this.currentLocation = currentLocation;
  }

  void onLocationFound(Location location) {
    if (!locationFound) {
      animateCamera(new LatLng(location.getLatitude(), location.getLongitude()));
      Snackbar.make(mapView, R.string.explanation_long_press_waypoint, Snackbar.LENGTH_LONG).show();
      locationFound = true;
      hideLoading();
    }
  }

  private void showSettings() {
    startActivityForResult(new Intent(this, NavigationSettingsActivity.class), CHANGE_SETTING_REQUEST_CODE);
  }

  @SuppressWarnings( {"MissingPermission"})
  private void initializeLocationEngine() {
    locationEngine = LocationEngineProvider.getBestLocationEngine(getApplicationContext());
    LocationEngineRequest request = buildEngineRequest();
    locationEngine.requestLocationUpdates(request, callback, null);
    locationEngine.getLastLocation(callback);
  }

  @SuppressWarnings( {"MissingPermission"})
  private void initializeLocationComponent(Style style) {
    LocationComponent locationComponent = mapboxMap.getLocationComponent();
    locationComponent.activateLocationComponent(this, style, locationEngine);
    locationComponent.setLocationComponentEnabled(true);
    locationComponent.setRenderMode(RenderMode.COMPASS);
  }

  private void initializeMapRoute() {
    mapRoute = new NavigationMapRoute(mapView, mapboxMap);
    mapRoute.setOnRouteSelectionChangeListener(this);
  }

  private void fetchRoute() {
    NavigationRoute.Builder builder = NavigationRoute.builder(this)
      .accessToken(Mapbox.getAccessToken())
      .origin(currentLocation)
      .destination(destination)
      .profile(getRouteProfileFromSharedPreferences())
      .alternatives(true);
    setFieldsFromSharedPreferences(builder);
    builder.build()
      .getRoute(new SimplifiedCallback() {
        @Override
        public void onResponse(Call<DirectionsResponse> call, Response<DirectionsResponse> response) {
          if (validRouteResponse(response)) {
            hideLoading();
            route = response.body().routes().get(0);
            if (route.distance() > 25d) {
              launchRouteBtn.setEnabled(true);
              mapRoute.addRoutes(response.body().routes());
              boundCameraToRoute();
            } else {
              Snackbar.make(mapView, R.string.error_select_longer_route, Snackbar.LENGTH_SHORT).show();
            }
          }
        }
      });
    loading.setVisibility(View.VISIBLE);
  }

  private void setFieldsFromSharedPreferences(NavigationRoute.Builder builder) {
    builder
      .language(getLanguageFromSharedPreferences())
      .voiceUnits(getUnitTypeFromSharedPreferences());
  }

  private String getUnitTypeFromSharedPreferences() {
    SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
    String defaultUnitType = getString(R.string.default_unit_type);
    String unitType = sharedPreferences.getString(getString(R.string.unit_type_key), defaultUnitType);
    if (unitType.equals(defaultUnitType)) {
      unitType = localeUtils.getUnitTypeForDeviceLocale(this);
    }

    return unitType;
  }

  private Locale getLanguageFromSharedPreferences() {
    SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
    String defaultLanguage = getString(R.string.default_locale);
    String language = sharedPreferences.getString(getString(R.string.language_key), defaultLanguage);
    if (language.equals(defaultLanguage)) {
      return localeUtils.inferDeviceLocale(this);
    } else {
      return new Locale(language);
    }
  }

  private boolean getShouldSimulateRouteFromSharedPreferences() {
    SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
    return sharedPreferences.getBoolean(getString(R.string.simulate_route_key), false);
  }

  private String getRouteProfileFromSharedPreferences() {
    SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
    return sharedPreferences.getString(
      getString(R.string.route_profile_key), DirectionsCriteria.PROFILE_DRIVING_TRAFFIC
    );
  }

  private void launchNavigationWithRoute() {
    if (route == null) {
      Snackbar.make(mapView, R.string.error_route_not_available, Snackbar.LENGTH_SHORT).show();
      return;
    }

    NavigationLauncherOptions.Builder optionsBuilder = NavigationLauncherOptions.builder()
      .shouldSimulateRoute(getShouldSimulateRouteFromSharedPreferences());
    CameraPosition initialPosition = new CameraPosition.Builder()
      .target(new LatLng(currentLocation.latitude(), currentLocation.longitude()))
      .zoom(INITIAL_ZOOM)
      .build();
    optionsBuilder.initialMapCameraPosition(initialPosition);
    optionsBuilder.directionsRoute(route);
    NavigationLauncher.startNavigation(this, optionsBuilder.build());
  }

  private boolean validRouteResponse(Response<DirectionsResponse> response) {
    return response.body() != null && !response.body().routes().isEmpty();
  }

  private void hideLoading() {
    if (loading.getVisibility() == View.VISIBLE) {
      loading.setVisibility(View.INVISIBLE);
    }
  }

  public void boundCameraToRoute() {
    if (route != null) {
      List<Point> routeCoords = LineString.fromPolyline(route.geometry(),
        Constants.PRECISION_6).coordinates();
      List<LatLng> bboxPoints = new ArrayList<>();
      for (Point point : routeCoords) {
        bboxPoints.add(new LatLng(point.latitude(), point.longitude()));
      }
      if (bboxPoints.size() > 1) {
        try {
          LatLngBounds bounds = new LatLngBounds.Builder().includes(bboxPoints).build();
          // left, top, right, bottom
          int topPadding = launchBtnFrame.getHeight() * 2;
          animateCameraBbox(bounds, CAMERA_ANIMATION_DURATION, new int[] {50, topPadding, 50, 100});
        } catch (InvalidLatLngBoundsException exception) {
          Toast.makeText(this, R.string.error_valid_route_not_found, Toast.LENGTH_SHORT).show();
        }
      }
    }
  }

  private void animateCameraBbox(LatLngBounds bounds, int animationTime, int[] padding) {
    CameraPosition position = mapboxMap.getCameraForLatLngBounds(bounds, padding);
    mapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(position), animationTime);
  }

  private void animateCamera(LatLng point) {
    mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(point, DEFAULT_CAMERA_ZOOM), CAMERA_ANIMATION_DURATION);
  }

  private void setCurrentMarkerPosition(LatLng position) {
    if (position != null) {
      if (currentMarker == null) {
        MarkerOptions markerViewOptions = new MarkerOptions()
          .position(position);
        currentMarker = mapboxMap.addMarker(markerViewOptions);
      } else {
        currentMarker.setPosition(position);
      }
    }
  }

  @NonNull
  private LocationEngineRequest buildEngineRequest() {
    return new LocationEngineRequest.Builder(UPDATE_INTERVAL_IN_MILLISECONDS)
      .setPriority(LocationEngineRequest.PRIORITY_HIGH_ACCURACY)
      .setFastestInterval(FASTEST_UPDATE_INTERVAL_IN_MILLISECONDS)
      .build();
  }

  private static class NavigationLauncherLocationCallback implements LocationEngineCallback<LocationEngineResult> {

    private final WeakReference<NavigationLauncherActivity> activityWeakReference;

    NavigationLauncherLocationCallback(NavigationLauncherActivity activity) {
      this.activityWeakReference = new WeakReference<>(activity);
    }

    @Override
    public void onSuccess(LocationEngineResult result) {
      NavigationLauncherActivity activity = activityWeakReference.get();
      if (activity != null) {
        Location location = result.getLastLocation();
        if (location == null) {
          return;
        }
        activity.updateCurrentLocation(Point.fromLngLat(location.getLongitude(), location.getLatitude()));
        activity.onLocationFound(location);
      }
    }

    @Override
    public void onFailure(@NonNull Exception exception) {
      Timber.e(exception);
    }
  }
}
