package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

public class DebugModeActivity  extends AppCompatActivity {

    private static final String TAG = "DebugModeActivity";

    private MapView mapView;
    private MapboxMap mapboxMap;

    private String currentStyle = Style.MAPBOX_STREETS;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_debug_mode);

        final Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mapView = (MapView) findViewById(R.id.mapView);
        mapView.setTag(true);
        mapView.setAccessToken(getString(R.string.mapbox_access_token));
        mapView.onCreate(savedInstanceState);

        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap map) {
                mapboxMap = map;
            }
        });

        FloatingActionButton fabDebug = (FloatingActionButton) findViewById(R.id.fabDebug);
        fabDebug.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mapboxMap != null) {
                    Log.d(TAG, "Debug FAB: isDebug Active? " + mapboxMap.isDebugActive());
                    mapboxMap.cycleDebugOptions();
                }
            }
        });

        FloatingActionButton fabStyles = (FloatingActionButton) findViewById(R.id.fabStyles);
        fabStyles.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (currentStyle.equals(Style.MAPBOX_STREETS)) {
                    currentStyle = Style.EMERALD;
                } else if (currentStyle.equals(Style.EMERALD)) {
                    currentStyle = Style.DARK;
                } else if (currentStyle.equals(Style.DARK)) {
                    currentStyle = Style.LIGHT;
                } else if (currentStyle.equals(Style.LIGHT)) {
                    currentStyle = Style.SATELLITE;
                } else if (currentStyle.equals(Style.SATELLITE)) {
                    currentStyle = Style.SATELLITE_STREETS;
                } else {
                    currentStyle = Style.MAPBOX_STREETS;
                }
                mapboxMap.setStyle(currentStyle);
            }
        });
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
