package com.mapbox.mapboxsdk.testapp;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.mapbox.mapboxsdk.MapFragment;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;
import com.mapbox.mapboxsdk.views.MapboxMap;
import com.mapbox.mapboxsdk.views.OnMapReadyCallback;

public class CameraActivity extends AppCompatActivity implements OnMapReadyCallback {

    private MapboxMap mMapboxMap;

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

        MapFragment mapFragment = (MapFragment) getSupportFragmentManager().findFragmentById(R.id.mapFragment);
        mapFragment.getMapAsync(this);
    }

    @Override
    public void onMapReady(@NonNull MapboxMap mapboxMap) {
        mMapboxMap = mapboxMap;
        mMapboxMap.setStyle(Style.MAPBOX_STREETS);
        mMapboxMap.setCompassEnabled(true);
        initClickListeners();
    }

    private void initClickListeners() {
        findViewById(R.id.cameraMoveButton).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                CameraPosition cameraPosition = new CameraPosition.Builder()
                        .target(new LatLng(44.50128, -88.06216))    // Sets the center of the map to Lambeau Field
                        .zoom(14)                                   // Sets the zoom
                        .tilt(30)                                   // Sets the tilt of the camera to 30 degrees
                        .build();                                   // Creates a CameraPosition from the builder
                mMapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
            }
        });

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
                        Toast.makeText(CameraActivity.this, "Ease onCancel Callback called.", Toast.LENGTH_LONG).show();
                    }

                    @Override
                    public void onFinish() {
                        Toast.makeText(CameraActivity.this, "Ease onFinish Callback called.", Toast.LENGTH_LONG).show();
                    }
                };

                mMapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(cameraPosition), 25000, callback);
            }
        });

        findViewById(R.id.cameraAnimateButton).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                CameraPosition cameraPosition = new CameraPosition.Builder()
                        .target(new LatLng(-22.91214, -43.23012))     // Sets the center of the map to Maracanã
                        .bearing(270)                               // Sets the orientation of the camera to west
                        .tilt(20)                                   // Sets the tilt of the camera to 30 degrees
                        .build();                                   // Creates a CameraPosition from the builder

                MapboxMap.CancelableCallback callback = new MapboxMap.CancelableCallback() {
                    @Override
                    public void onCancel() {
                        Toast.makeText(CameraActivity.this, "Duration onCancel Callback called.", Toast.LENGTH_LONG).show();
                    }

                    @Override
                    public void onFinish() {
                        Toast.makeText(CameraActivity.this, "Duration onFinish Callback called.", Toast.LENGTH_LONG).show();
                    }
                };

                mMapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition), 25000, callback);
            }
        });
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
