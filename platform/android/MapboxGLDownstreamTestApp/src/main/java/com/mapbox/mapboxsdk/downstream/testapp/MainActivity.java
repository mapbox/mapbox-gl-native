package com.mapbox.mapboxsdk.downstream.testapp;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.mapbox.api.directions.v5.DirectionsAdapterFactory;
import com.mapbox.api.directions.v5.DirectionsCriteria;
import com.mapbox.api.directions.v5.MapboxDirections;
import com.mapbox.api.directions.v5.models.DirectionsResponse;
import com.mapbox.api.directions.v5.models.DirectionsRoute;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.services.android.navigation.ui.v5.route.NavigationMapRoute;
import com.mapbox.services.android.navigation.ui.v5.route.OnRouteSelectionChangeListener;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import timber.log.Timber;

public class MainActivity extends AppCompatActivity implements OnMapReadyCallback,
  MapboxMap.OnMapClickListener, Callback<DirectionsResponse>, OnRouteSelectionChangeListener {

  private static final String DIRECTIONS_RESPONSE = "directions-route.json";

  private MapView mapView;
  private TextView primaryRouteIndexTextView;

  private MapboxMap mapboxMap;
  private NavigationMapRoute navigationMapRoute;
  private StyleCycle styleCycle = new StyleCycle();

  private Marker originMarker;
  private Marker destinationMarker;
  private boolean alternativesVisible = true;
  private List<DirectionsRoute> routes = new ArrayList<>();

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    Mapbox.getInstance(this, getString(R.string.mapbox_access_token));

    setContentView(R.layout.activity_main);

    primaryRouteIndexTextView = findViewById(R.id.primaryRouteIndexTextView);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.setStyleUrl(styleCycle.getStyle());
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);

    findViewById(R.id.fabStyles).setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        if (mapboxMap != null) {
          mapboxMap.setStyleUrl(styleCycle.getNextStyle());
        }
      }
    });

    findViewById(R.id.fabToggleAlternatives).setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        alternativesVisible = !alternativesVisible;
        if (navigationMapRoute != null) {
          navigationMapRoute.showAlternativeRoutes(alternativesVisible);
        }
      }
    });
  }

  @Override
  public void onNewPrimaryRouteSelected(DirectionsRoute directionsRoute) {
    primaryRouteIndexTextView.setText(String.valueOf(routes.indexOf(directionsRoute)));
  }

  @Override
  public void onMapReady(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
    navigationMapRoute = new NavigationMapRoute(null, mapView, mapboxMap,
      "admin-3-4-boundaries-bg");
    Gson gson = new GsonBuilder().registerTypeAdapterFactory(DirectionsAdapterFactory.create())
      .create();
    DirectionsResponse response = gson.fromJson(loadJsonFromAsset(DIRECTIONS_RESPONSE),
      DirectionsResponse.class);
    navigationMapRoute.addRoute(response.routes().get(0));
    mapboxMap.addOnMapClickListener(this);
    navigationMapRoute.setOnRouteSelectionChangeListener(this);
  }

  @Override
  public void onMapClick(@NonNull LatLng point) {
    if (originMarker == null) {
      originMarker = mapboxMap.addMarker(new MarkerOptions().position(point));
    } else if (destinationMarker == null) {
      destinationMarker = mapboxMap.addMarker(new MarkerOptions().position(point));
      Point originPoint = Point.fromLngLat(
        originMarker.getPosition().getLongitude(), originMarker.getPosition().getLatitude());
      Point destinationPoint = Point.fromLngLat(
        destinationMarker.getPosition().getLongitude(), destinationMarker.getPosition().getLatitude());
      requestDirectionsRoute(originPoint, destinationPoint);
      mapboxMap.removeMarker(originMarker);
      mapboxMap.removeMarker(destinationMarker);
    } else {
      originMarker = null;
      destinationMarker = null;
      navigationMapRoute.removeRoute();
    }
  }

//  @Override
//  public void onMapLongClick(@NonNull LatLng point) {
//
//  }

  public void requestDirectionsRoute(Point origin, Point destination) {
    MapboxDirections directions = MapboxDirections.builder()
      .origin(origin)
      .destination(destination)
      .accessToken(Mapbox.getAccessToken())
      .profile(DirectionsCriteria.PROFILE_DRIVING_TRAFFIC)
      .overview(DirectionsCriteria.OVERVIEW_FULL)
      .annotations(DirectionsCriteria.ANNOTATION_CONGESTION)
      .alternatives(true)
      .steps(true)
      .build();

    directions.enqueueCall(this);
  }

  @Override
  public void onResponse(Call<DirectionsResponse> call, Response<DirectionsResponse> response) {
    if (response.body() != null && !response.body().routes().isEmpty()) {
      List<DirectionsRoute> routes = response.body().routes();
      this.routes = routes;
      navigationMapRoute.addRoutes(routes);
    }
  }

  @Override
  public void onFailure(Call<DirectionsResponse> call, Throwable throwable) {
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
  }

  @Override
  protected void onStop() {
    super.onStop();
    mapView.onStop();
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

  public NavigationMapRoute getNavigationMapRoute() {
    return navigationMapRoute;
  }

  private String loadJsonFromAsset(String filename) {
    // Using this method to load in GeoJSON files from the assets folder.

    try {
      InputStream is = getAssets().open(filename);
      int size = is.available();
      byte[] buffer = new byte[size];
      is.read(buffer);
      is.close();
      return new String(buffer, "UTF-8");

    } catch (IOException ex) {
      ex.printStackTrace();
      return null;
    }
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