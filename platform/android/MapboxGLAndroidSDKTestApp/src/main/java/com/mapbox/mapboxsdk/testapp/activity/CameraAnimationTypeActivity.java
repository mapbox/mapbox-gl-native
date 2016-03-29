package com.mapbox.mapboxsdk.testapp.activity;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.ApiAccess;
import com.mapbox.mapboxsdk.maps.MapView;

public class CameraAnimationTypeActivity extends AppCompatActivity {

    private static final String TAG = "CameraActivity";

    private MapView mapView;

    private boolean cameraState;
    private static final LatLng LAT_LNG_LONDON_EYE = new LatLng(51.50325, -0.11968);
    private static final LatLng LAT_LNG_TOWER_BRIDGE = new LatLng(51.50550, -0.07520);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_animation_types);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mapView = (MapView) findViewById(R.id.mapView);
        mapView.setAccessToken(ApiAccess.getToken(this));
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull final MapboxMap mapboxMap) {
                mapboxMap.getUiSettings().setAttributionEnabled(false);
                mapboxMap.getUiSettings().setLogoEnabled(false);
                mapboxMap.setOnCameraChangeListener(new MapboxMap.OnCameraChangeListener() {
                    @Override
                    public void onCameraChange(CameraPosition position) {
                        Log.v(MapboxConstants.TAG, position.toString());
                    }
                });

                // handle move button clicks
                findViewById(R.id.cameraMoveButton).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        CameraPosition cameraPosition = new CameraPosition.Builder()
                                .target(getNextLatLng())
                                .zoom(14)
                                .tilt(30)
                                .tilt(0)
                                .build();

                        mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
                    }
                });

                // handle ease button clicks
                findViewById(R.id.cameraEaseButton).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        CameraPosition cameraPosition = new CameraPosition.Builder()
                                .target(getNextLatLng())
                                .zoom(15)
                                .bearing(180)
                                .tilt(30)
                                .build();

                        MapboxMap.CancelableCallback callback = new MapboxMap.CancelableCallback() {
                            @Override
                            public void onCancel() {
                                Log.i(TAG, "Duration onCancel Callback called.");
                                Toast.makeText(CameraAnimationTypeActivity.this, "Ease onCancel Callback called.", Toast.LENGTH_LONG).show();
                            }

                            @Override
                            public void onFinish() {
                                Log.i(TAG, "Duration onFinish Callback called.");
                                Toast.makeText(CameraAnimationTypeActivity.this, "Ease onFinish Callback called.", Toast.LENGTH_LONG).show();
                            }
                        };

                        mapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(cameraPosition), 7500, callback);
                    }
                });

                // handle animate button clicks
                findViewById(R.id.cameraAnimateButton).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        CameraPosition cameraPosition = new CameraPosition.Builder()
                                .target(getNextLatLng())
                                .bearing(270)
                                .tilt(20)
                                .build();

                        MapboxMap.CancelableCallback callback = new MapboxMap.CancelableCallback() {
                            @Override
                            public void onCancel() {
                                Log.i(TAG, "Duration onCancel Callback called.");
                                Toast.makeText(CameraAnimationTypeActivity.this, "Duration onCancel Callback called.", Toast.LENGTH_LONG).show();
                            }

                            @Override
                            public void onFinish() {
                                Log.i(TAG, "Duration onFinish Callback called.");
                                Toast.makeText(CameraAnimationTypeActivity.this, "Duration onFinish Callback called.", Toast.LENGTH_LONG).show();
                            }
                        };

                        mapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition), 7500, callback);
                    }
                });
            }
        });
    }

    private LatLng getNextLatLng() {
        cameraState = !cameraState;
        return cameraState ? LAT_LNG_TOWER_BRIDGE : LAT_LNG_LONDON_EYE;
    }

    @Override
    protected void onStart() {
        super.onStart();
        mapView.onStart();
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
