package com.mapbox.mapboxsdk.testapp;

import android.app.FragmentTransaction;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Gravity;
import android.view.MenuItem;
import android.view.View;

import com.mapbox.mapboxsdk.MapFragment;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;

public class MapFragmentActivity extends AppCompatActivity {

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
            FragmentTransaction transaction = getFragmentManager().beginTransaction();
            mMapFragment = MapFragment.newInstance(ApiAccess.getToken(this));

            MapView mapView = mMapFragment.getMap();
            mapView.setStyleUrl(Style.EMERALD);
            mapView.setZoomLevel(12);
            mapView.setCenterCoordinate(new LatLng(50.853658, 4.352419));

            // move attribution control to right of screen
            mapView.setAttributionGravity(Gravity.BOTTOM | Gravity.END);
            int tenDp =  (int) getResources().getDimension(R.dimen.attr_margin);
            mapView.setAttributionMargins(0, 0, tenDp, tenDp);

            transaction.add(R.id.fragment_container, mMapFragment, TAG_FRAGMENT);
            transaction.commit();
        } else {
            mMapFragment = (MapFragment) getFragmentManager().findFragmentByTag(TAG_FRAGMENT);
        }
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
