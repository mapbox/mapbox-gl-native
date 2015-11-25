package com.mapbox.mapboxsdk.testapp;

import android.graphics.RectF;
import android.os.Bundle;
import android.support.design.widget.Snackbar;
import android.support.v7.app.ActionBar;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;

import com.mapbox.mapboxsdk.MapActivity;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.CoordinateBounds;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.views.MapView;

public class VisibleCoordinateBoundsActivity extends MapActivity {

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

        MapView mapView = getMapView();

        final LatLng losAngeles = new LatLng(34.053940, -118.242622);
        final LatLng newYork = new LatLng(40.712730, -74.005953);

        mapView.addMarker(new MarkerOptions()
                .title("Los Angeles")
                .snippet("City Hall")
                .position(losAngeles));

        mapView.addMarker(new MarkerOptions()
                .title("New York")
                .snippet("City Hall")
                .position(newYork));

        Snackbar.make(findViewById(android.R.id.content), R.string.action_visible_bounds_explanation, Snackbar.LENGTH_INDEFINITE)
                .setAction(android.R.string.ok, new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        float margin = getResources().getDimension(R.dimen.coordinatebounds_margin);
                        mMapView.setVisibleCoordinateBounds(new CoordinateBounds(losAngeles, newYork), new RectF(margin, 0, margin, 0), true);
                    }
                }).show();
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
