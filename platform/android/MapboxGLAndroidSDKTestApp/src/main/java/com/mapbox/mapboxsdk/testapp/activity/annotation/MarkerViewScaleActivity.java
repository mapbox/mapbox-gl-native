package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.annotations.MarkerViewOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.Locale;

public class MarkerViewScaleActivity extends AppCompatActivity implements OnMapReadyCallback {

    private MapboxMap mapboxMap;
    private MapView mapView;

    private MarkerView markerView;
    private MarkerViewManager markerViewManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_marker_view_scale);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        final ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mapView = (MapView) findViewById(R.id.mapView);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(this);
    }

    @Override
    public void onMapReady(MapboxMap map) {
        mapboxMap = map;
        markerViewManager = map.getMarkerViewManager();

        final SeekBar xBar = (SeekBar) findViewById(R.id.seekbar_factor);
        final TextView textView = (TextView) findViewById(R.id.textview_factor);

        // We need to listen to a render event to be sure
        // the View of the Marker has been added to the map
        mapView.addOnMapChangedListener(new MapView.OnMapChangedListener() {
            @Override
            public void onMapChanged(@MapView.MapChange int change) {
                if (isMarkerRendered()) {
                    Toast.makeText(MarkerViewScaleActivity.this, "MarkerView is ready", Toast.LENGTH_SHORT).show();
                    View view = markerViewManager.getView(markerView);
                    xBar.setOnSeekBarChangeListener(new FactorChangeListener(view, textView));
                    xBar.setClickable(true);
                    mapView.removeOnMapChangedListener(this);
                }
            }

            private boolean isMarkerRendered() {
                return markerView != null && markerViewManager.getView(markerView) != null;
            }
        });

        Icon icon = IconFactory.getInstance(MarkerViewScaleActivity.this)
                .fromResource(R.drawable.ic_circle);

        markerView = mapboxMap.addMarker(new MarkerViewOptions()
                .position(new LatLng(38.907192, -77.036871))
                .icon(icon)
                .flat(true));
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
        mapView.onResume();
    }

    @Override
    public void onStart() {
        super.onResume();
        mapView.onStart();
    }

    @Override
    public void onStop(){
        super.onStop();
        mapView.onStop();
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

    private static class FactorChangeListener implements SeekBar.OnSeekBarChangeListener {

        private TextView textView;
        private View view;

        FactorChangeListener(View view, TextView textView) {
            this.view = view;
            this.textView = textView;
        }

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            float newScale = getScale(progress);
            textView.setText(String.format(Locale.US, "Scale: %.1f", newScale));
            if (view != null) {
                view.setScaleX(newScale);
                view.setScaleY(newScale);
            }
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
            // Not used
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            // Not used
        }

        private float getScale(int progress) {
            float scale = 1.0f * progress / 25;
            return scale < 1.0 ? 1.0f : scale;
        }
    }

}