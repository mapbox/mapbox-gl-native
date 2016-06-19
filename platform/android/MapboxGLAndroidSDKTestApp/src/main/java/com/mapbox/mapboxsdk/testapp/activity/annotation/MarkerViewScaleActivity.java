package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.annotations.MarkerViewOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.Locale;

public class MarkerViewScaleActivity extends AppCompatActivity {

    private MapView mMapView;
    private View mMarkerView;

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

        final SeekBar xBar = (SeekBar) findViewById(R.id.seekbar_factor);
        TextView xText = (TextView) findViewById(R.id.textview_factor);
        xBar.setOnSeekBarChangeListener(new FactorChangeListener(xText));

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(MapboxMap mapboxMap) {
                Icon icon = IconFactory.getInstance(MarkerViewScaleActivity.this)
                        .fromResource(R.drawable.ic_circle);

                MarkerView mMarker = mapboxMap.addMarker(new MarkerViewOptions()
                        .position(new LatLng(38.907192, -77.036871))
                        .icon(icon)
                        .flat(true));

                mMarkerView = mapboxMap.getMarkerViewManager().getView(mMarker);
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

    private class FactorChangeListener implements SeekBar.OnSeekBarChangeListener {

        private TextView xText;

        public FactorChangeListener(TextView xText) {
            this.xText = xText;
        }

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            float newScale = getScale(progress);
            xText.setText(String.format(Locale.US, "Scale: %.1f", newScale));
            if (MarkerViewScaleActivity.this.mMarkerView != null) {
                mMarkerView.setScaleX(newScale);
                mMarkerView.setScaleY(newScale);

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
