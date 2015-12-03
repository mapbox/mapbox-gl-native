package com.mapbox.mapboxsdk.testapp;

import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;

public class TiltActivity extends AppCompatActivity {

    private MapView mMapView = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_tilt);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        // Target
        LatLng dc = new LatLng(38.90252, -77.02291);
        LatLng nyc = new LatLng(40.73581, -73.99155);
        LatLng target = nyc;

        // Set up the map
        mMapView = (MapView) findViewById(R.id.tiltMapView);
        mMapView.setAccessToken(ApiAccess.getToken(this));
        mMapView.setStyleUrl(Style.MAPBOX_STREETS);
        mMapView.setCenterCoordinate(target);
        mMapView.setZoomLevel(11);
        mMapView.onCreate(savedInstanceState);

        /*
         * Our tilt API follows Google's Android API:
         * https://developers.google.com/maps/documentation/android-api/views#updating_the_camera_view
         */

        // Construct a CameraPosition focusing on target and animate the camera to that position.
        CameraPosition cameraPosition = new CameraPosition.Builder()
                .target(target)   // Sets the center of the map to target
                .zoom(17)         // Sets the zoom
                .bearing(90)      // Sets the orientation of the camera to east
                .tilt(30)         // Sets the tilt of the camera to 30 degrees
                .build();         // Creates a CameraPosition from the builder

        // Triggers tilt
        mMapView.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
    }

    @Override
    protected void onStart() {
        super.onStart();
        mMapView.onStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
        mMapView.onStop();
    }

    @Override
    public void onPause()  {
        super.onPause();
        mMapView.onPause();
    }

    @Override
    public void onResume() {
        super.onResume();
        mMapView.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMapView.onDestroy();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mMapView.onSaveInstanceState(outState);
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
