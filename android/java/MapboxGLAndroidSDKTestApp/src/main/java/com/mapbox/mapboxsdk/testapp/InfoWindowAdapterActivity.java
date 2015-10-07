package com.mapbox.mapboxsdk.testapp;

import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.views.MapView;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class InfoWindowAdapterActivity extends AppCompatActivity {

    private static final String TAG = "InfoWindowAdapter";

    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_second);

        Toolbar toolbar = (Toolbar) findViewById(R.id.secondToolBar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if(actionBar!=null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.secondMapView);
        mMapView.setAccessToken(Util.getAccessToken(this));
        mMapView.onCreate(savedInstanceState);

        mMapView.setStyleUrl(MapView.StyleUrls.EMERALD);

        mMapView.setInfoWindowAdapter(new MapView.InfoWindowAdapter() {

            @Override
            public View getInfoWindow(Marker marker) {
                TextView textView = new TextView(InfoWindowAdapterActivity.this);
                textView.setText(marker.getTitle());
                textView.setBackgroundColor(Color.WHITE);
                return textView;
            }
        });

        mMapView.addMarker(generateMarker("Andorra", 42.505777, 1.525294));
        mMapView.addMarker(generateMarker("Luxembourg", 49.815273, 6.129583));
        mMapView.addMarker(generateMarker("Monaco", 43.738418, 7.424616));
        mMapView.addMarker(generateMarker("Vatican City", 41.902916, 12.453389));
        mMapView.addMarker(generateMarker("San Marino", 43.942360, 12.457777));
        mMapView.addMarker(generateMarker("Malta", 35.892110, 14.427795));
        mMapView.addMarker(generateMarker("Liechtenstein", 47.166000, 9.555373));

        mMapView.setCenterCoordinate(new LatLng(47.798202, 3.573781));
        mMapView.setZoomLevel(3);
    }

    private MarkerOptions generateMarker(String title, double lat, double lng){
        MarkerOptions marker = new MarkerOptions();
        marker.title(title);
        marker.snippet(title);
        marker.position(new LatLng(lat, lng));
        return marker;
    }

    /**
     * Dispatch onStart() to all fragments.  Ensure any created loaders are
     * now started.
     */
    @Override
    protected void onStart() {
        super.onStart();
        mMapView.onStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
        mMapView.onStop();
    }

    // Called when our app goes into the background
    @Override
    public void onPause()  {
        super.onPause();

        mMapView.onPause();
    }

    // Called when our app comes into the foreground
    @Override
    public void onResume() {
        super.onResume();

        mMapView.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMapView.onDestroy();
    }

    // Called when the system is running low on memory
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
