package com.mapbox.mapboxsdk.testapp.activity.directions;

import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.services.Constants;
import com.mapbox.services.commons.ServicesException;
import com.mapbox.services.commons.geojson.LineString;
import com.mapbox.services.commons.models.Position;
import com.mapbox.services.directions.v5.DirectionsCriteria;
import com.mapbox.services.directions.v5.MapboxDirections;
import com.mapbox.services.directions.v5.models.DirectionsResponse;
import com.mapbox.services.directions.v5.models.DirectionsRoute;

import java.util.ArrayList;
import java.util.List;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class DirectionsActivity extends AppCompatActivity {

    private static final String LOG_TAG = "DirectionsActivity";

    private MapView mapView;
    private MapboxMap mapboxMap;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_directions);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mapView = (MapView) findViewById(R.id.mapView);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                DirectionsActivity.this.mapboxMap = mapboxMap;
                loadRoute();
            }
        });
    }

    private void loadRoute() {
        // Dupont Circle (Washington, DC)
        Position origin = Position.fromCoordinates(-77.04341, 38.90962);

        // The White House (Washington, DC)
        Position destination = Position.fromCoordinates(-77.0365, 38.8977);

        // Set map at centroid
        LatLng centroid = new LatLng(
                (origin.getLatitude() + destination.getLatitude()) / 2,
                (origin.getLongitude() + destination.getLongitude()) / 2);

        mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(new CameraPosition.Builder()
                .target(centroid)
                .zoom(14)
                .build()));

        // Add origin and destination to the map
        mapboxMap.addMarker(new MarkerOptions()
                .position(new LatLng(origin.getLatitude(), origin.getLongitude()))
                .title("Origin")
                .snippet("Dupont Circle"));
        mapboxMap.addMarker(new MarkerOptions()
                .position(new LatLng(destination.getLatitude(), destination.getLongitude()))
                .title("Destination")
                .snippet("The White House"));

        // Get route from API
        getRoute(origin, destination);
    }

    private void getRoute(Position origin, Position destination) {
        try {
            MapboxDirections md = new MapboxDirections.Builder()
                    .setAccessToken(getString(R.string.mapbox_access_token))
                    .setOrigin(origin)
                    .setOverview(DirectionsCriteria.OVERVIEW_FULL)
                    .setDestination(destination)
                    .setProfile(DirectionsCriteria.PROFILE_WALKING)
                    .build();

            md.enqueueCall(new Callback<DirectionsResponse>() {

                @Override
                public void onFailure(Call<DirectionsResponse> call, Throwable throwable) {
                    Log.e(LOG_TAG, "Error: " + throwable.getMessage());
                }

                @Override
                public void onResponse(Call<DirectionsResponse> call, Response<DirectionsResponse> response) {
                    // You can get generic HTTP info about the response
                    Log.d(LOG_TAG, "Response code: " + response.code());

                    // Print some info about the route
                    DirectionsRoute currentRoute = response.body().getRoutes().get(0);
                    Log.d(LOG_TAG, "Distance: " + currentRoute.getDistance());

                    // Draw the route on the map
                    drawRoute(currentRoute);
                }

            });
        } catch (ServicesException servicesException) {
            Log.e(LOG_TAG, "Error displaying route: " + servicesException.toString());
            servicesException.printStackTrace();
        }
    }

    private void drawRoute(DirectionsRoute route) {

        PolylineOptions builder = new PolylineOptions();
        builder.color(Color.parseColor("#3887be"));
        builder.alpha(0.5f);
        builder.width(5);
        builder.width(5);

        LineString lineString = LineString.fromPolyline(route.getGeometry(), Constants.OSRM_PRECISION_V5);
        List<Position> coordinates = lineString.getCoordinates();
        List<LatLng> points = new ArrayList<>();
        for (int i = 0; i < coordinates.size(); i++) {
            points.add(new LatLng(
              coordinates.get(i).getLatitude(),
              coordinates.get(i).getLongitude()));
        }

        builder.addAll(points);

        // Draw Points on MapView
        mapboxMap.addPolyline(builder);
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

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

}
