package com.mapbox.mapboxsdk.testapp.activity.camera;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.StringRes;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.testapp.R;

public class ScrollByActivity extends AppCompatActivity implements OnMapReadyCallback {

    public static final int MULTIPLIER_PER_PIXEL = 50;

    private MapView mapView;
    private MapboxMap mapboxMap;
    private SeekBar xBar, yBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_scroll_by);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        xBar = (SeekBar) findViewById(R.id.seekbar_move_x);
        TextView xText = (TextView) findViewById(R.id.textview_x);
        xBar.setOnSeekBarChangeListener(new PixelBarChangeListener(xText, R.string.scrollby_x_value));

        yBar = (SeekBar) findViewById(R.id.seekbar_move_y);
        TextView yText = (TextView) findViewById(R.id.textview_y);
        yBar.setOnSeekBarChangeListener(new PixelBarChangeListener(yText, R.string.scrollby_y_value));

        mapView = (MapView) findViewById(R.id.mapView);
        mapView.setTag(true);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(this);
    }

    @Override
    public void onMapReady(MapboxMap map) {
        mapboxMap = map;
        UiSettings uiSettings = mapboxMap.getUiSettings();
        uiSettings.setLogoEnabled(false);
        uiSettings.setAttributionEnabled(false);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setColorFilter(ContextCompat.getColor(ScrollByActivity.this, R.color.primary));
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mapboxMap.easeCamera(CameraUpdateFactory.scrollBy(
                        xBar.getProgress() * MULTIPLIER_PER_PIXEL,
                        yBar.getProgress() * MULTIPLIER_PER_PIXEL)
                );
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

    private static class PixelBarChangeListener implements SeekBar.OnSeekBarChangeListener {

        @StringRes
        private int mPrefixTextResource;
        private TextView mValueView;

        public PixelBarChangeListener(@NonNull TextView textView, @StringRes int textRes) {
            mValueView = textView;
            mPrefixTextResource = textRes;
        }

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            int value = progress * ScrollByActivity.MULTIPLIER_PER_PIXEL;
            mValueView.setText(String.format(seekBar.getResources().getString(mPrefixTextResource), value));
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {

        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {

        }
    }
}