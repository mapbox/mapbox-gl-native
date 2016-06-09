package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.animation.Animator;
import android.animation.TypeEvaluator;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.animation.LinearInterpolator;

import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerViewOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.Random;

public class AnimatedMarkerActivity extends AppCompatActivity {

    private final static String LOG_TAG = "AnimatedMarkerActivity";

    private MapView mMapView;
    private MapboxMap mMapboxMap;

    private LatLng dupontCircle = new LatLng(38.90962, -77.04341);

    private Marker passengerMarker = null;
    private Marker carMarker = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_animated_marker);

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
            public void onMapReady(@NonNull final MapboxMap mapboxMap) {
                mMapboxMap = mapboxMap;
                setupMap();

                addPassenger();
                addCar();
                animateMarker(carMarker, carMarker.getPosition(), passengerMarker.getPosition());
            }
        });
    }

    private void setupMap() {
        CameraPosition cameraPosition = new CameraPosition.Builder()
                .target(dupontCircle)
                .zoom(15)
                .build();
        mMapboxMap.setCameraPosition(cameraPosition);
    }

    private void addPassenger() {
        LatLng randomLatLng = getLatLngInBounds();

        if (passengerMarker == null) {
            Icon icon = IconFactory.getInstance(AnimatedMarkerActivity.this)
                    .fromResource(R.drawable.ic_directions_run_black_24dp);
            passengerMarker = mMapboxMap.addMarker(new MarkerViewOptions()
                    .position(randomLatLng)
                    .icon(icon));
        } else {
            passengerMarker.setPosition(randomLatLng);
        }
    }

    private void addCar() {
        LatLng randomLatLng = getLatLngInBounds();

        if (carMarker == null) {
            Icon icon = IconFactory.getInstance(AnimatedMarkerActivity.this)
                    .fromResource(R.drawable.ic_directions_car_black_24dp);
            carMarker = mMapboxMap.addMarker(new MarkerViewOptions()
                    .position(randomLatLng)
                    .icon(icon));
        } else {
            carMarker.setPosition(randomLatLng);
        }

        // Make sure the car marker is selected so that it's always brought to the front (#5285)
        mMapboxMap.selectMarker(carMarker);
    }

    private LatLng getLatLngInBounds() {
        LatLngBounds bounds = mMapboxMap.getProjection().getVisibleRegion().latLngBounds;
        Random generator = new Random();
        double randomLat = bounds.getLatSouth() + generator.nextDouble() * (bounds.getLatNorth() - bounds.getLatSouth());
        double randomLon = bounds.getLonWest() + generator.nextDouble() * (bounds.getLonEast() - bounds.getLonWest());
        return new LatLng(randomLat, randomLon);
    }

    private void animateMarker(final Marker marker, LatLng from, LatLng to) {
        ValueAnimator markerAnimator = ValueAnimator.ofObject(new LatLngEvaluator(), (Object[]) new LatLng[]{from, to});
        markerAnimator.setDuration(5000);
        markerAnimator.setInterpolator(new LinearInterpolator());

        markerAnimator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
            @Override
            public void onAnimationUpdate(ValueAnimator animation) {
                if (marker != null) {
                    marker.setPosition((LatLng) animation.getAnimatedValue());
                }
            }
        });

        markerAnimator.addListener(new Animator.AnimatorListener() {
            @Override
            public void onAnimationStart(Animator animation) {
                // Nothing
            }

            @Override
            public void onAnimationEnd(Animator animation) {
                Log.d(LOG_TAG, "Animation ended.");

                addPassenger();
                animateMarker(carMarker, carMarker.getPosition(), passengerMarker.getPosition());
            }

            @Override
            public void onAnimationCancel(Animator animation) {
                // Nothing
            }

            @Override
            public void onAnimationRepeat(Animator animation) {
                // Nothing
            }
        });

        // Start
        markerAnimator.start();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_zoom, menu);
        return true;
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

    private class LatLngEvaluator implements TypeEvaluator<LatLng> {

        private LatLng mLatLng = new LatLng();

        @Override
        public LatLng evaluate(float fraction, LatLng startValue, LatLng endValue) {
            mLatLng.setLatitude(startValue.getLatitude() + (endValue.getLatitude() - startValue.getLatitude()) * fraction);
            mLatLng.setLongitude(startValue.getLongitude() + (endValue.getLongitude() - startValue.getLongitude()) * fraction);
            return mLatLng;
        }
    }
}
