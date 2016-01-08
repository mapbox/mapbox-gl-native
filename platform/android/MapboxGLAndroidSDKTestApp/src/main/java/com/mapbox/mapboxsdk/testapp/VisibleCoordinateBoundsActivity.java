package com.mapbox.mapboxsdk.testapp;

import android.graphics.RectF;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.design.widget.Snackbar;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;

import com.mapbox.mapboxsdk.MapFragment;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.CoordinateBounds;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;
import com.mapbox.mapboxsdk.views.MapboxMap;
import com.mapbox.mapboxsdk.views.OnMapReadyCallback;

public class VisibleCoordinateBoundsActivity extends AppCompatActivity {

    private static final LatLng CITY_LOS_ANGELES = new LatLng(34.053940, -118.242622);
    private static final LatLng CITY_NEW_YORK = new LatLng(40.712730, -74.005953);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_visible_bounds);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        MapFragment mapFragment = (MapFragment) getSupportFragmentManager().findFragmentById(R.id.mapFragment);
        mapFragment.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull final MapboxMap mapboxMap) {
                mapboxMap.setStyle(Style.DARK);

                mapboxMap.setAllGesturesEnabled(false);

                mapboxMap.addMarker(new MarkerOptions()
                        .title("Los Angeles")
                        .snippet("City Hall")
                        .position(CITY_LOS_ANGELES));

                mapboxMap.addMarker(new MarkerOptions()
                        .title("New York")
                        .snippet("City Hall")
                        .position(CITY_NEW_YORK));

                // bug, need to post on queu to execute
                new Handler().post(new Runnable() {

                    float margin = getResources().getDimension(R.dimen.coordinatebounds_margin);

                    @Override
                    public void run() {
                        mapboxMap.setVisibleCoordinateBounds(
                                new CoordinateBounds(CITY_LOS_ANGELES, CITY_NEW_YORK),
                                new RectF(margin, 0, margin, 0),
                                true);
                    }
                });
            }
        });
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
