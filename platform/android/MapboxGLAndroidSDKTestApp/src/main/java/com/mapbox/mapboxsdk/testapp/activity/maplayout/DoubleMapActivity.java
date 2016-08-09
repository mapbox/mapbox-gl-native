package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.TrackingSettings;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.testapp.R;

public class DoubleMapActivity extends AppCompatActivity {

    private static final String TAG_FRAGMENT = "map";

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

        Fragment mapFragment;
        if (savedInstanceState == null) {
            FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
            transaction.add(R.id.fragment_container, mapFragment = new DoubleMapFragment(), TAG_FRAGMENT);
            transaction.commit();
        } else {
            mapFragment = (DoubleMapFragment) getSupportFragmentManager().findFragmentByTag(TAG_FRAGMENT);
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
            mMapView = (MapView) view.findViewById(R.id.mapView);
            mMapView.onCreate(savedInstanceState);
            mMapView.getMapAsync(new OnMapReadyCallback() {
                @Override
                public void onMapReady(@NonNull MapboxMap mapboxMap) {
                    mapboxMap.setStyleUrl(Style.DARK);

                    mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(18));
                    try {
                        TrackingSettings settings = mapboxMap.getTrackingSettings();
                        settings.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW);
                    } catch (SecurityException e) {
                        // permission is handled in MainActivity
                        getActivity().finish();
                    }
                }
            });

            // MapView mini
            mMapViewMini = (MapView) view.findViewById(R.id.mini_map);
            mMapViewMini.onCreate(savedInstanceState);
            mMapViewMini.getMapAsync(new OnMapReadyCallback() {
                @Override
                public void onMapReady(@NonNull MapboxMap mapboxMap) {
                    mapboxMap.setStyleUrl(Style.LIGHT);
                    mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(4));

                    UiSettings uiSettings = mapboxMap.getUiSettings();
                    uiSettings.setAllGesturesEnabled(false);
                    uiSettings.setCompassEnabled(false);
                    uiSettings.setAttributionEnabled(false);
                    uiSettings.setLogoEnabled(false);

                    try {
                        TrackingSettings settings = mapboxMap.getTrackingSettings();
                        settings.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW);
                    } catch (SecurityException e) {
                        // permission is handled in MainActivity
                        getActivity().finish();
                    }

                    mapboxMap.setOnMapClickListener(new MapboxMap.OnMapClickListener() {
                        @Override
                        public void onMapClick(@NonNull LatLng point) {
                            // test if we can open 2 activities after each other
                            startActivity(new Intent(mMapViewMini.getContext(), DoubleMapActivity.class));
                        }
                    });
                }
            });

            SurfaceView surfaceViewMini = (SurfaceView) mMapViewMini.findViewById(R.id.surfaceView);
            surfaceViewMini.setZOrderMediaOverlay(true);
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