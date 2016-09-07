package com.mapbox.mapboxsdk.testapp.activity.camera;

import android.graphics.Point;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.testapp.R;

public class ManualZoomActivity extends AppCompatActivity {

    private MapboxMap mapboxMap;
    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_manual_zoom);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.setStyleUrl(Style.SATELLITE);
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull final MapboxMap mapboxMap) {
                ManualZoomActivity.this.mapboxMap = mapboxMap;

                UiSettings uiSettings = ManualZoomActivity.this.mapboxMap.getUiSettings();
                uiSettings.setAllGesturesEnabled(false);
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
//        findViewById(R.id.)
        getMenuInflater().inflate(R.menu.menu_zoom, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {

            case android.R.id.home:
                onBackPressed();
                return true;

            case R.id.action_zoom_in:
                mapboxMap.animateCamera(CameraUpdateFactory.zoomIn());
                return true;

            case R.id.action_zoom_out:
                mapboxMap.animateCamera(CameraUpdateFactory.zoomOut());
                return true;

            case R.id.action_zoom_by:
                mapboxMap.animateCamera(CameraUpdateFactory.zoomBy(2));
                return true;
            case R.id.action_zoom_to:
                mapboxMap.animateCamera(CameraUpdateFactory.zoomTo(2));
                return true;

            case R.id.action_zoom_to_point:
                View view = getWindow().getDecorView();
                mapboxMap.animateCamera(CameraUpdateFactory.zoomBy(1, new Point(view.getMeasuredWidth() / 4, view.getMeasuredHeight() / 4)));
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
}
