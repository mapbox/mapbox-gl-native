package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Gravity;
import android.view.MenuItem;
import android.view.View;

import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.testapp.R;

public class ScaleOptionsActivity extends AppCompatActivity {

    private static final String TAG = ScaleOptionsActivity.class.getSimpleName();
    private static final String INDEX = "currentStyleIndex";

    private MapView mapView;
    private MapboxMap mapboxMap;

    private int currentStyleIndex = 0;

    private static final String[] STYLES = new String[]{
            Style.MAPBOX_STREETS,
            Style.OUTDOORS,
            Style.LIGHT,
            Style.DARK,
            Style.SATELLITE_STREETS,
            Style.SATELLITE,
    };

    private static final int GRAVITIES[] = {
            Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL,
            Gravity.BOTTOM | Gravity.LEFT,
            Gravity.TOP | Gravity.LEFT,
            Gravity.TOP | Gravity.RIGHT,
            Gravity.BOTTOM | Gravity.RIGHT,
            Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL,
    };

    private static final int COLORS[] = {
            MapboxConstants.SCALE_COLOR_BLACK,
            MapboxConstants.SCALE_COLOR_BLACK,
            MapboxConstants.SCALE_COLOR_BLACK,
            MapboxConstants.SCALE_COLOR_WHITE,
            MapboxConstants.SCALE_COLOR_WHITE,
            MapboxConstants.SCALE_COLOR_BLACK,
    };

    private static final float WIDTHS[] = {
            0.34f,
            0.4f,
            0.4f,
            0.5f,
            0.5f,
            0.34f,
    };

    private static UiSettings.ScaleUnit UNITS[] = {
            UiSettings.ScaleUnit.NM,
            UiSettings.ScaleUnit.KM,
            UiSettings.ScaleUnit.KM,
            UiSettings.ScaleUnit.MILE,
            UiSettings.ScaleUnit.MILE,
            UiSettings.ScaleUnit.KM,
    };

    private static boolean ENABLED[] = {
            true,
            true,
            true,
            true,
            true,
            false
    };

    private void setStyles() {
        UiSettings uiSettings = mapboxMap.getUiSettings();
        uiSettings.setScaleEnabled(ENABLED[currentStyleIndex]);
        uiSettings.setScaleUnit(UNITS[currentStyleIndex]);
        uiSettings.setScaleWidth(WIDTHS[currentStyleIndex]);
        uiSettings.setScaleGravity(GRAVITIES[currentStyleIndex]);
        uiSettings.setScaleColor(COLORS[currentStyleIndex]);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_scale_options);

        final Toolbar toolbar = (Toolbar)findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mapView = (MapView)findViewById(R.id.mapView);
        mapView.setTag(true);
        mapView.setStyleUrl(STYLES[currentStyleIndex]);
        mapView.onCreate(savedInstanceState);

        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap map) {
                mapboxMap = map;
                setStyles();
            }
        });

        FloatingActionButton fabStyles = (FloatingActionButton)findViewById(R.id.scaleChange);
        fabStyles.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mapboxMap != null) {
                    currentStyleIndex++;
                    if (currentStyleIndex == STYLES.length) {
                        currentStyleIndex = 0;
                    }
                    setStyles();
                    mapboxMap.setStyleUrl(STYLES[currentStyleIndex]);
                }
            }
        });
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
        if(savedInstanceState != null)
            currentStyleIndex = savedInstanceState.getInt(INDEX, 0);
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
        outState.putInt(INDEX, currentStyleIndex);
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
