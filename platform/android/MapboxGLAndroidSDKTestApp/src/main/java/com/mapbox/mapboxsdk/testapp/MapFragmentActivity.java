package com.mapbox.mapboxsdk.testapp;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Gravity;
import android.view.MenuItem;
import android.view.View;

import com.mapbox.mapboxsdk.MapFragment;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.views.MapView;
import com.mapbox.mapboxsdk.views.MapboxMap;
import com.mapbox.mapboxsdk.views.OnMapReadyCallback;

public class MapFragmentActivity extends AppCompatActivity implements OnMapReadyCallback{

    private static final String TAG_FRAGMENT = "map";
    private MapFragment mMapFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map_fragment);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        if (savedInstanceState == null) {
            FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
            transaction.add(R.id.fragment_container, mMapFragment = new MapFragment(), TAG_FRAGMENT);
            transaction.commit();
        } else {
            mMapFragment = (MapFragment) getSupportFragmentManager().findFragmentByTag(TAG_FRAGMENT);
        }

        mMapFragment.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                mapboxMap.setStyleUrl(Style.EMERALD);
                mapboxMap.setZoom(12);
                mapboxMap.setLatLng(new LatLng(50.853658, 4.352419));

                // move attribution control to right of screen
                mapboxMap.setAttributionGravity(Gravity.BOTTOM | Gravity.END);
                int tenDp =  (int) getResources().getDimension(R.dimen.attr_margin);
                mapboxMap.setAttributionMargins(0, 0, tenDp, tenDp);
            }
        });
    }

    @Override
    public void onMapReady(@NonNull MapboxMap mapboxMap) {

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
