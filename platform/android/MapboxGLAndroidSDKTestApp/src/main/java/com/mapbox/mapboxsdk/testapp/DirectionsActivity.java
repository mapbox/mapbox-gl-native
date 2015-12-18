package com.mapbox.mapboxsdk.testapp;

import android.content.Context;
import android.graphics.RectF;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.geometry.CoordinateBounds;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.testapp.model.directions.DirectionsFeature;
import com.mapbox.mapboxsdk.testapp.model.directions.DirectionsResponse;
import com.mapbox.mapboxsdk.testapp.model.directions.DirectionsRoute;
import com.mapbox.mapboxsdk.testapp.model.directions.RouteStep;
import com.mapbox.mapboxsdk.testapp.model.directions.StepManeuver;
import com.mapbox.mapboxsdk.testapp.utils.DirectionsUtil;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;

import java.util.ArrayList;
import java.util.List;

public class DirectionsActivity extends AppCompatActivity {

    private MapView mMapView;

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
        mMapView.setAccessToken(ApiAccess.getToken(this));
        mMapView.onCreate(savedInstanceState);

        // load routes async
        new DirectionsTask(this).execute();
    }

    public void setDirectionsResponse(@NonNull DirectionsResponse response) {

        // Show lines for routes and markers for steps
        final List<DirectionsRoute> routes = response.getRoutes();
        final List<PolylineOptions> polylineOptions = new ArrayList<>();
        final List<MarkerOptions> stepMarkers = new ArrayList<>();
        for (int i = routes.size() - 1; i >= 0; i--) {

            // show lines
            DirectionsRoute route = routes.get(i);
            PolylineOptions builder = new PolylineOptions();
            for (List<Double> coordinates : route.getGeometry().getCoordinates()) {
                builder.add(new LatLng(coordinates.get(1), coordinates.get(0)));
            }
            polylineOptions.add(builder);

            // Show steps
            final List<RouteStep> steps = route.getSteps();
            for (int j = 0; j < steps.size(); j++) {
                StepManeuver maneuver = steps.get(j).getManeuver();
                List<Double> coordinates = maneuver.getLocation().getCoordinates();
                stepMarkers.add(new MarkerOptions()
                        .title("Step " + j)
                        .snippet(maneuver.getInstruction())
                        .position(new LatLng(coordinates.get(1), coordinates.get(0))));
            }
        }

        mMapView.addPolylines(polylineOptions);
        mMapView.addMarkers(stepMarkers);

        // Add marker for origin
        final DirectionsFeature origin = response.getOrigin();
        final LatLng originLatLng = DirectionsUtil.convertLatLng(origin);
        mMapView.addMarker(new MarkerOptions()
                .title("Origin")
                .snippet(origin.getProperties().getName())
                .icon(mMapView.getSpriteFactory().fromResource(R.drawable.ic_location))
                .position(originLatLng));

        // Add marker for destination
        final DirectionsFeature destination = response.getDestination();
        final LatLng destinationLatLng = DirectionsUtil.convertLatLng(destination);
        mMapView.addMarker(new MarkerOptions()
                .title("Destination")
                .snippet(destination.getProperties().getName())
                .icon(mMapView.getSpriteFactory().fromResource(R.drawable.ic_location_city_24dp))
                .position(destinationLatLng));

        // Move bounds of Map to fit origin & destination
        CoordinateBounds coordinateBounds = DirectionsUtil.calculateCoordinateBounds(originLatLng, destinationLatLng);
        mMapView.setVisibleCoordinateBounds(coordinateBounds, new RectF(200, 0, 200, 0), true);
    }

    @Override
    protected void onStart() {
        super.onStart();
        mMapView.onStart();
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
    protected void onStop() {
        super.onStop();
        mMapView.onStop();
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

    private class DirectionsTask extends AsyncTask<Void, Void, DirectionsResponse> {

        private Context mContext;

        public DirectionsTask(Context context) {
            mContext = context.getApplicationContext();
        }

        @Override
        protected DirectionsResponse doInBackground(Void... params) {
            return DirectionsUtil.getDirections(mContext);
        }

        @Override
        protected void onPostExecute(DirectionsResponse directionsResponse) {
            super.onPostExecute(directionsResponse);
            if (directionsResponse != null) {
                setDirectionsResponse(directionsResponse);
            }
        }
    }

}
