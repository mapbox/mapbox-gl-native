package com.mapbox.mapboxsdk.downstream.testapp;


import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.view.View;
import android.widget.ProgressBar;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;
import com.mapbox.api.directions.v5.models.DirectionsResponse;
import com.mapbox.api.directions.v5.models.DirectionsRoute;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.LocationComponent;
import com.mapbox.mapboxsdk.location.modes.CameraMode;
import com.mapbox.mapboxsdk.location.modes.RenderMode;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.services.android.navigation.ui.v5.route.NavigationMapRoute;
import com.mapbox.services.android.navigation.v5.navigation.NavigationRoute;

import java.util.List;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import timber.log.Timber;

public class NavigationMapRouteActivity extends AppCompatActivity implements OnMapReadyCallback,
  MapboxMap.OnMapLongClickListener, Callback<DirectionsResponse> {

  private static final int ONE_HUNDRED_MILLISECONDS = 100;

  @BindView(R.id.mapView)
  MapView mapView;
  @BindView(R.id.routeLoadingProgressBar)
  ProgressBar routeLoading;
  @BindView(R.id.fabRemoveRoute)
  FloatingActionButton fabRemoveRoute;

  private MapboxMap mapboxMap;
  private NavigationMapRoute navigationMapRoute;
  private StyleCycle styleCycle = new StyleCycle();

  private Marker originMarker;
  private Marker destinationMarker;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    Mapbox.getInstance(this, "YOUR_ACCESS_TOKEN");
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_navigation_map_route);
    ButterKnife.bind(this);

    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
  }

  @OnClick(R.id.fabStyles)
  public void onStyleFabClick() {
    if (mapboxMap != null) {
      mapboxMap.setStyle(styleCycle.getNextStyle());
    }
  }

  @OnClick(R.id.fabRemoveRoute)
  public void onRemoveRouteClick(View fabRemoveRoute) {
    removeRouteAndMarkers();
    fabRemoveRoute.setVisibility(View.INVISIBLE);
  }

  @Override
  public void onMapReady(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
    mapboxMap.setStyle(styleCycle.getStyle(), style -> {
//      initializeLocationComponent(mapboxMap);
      navigationMapRoute = new NavigationMapRoute(null, mapView, mapboxMap);
      mapboxMap.addOnMapLongClickListener(this);
      Snackbar.make(mapView, "Long press to select route", Snackbar.LENGTH_SHORT).show();
    });
  }

  @Override
  public boolean onMapLongClick(@NonNull LatLng point) {
    handleClicked(point);
    return true;
  }

  @Override
  public void onResponse(@NonNull Call<DirectionsResponse> call, @NonNull Response<DirectionsResponse> response) {
    if (response.isSuccessful()
      && response.body() != null
      && !response.body().routes().isEmpty()) {
      List<DirectionsRoute> routes = response.body().routes();
      navigationMapRoute.addRoutes(routes);
      routeLoading.setVisibility(View.INVISIBLE);
      fabRemoveRoute.setVisibility(View.VISIBLE);
    }
  }

  @Override
  public void onFailure(@NonNull Call<DirectionsResponse> call, @NonNull Throwable throwable) {
    Timber.e(throwable);
  }

  @Override
  public void onResume() {
    super.onResume();
    mapView.onResume();
  }

  @Override
  protected void onStart() {
    super.onStart();
    mapView.onStart();
    if (navigationMapRoute != null) {
      navigationMapRoute.onStart();
    }
  }

  @Override
  protected void onStop() {
    super.onStop();
    mapView.onStop();
    if (navigationMapRoute != null) {
      navigationMapRoute.onStop();
    }
  }

  @Override
  public void onPause() {
    super.onPause();
    mapView.onPause();
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

  @SuppressWarnings("MissingPermission")
  private void initializeLocationComponent(MapboxMap mapboxMap) {
    LocationComponent locationComponent = mapboxMap.getLocationComponent();
    locationComponent.activateLocationComponent(this, mapboxMap.getStyle());
    locationComponent.setLocationComponentEnabled(true);
    locationComponent.setRenderMode(RenderMode.COMPASS);
    locationComponent.setCameraMode(CameraMode.TRACKING);
    locationComponent.zoomWhileTracking(10d);
  }

  private void handleClicked(@NonNull LatLng point) {
    vibrate();
    if (originMarker == null) {
      originMarker = mapboxMap.addMarker(new MarkerOptions().position(point));
      Snackbar.make(mapView, "Origin selected", Snackbar.LENGTH_SHORT).show();
    } else if (destinationMarker == null) {
      destinationMarker = mapboxMap.addMarker(new MarkerOptions().position(point));
      Point originPoint = Point.fromLngLat(
        originMarker.getPosition().getLongitude(), originMarker.getPosition().getLatitude());
      Point destinationPoint = Point.fromLngLat(
        destinationMarker.getPosition().getLongitude(), destinationMarker.getPosition().getLatitude());
      Snackbar.make(mapView, "Destination selected", Snackbar.LENGTH_SHORT).show();
      findRoute(originPoint, destinationPoint);
      routeLoading.setVisibility(View.VISIBLE);
    }
  }

  @SuppressLint("MissingPermission")
  private void vibrate() {
    Vibrator vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
    if (vibrator == null) {
      return;
    }
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
      vibrator.vibrate(VibrationEffect.createOneShot(ONE_HUNDRED_MILLISECONDS, VibrationEffect.DEFAULT_AMPLITUDE));
    } else {
      vibrator.vibrate(ONE_HUNDRED_MILLISECONDS);
    }
  }

  private void removeRouteAndMarkers() {
    mapboxMap.removeMarker(originMarker);
    originMarker = null;
    mapboxMap.removeMarker(destinationMarker);
    destinationMarker = null;
    navigationMapRoute.removeRoute();
  }

  public void findRoute(Point origin, Point destination) {
    NavigationRoute.builder(this)
      .accessToken(Mapbox.getAccessToken())
      .origin(origin)
      .destination(destination)
      .alternatives(true)
      .build()
      .getRoute(this);
  }

  private static class StyleCycle {
    private static final String[] STYLES = new String[] {
      Style.MAPBOX_STREETS,
      Style.OUTDOORS,
      Style.LIGHT,
      Style.DARK,
      Style.SATELLITE_STREETS
    };

    private int index;

    private String getNextStyle() {
      index++;
      if (index == STYLES.length) {
        index = 0;
      }
      return getStyle();
    }

    private String getStyle() {
      return STYLES[index];
    }
  }
}

