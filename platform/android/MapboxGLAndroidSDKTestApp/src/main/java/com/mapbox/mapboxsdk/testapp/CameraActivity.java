package com.mapbox.mapboxsdk.testapp;

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
import com.mapbox.mapboxsdk.maps.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.maps.MapView;

public class CameraActivity extends AppCompatActivity {

    private static final String TAG = "CameraActivity";

    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.cameraMapView);
        mMapView.setAccessToken(ApiAccess.getToken(this));
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull final MapboxMap mapboxMap) {
                // set a style
                mapboxMap.setStyle(Style.MAPBOX_STREETS);
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
                                .target(new LatLng(44.50128, -88.06216))    // Sets the center of the map to Lambeau Field
                                .zoom(14)                                   // Sets the zoom
                                .tilt(30)                                   // Sets the tilt of the camera to 30 degrees
                                .build();

                        mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
                    }
                });

                // handle ease button clicks
                findViewById(R.id.cameraEaseButton).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        CameraPosition cameraPosition = new CameraPosition.Builder()
                                .target(new LatLng(48.21874, 11.62465))     // Sets the center of the map to Allianz Arena
                                .zoom(16)                                   // Sets the zoom
                                .bearing(180)                               // Sets the orientation of the camera to south
                                .build();                                   // Creates a CameraPosition from the builder

                        MapboxMap.CancelableCallback callback = new MapboxMap.CancelableCallback() {
                            @Override
                            public void onCancel() {
                                Log.i(TAG, "Duration onCancel Callback called.");
                                Toast.makeText(CameraActivity.this, "Ease onCancel Callback called.", Toast.LENGTH_LONG).show();
                            }

                            @Override
                            public void onFinish() {
                                Log.i(TAG, "Duration onFinish Callback called.");
                                Toast.makeText(CameraActivity.this, "Ease onFinish Callback called.", Toast.LENGTH_LONG).show();
                            }
                        };

                        mapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(cameraPosition), 25000, callback);
                    }
                });

                // handle animate button clicks
                findViewById(R.id.cameraAnimateButton).setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        CameraPosition cameraPosition = new CameraPosition.Builder()
                                .target(new LatLng(-22.91214, -43.23012))   // Sets the center of the map to Maracanã
                                .bearing(270)                               // Sets the orientation of the camera to west
                                .tilt(20)                                   // Sets the tilt of the camera to 30 degrees
                                .build();                                   // Creates a CameraPosition from the builder

                        MapboxMap.CancelableCallback callback = new MapboxMap.CancelableCallback() {
                            @Override
                            public void onCancel() {
                                Log.i(TAG, "Duration onCancel Callback called.");
                                Toast.makeText(CameraActivity.this, "Duration onCancel Callback called.", Toast.LENGTH_LONG).show();
                            }

                            @Override
                            public void onFinish() {
                                Log.i(TAG, "Duration onFinish Callback called.");
                                Toast.makeText(CameraActivity.this, "Duration onFinish Callback called.", Toast.LENGTH_LONG).show();
                            }
                        };

                        mapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition), 25000, callback);
                    }
                });
            }
        });
    }

    @Override
    protected void onStart() {
        super.onStart();
        mMapView.onStart();
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
    protected void onStop() {
        super.onStop();
        mMapView.onStop();
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
