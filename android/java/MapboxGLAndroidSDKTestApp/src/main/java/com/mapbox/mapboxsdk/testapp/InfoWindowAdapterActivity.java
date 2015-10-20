package com.mapbox.mapboxsdk.testapp;

import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;

public class InfoWindowAdapterActivity extends AppCompatActivity {

    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_infowindow_adapter);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.setAccessToken(ApiAccess.getToken(this));
        mMapView.onCreate(savedInstanceState);
        mMapView.setInfoWindowAdapter(new MapView.InfoWindowAdapter() {

            private int tenDp = (int) getResources().getDimension(R.dimen.attr_margin);

            @Override
            public View getInfoWindow(@NonNull Marker marker) {
                TextView textView = new TextView(InfoWindowAdapterActivity.this);
                textView.setText(marker.getTitle());
                textView.setTextColor(Color.WHITE);
                textView.setBackgroundColor(Color.parseColor(marker.getSnippet()));
                textView.setPadding(tenDp, tenDp, tenDp, tenDp);
                return textView;
            }
        });

        mMapView.addMarker(generateMarker("Andorra", 42.505777, 1.52529, "#F44336"));
        mMapView.addMarker(generateMarker("Luxembourg", 49.815273, 6.129583, "#3F51B5"));
        mMapView.addMarker(generateMarker("Monaco", 43.738418, 7.424616, "#673AB7"));
        mMapView.addMarker(generateMarker("Vatican City", 41.902916, 12.453389, "#009688"));
        mMapView.addMarker(generateMarker("San Marino", 43.942360, 12.457777, "#795548"));
        mMapView.addMarker(generateMarker("Liechtenstein", 47.166000, 9.555373, "#FF5722"));
    }

    private MarkerOptions generateMarker(String title, double lat, double lng, String color) {
        MarkerOptions marker = new MarkerOptions();
        marker.title(title);
        marker.snippet(color);
        marker.position(new LatLng(lat, lng));
        return marker;
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
