package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.ObjectAnimator;
import android.animation.TypeEvaluator;
import android.animation.ValueAnimator;
import android.os.Bundle;
import android.support.annotation.DrawableRes;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.animation.AccelerateDecelerateInterpolator;

import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerView;
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

    private MapView mapView;
    private MapboxMap mapboxMap;

    private LatLng dupontCircle = new LatLng(38.90962, -77.04341);

    private Marker passengerMarker = null;
    private MarkerView carMarker = null;

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

        mapView = (MapView) findViewById(R.id.mapView);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(new OnMapReadyCallback() {

            @Override
            public void onMapReady(@NonNull final MapboxMap mapboxMap) {
                AnimatedMarkerActivity.this.mapboxMap = mapboxMap;
                setupMap();

                for (int i = 0; i < 10; i++) {
                    addRandomCar();
                }

                addPassenger();
                addMainCar();
                animateMoveToPassenger(carMarker);
            }
        });
    }

    private void setupMap() {
        CameraPosition cameraPosition = new CameraPosition.Builder()
            .target(dupontCircle)
            .zoom(15)
            .build();
        mapboxMap.setCameraPosition(cameraPosition);
    }

    private void addPassenger() {
        LatLng randomLatLng = getLatLngInBounds();

        if (passengerMarker == null) {
            Icon icon = IconFactory.getInstance(AnimatedMarkerActivity.this)
                .fromResource(R.drawable.ic_directions_run_black_24dp);
            passengerMarker = mapboxMap.addMarker(new MarkerViewOptions()
                .position(randomLatLng)
                .icon(icon));
        } else {
            passengerMarker.setPosition(randomLatLng);
        }
    }

    private void addMainCar() {
        LatLng randomLatLng = getLatLngInBounds();

        if (carMarker == null) {
            carMarker = createCarMarker(randomLatLng, R.drawable.ic_taxi_top);
        } else {
            carMarker.setPosition(randomLatLng);
        }

        // Make sure the car marker is selected so that it's always brought to the front (#5285)
        mapboxMap.selectMarker(carMarker);
    }

    private void animateMoveToPassenger(final MarkerView car) {
        ValueAnimator animator = animateMoveMarker(car, passengerMarker.getPosition());
        animator.addListener(new AnimatorListenerAdapter() {
            @Override
            public void onAnimationEnd(Animator animation) {
                addPassenger();
                animateMoveToPassenger(car);
            }
        });
    }

    protected void addRandomCar() {
        MarkerView car = createCarMarker(getLatLngInBounds(), R.drawable.ic_car_top);
        randomlyMoveMarker(car);
    }

    private void randomlyMoveMarker(final MarkerView marker) {
        ValueAnimator animator = animateMoveMarker(marker, getLatLngInBounds());

        //Add listener to restart animation on end
        animator.addListener(new AnimatorListenerAdapter() {
            @Override
            public void onAnimationEnd(Animator animation) {
                randomlyMoveMarker(marker);
            }
        });
    }

    private ValueAnimator animateMoveMarker(final MarkerView marker, LatLng to) {
        marker.setRotation((float) getBearing(marker.getPosition(), to));

        final ValueAnimator markerAnimator = ObjectAnimator.ofObject(
            marker, "position", new LatLngEvaluator(), marker.getPosition(), to);
        markerAnimator.setDuration((long) (10 * marker.getPosition().distanceTo(to)));
        markerAnimator.setInterpolator(new AccelerateDecelerateInterpolator());

        // Start
        markerAnimator.start();

        return markerAnimator;
    }

    private MarkerView createCarMarker(LatLng start, @DrawableRes int carResource) {
        Icon icon = IconFactory.getInstance(AnimatedMarkerActivity.this)
            .fromResource(carResource);

        //View Markers
        return mapboxMap.addMarker(new MarkerViewOptions()
            .position(start)
            .icon(icon));

        //GL Markers
//        return mapboxMap.addMarker(new MarkerOptions()
//                .position(start)
//                .icon(icon));

    }

    private LatLng getLatLngInBounds() {
        LatLngBounds bounds = mapboxMap.getProjection().getVisibleRegion().latLngBounds;
        Random generator = new Random();
        double randomLat = bounds.getLatSouth() + generator.nextDouble()
            * (bounds.getLatNorth() - bounds.getLatSouth());
        double randomLon = bounds.getLonWest() + generator.nextDouble()
            * (bounds.getLonEast() - bounds.getLonWest());
        return new LatLng(randomLat, randomLon);
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
    protected void onStart() {
        super.onStart();
        mapView.onStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mapView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mapView.onPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
        mapView.onStop();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mapView.onSaveInstanceState(outState);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mapView.onDestroy();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mapView.onLowMemory();
    }

    /**
     * Evaluator for LatLng pairs
     */
    private static class LatLngEvaluator implements TypeEvaluator<LatLng> {

        private LatLng latLng = new LatLng();

        @Override
        public LatLng evaluate(float fraction, LatLng startValue, LatLng endValue) {
            latLng.setLatitude(startValue.getLatitude()
                + ((endValue.getLatitude() - startValue.getLatitude()) * fraction));
            latLng.setLongitude(startValue.getLongitude()
                + ((endValue.getLongitude() - startValue.getLongitude()) * fraction));
            return latLng;
        }
    }

    private double getBearing(LatLng from, LatLng to) {
        double degrees2radians = Math.PI / 180;
        double radians2degrees = 180 / Math.PI;

        double lon1 = degrees2radians * from.getLongitude();
        double lon2 = degrees2radians * to.getLongitude();
        double lat1 = degrees2radians * from.getLatitude();
        double lat2 = degrees2radians * to.getLatitude();
        double a = Math.sin(lon2 - lon1) * Math.cos(lat2);
        double b = Math.cos(lat1) * Math.sin(lat2)
            - Math.sin(lat1) * Math.cos(lat2) * Math.cos(lon2 - lon1);

        return radians2degrees * Math.atan2(a, b);
    }
}
