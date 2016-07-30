package com.mapbox.mapboxsdk.testapp.activity.camera;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

public class CameraAnimationChainingActivity extends AppCompatActivity {
    private static final String TAG = CameraAnimationChainingActivity.class.getSimpleName();

    private MapView mapView;

    private boolean cameraState;
    private static final LatLng LAT_LNG_LONDON_EYE = new LatLng(51.50325, -0.11968);
    private static final LatLng LAT_LNG_TOWER_BRIDGE = new LatLng(51.50550, -0.07520);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, String.format("UI Thread id: %s, name %s", Thread.currentThread().getId(), Thread.currentThread().getName()));
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_animation_chaining);

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
                mapboxMap.getUiSettings().setAttributionEnabled(false);
                mapboxMap.getUiSettings().setLogoEnabled(false);

                // handle animate button clicks
                //noinspection ConstantConditions
                findViewById(R.id.cameraAnimateButton).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        animateToNextPosition(2);
                    }
                });
            }
        });
    }

    private void animateToNextPosition(final int iterations) {
        //Setup the target camera position
        final CameraPosition cameraPosition = new CameraPosition.Builder()
                .target(getNextLatLng())
                .bearing(270)
                .tilt(20)
                .build();

        //Create a callback
        final MapboxMap.CancelableCallback callback = new MapboxMap.CancelableCallback() {
            @Override
            public void onCancel() {
                Log.i(TAG, String.format("Duration onCancel Callback called on Thread id: %s, name %s", Thread.currentThread().getId(), Thread.currentThread().getName()));
                //Toast.makeText(CameraAnimationChainingActivity.this, "Duration onCancel Callback called.", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onFinish() {
                Log.i(TAG, String.format("Duration onFinish Callback called on Thread id: %s, name %s", Thread.currentThread().getId(), Thread.currentThread().getName()));
                //Toast.makeText(CameraAnimationChainingActivity.this, "Duration onFinish Callback called.", Toast.LENGTH_SHORT).show();
                if (iterations > 0) {
                    animateToNextPosition(iterations - 1);
                }
            }
        };

        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(MapboxMap mapboxMap) {
                mapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition), 7500, callback);
            }
        });
    }

    private LatLng getNextLatLng() {
        cameraState = !cameraState;
        return cameraState ? LAT_LNG_TOWER_BRIDGE : LAT_LNG_LONDON_EYE;
    }

    @Override
    public void onResume() {
        super.onResume();
        mapView.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        mapView.onPause();
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
