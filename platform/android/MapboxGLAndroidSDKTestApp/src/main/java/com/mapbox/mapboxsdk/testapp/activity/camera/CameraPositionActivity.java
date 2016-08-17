package com.mapbox.mapboxsdk.testapp.activity.camera;

import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.annotation.IdRes;
import android.support.annotation.NonNull;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

public class CameraPositionActivity extends AppCompatActivity implements OnMapReadyCallback {

    private MapView mapView;
    private MapboxMap mapboxMap;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_position);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mapView = (MapView) findViewById(R.id.mapView);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(this);
    }

    @Override
    public void onMapReady(@NonNull final MapboxMap mapboxMap) {
        this.mapboxMap = mapboxMap;
        // add a listener to FAB
        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setColorFilter(ContextCompat.getColor(CameraPositionActivity.this, R.color.primary));
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Context context = v.getContext();
                final View dialogContent = LayoutInflater.from(context).inflate(R.layout.dialog_camera_position, null);
                AlertDialog.Builder builder = new AlertDialog.Builder(context, com.mapbox.mapboxsdk.R.style.AttributionAlertDialogStyle);
                builder.setTitle(R.string.dialog_camera_position);
                builder.setView(onInflateDialogContent(dialogContent));
                builder.setPositiveButton("Animate", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        double latitude = Double.parseDouble(((TextView) dialogContent.findViewById(R.id.value_lat)).getText().toString());
                        double longitude = Double.parseDouble(((TextView) dialogContent.findViewById(R.id.value_lon)).getText().toString());
                        double zoom = Double.parseDouble(((TextView) dialogContent.findViewById(R.id.value_zoom)).getText().toString());
                        double bearing = Double.parseDouble(((TextView) dialogContent.findViewById(R.id.value_bearing)).getText().toString());
                        double tilt = Double.parseDouble(((TextView) dialogContent.findViewById(R.id.value_tilt)).getText().toString());

                        CameraPosition cameraPosition = new CameraPosition.Builder()
                                .target(new LatLng(latitude, longitude))
                                .zoom(zoom)
                                .bearing(bearing)
                                .tilt(tilt)
                                .build();

                        mapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition), 5000);
                        Log.v(MapboxConstants.TAG, cameraPosition.toString());
                    }
                });
                builder.setNegativeButton("Cancel", null);
                builder.setCancelable(false);
                builder.show();
            }
        });
    }

    @Override
    public void onPause() {
        super.onPause();
        mapView.onPause();
    }

    @Override
    public void onResume() {
        super.onResume();
        mapView.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mapView.onDestroy();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mapView.onSaveInstanceState(outState);
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

    private View onInflateDialogContent(View view) {
        linkTextView(view, R.id.value_lat, R.id.seekbar_lat, new LatLngChangeListener(), 180 + 38);
        linkTextView(view, R.id.value_lon, R.id.seekbar_lon, new LatLngChangeListener(), 180 - 77);
        linkTextView(view, R.id.value_zoom, R.id.seekbar_zoom, new ValueChangeListener(), 6);
        linkTextView(view, R.id.value_bearing, R.id.seekbar_bearing, new ValueChangeListener(), 90);
        linkTextView(view, R.id.value_tilt, R.id.seekbar_tilt, new ValueChangeListener(), 40);
        return view;
    }

    private void linkTextView(View view, @IdRes int textViewRes, @IdRes int seekBarRes, ValueChangeListener listener, int defaultValue) {
        final TextView value = (TextView) view.findViewById(textViewRes);
        SeekBar seekBar = (SeekBar) view.findViewById(seekBarRes);
        listener.setLinkedValueView(value);
        seekBar.setOnSeekBarChangeListener(listener);
        seekBar.setProgress(defaultValue);
    }

    private class ValueChangeListener implements SeekBar.OnSeekBarChangeListener {

        protected TextView textView;

        public void setLinkedValueView(TextView textView) {
            this.textView = textView;
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {

        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {

        }

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            textView.setText(String.valueOf(progress));
        }
    }

    private class LatLngChangeListener extends ValueChangeListener {

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            super.onProgressChanged(seekBar, progress - 180, fromUser);
        }
    }
}
