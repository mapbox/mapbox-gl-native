package com.mapbox.mapboxsdk.testapp;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.views.MapView;

public class DoubleMapActivity extends AppCompatActivity {

    private static final String TAG_FRAGMENT = "map";
    private DoubleMapFragment mMapFragment;

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
            transaction.add(R.id.fragment_container, mMapFragment = new DoubleMapFragment(), TAG_FRAGMENT);
            transaction.commit();
        } else {
            mMapFragment = (DoubleMapFragment) getSupportFragmentManager().findFragmentByTag(TAG_FRAGMENT);
        }
    }

    public static class DoubleMapFragment extends Fragment {

        private MapView mMapView;
        private MapView mMapViewMini;

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
            return inflater.inflate(R.layout.fragment_double_map, container, false);
        }

        @Override
        public void onViewCreated(View view, Bundle savedInstanceState) {
            super.onViewCreated(view, savedInstanceState);

            // MapView large
            mMapView = (MapView) view.findViewById(R.id.mapview);
            mMapView.onCreate(savedInstanceState);
            mMapView.setStyle(Style.DARK);
            mMapView.setZoom(18);

            // MapView mini
            mMapViewMini = (MapView) view.findViewById(R.id.mini_map);
            mMapViewMini.onCreate(savedInstanceState);
            mMapViewMini.setStyle(Style.LIGHT);
            mMapViewMini.setAttributionVisibility(View.GONE);
            mMapViewMini.setLogoVisibility(View.GONE);
            mMapViewMini.setCompassEnabled(false);
            mMapViewMini.setZoom(4);
            mMapViewMini.setAllGesturesEnabled(false);

            try {
                mMapView.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW);
                mMapViewMini.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW);
            }catch (SecurityException e){
                // permission is handled in MainActivity
                getActivity().finish();
            }
        }

        @Override
        public void onStart() {
            super.onStart();
            mMapView.onStart();
            mMapViewMini.onStart();
        }

        @Override
        public void onResume() {
            super.onResume();
            mMapView.onResume();
            mMapViewMini.onResume();
        }

        @Override
        public void onPause() {
            super.onPause();
            mMapView.onPause();
            mMapViewMini.onPause();
        }

        @Override
        public void onStop() {
            super.onStop();
            mMapView.onStop();
            mMapViewMini.onStop();
        }

        @Override
        public void onDestroy() {
            super.onDestroy();
            mMapView.onDestroy();
            mMapViewMini.onDestroy();
        }

        @Override
        public void onLowMemory() {
            super.onLowMemory();
            mMapView.onLowMemory();
            mMapViewMini.onLowMemory();
        }

        @Override
        public void onSaveInstanceState(Bundle outState) {
            super.onSaveInstanceState(outState);
            mMapView.onSaveInstanceState(outState);
            mMapViewMini.onSaveInstanceState(outState);
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
