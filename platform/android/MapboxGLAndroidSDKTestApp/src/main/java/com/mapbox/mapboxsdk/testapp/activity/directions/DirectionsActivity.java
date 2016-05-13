package com.mapbox.mapboxsdk.testapp.activity.directions;

import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;

import com.mapbox.directions.DirectionsCriteria;
import com.mapbox.directions.MapboxDirections;
import com.mapbox.directions.service.models.DirectionsResponse;
import com.mapbox.directions.service.models.DirectionsRoute;
import com.mapbox.directions.service.models.Waypoint;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.maps.MapView;

import java.util.List;

import retrofit.Callback;
import retrofit.Response;
import retrofit.Retrofit;

public class DirectionsActivity extends AppCompatActivity {

    private final static String LOG_TAG = "DirectionsActivity";

    private MapView mMapView;
    private MapboxMap mMapboxMap;

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

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                mMapboxMap = mapboxMap;
                loadRoute();
            }
        });
    }

    private void loadRoute() {
        // Dupont Circle (Washington, DC)
        Waypoint origin = new Waypoint(-77.04341, 38.90962);

        // The White House (Washington, DC)
        Waypoint destination = new Waypoint(-77.0365, 38.8977);

        // Set map at centroid
        LatLng centroid = new LatLng(
                (origin.getLatitude() + destination.getLatitude()) / 2,
                (origin.getLongitude() + destination.getLongitude()) / 2);

        mMapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(new CameraPosition.Builder()
                .target(centroid)
                .zoom(14)
                .build()));

        // Add origin and destination to the map
        mMapboxMap.addMarker(new MarkerOptions()
                .position(new LatLng(origin.getLatitude(), origin.getLongitude()))
                .title("Origin")
                .snippet("Dupont Circle"));
        mMapboxMap.addMarker(new MarkerOptions()
                .position(new LatLng(destination.getLatitude(), destination.getLongitude()))
                .title("Destination")
                .snippet("The White House"));

        // Get route from API
        getRoute(origin, destination);
    }

    private void getRoute(Waypoint origin, Waypoint destination) {
        MapboxDirections md = new MapboxDirections.Builder()
                .setAccessToken(getString(R.string.mapbox_access_token))
                .setOrigin(origin)
                .setDestination(destination)
                .setProfile(DirectionsCriteria.PROFILE_WALKING)
                .build();

        md.enqueue(new Callback<DirectionsResponse>() {
            @Override
            public void onResponse(Response<DirectionsResponse> response, Retrofit retrofit) {
                // You can get generic HTTP info about the response
                Log.d(LOG_TAG, "Response code: " + response.code());

                // Print some info about the route
                DirectionsRoute currentRoute = response.body().getRoutes().get(0);
                Log.d(LOG_TAG, "Distance: " + currentRoute.getDistance());

                // Draw the route on the map
                drawRoute(currentRoute);
            }

            @Override
            public void onFailure(Throwable t) {
                Log.e(LOG_TAG, "Error: " + t.getMessage());
            }
        });
    }

    private void drawRoute(DirectionsRoute route) {
        // Convert List<Waypoint> into LatLng[]
        List<Waypoint> waypoints = route.getGeometry().getWaypoints();
        LatLng[] point = new LatLng[waypoints.size()];
        for (int i = 0; i < waypoints.size(); i++) {
            point[i] = new LatLng(
                    waypoints.get(i).getLatitude(),
                    waypoints.get(i).getLongitude());
        }

        // Draw Points on MapView
        mMapboxMap.addPolyline(new PolylineOptions()
                .add(point)
                .color(Color.parseColor("#3887be"))
                .width(5));
    }

    @Override
    public void onResume() {
        super.onResume();
        mMapView.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        mMapView.onPause();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mMapView.onSaveInstanceState(outState);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMapView.onDestroy();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mMapView.onLowMemory();
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
