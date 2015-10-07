package com.mapbox.mapboxsdk.testapp;

import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.views.MapView;

public class InfoWindowAdapterActivity extends AppCompatActivity {

    private static final String TAG = "InfoWindowAdapter";

    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_infowindow_adapter);

        Toolbar toolbar = (Toolbar) findViewById(R.id.secondToolBar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.secondMapView);
        mMapView.setAccessToken(Util.getAccessToken(this));
        mMapView.onCreate(savedInstanceState);
        mMapView.setStyleUrl(MapView.StyleUrls.MAPBOX_STREETS);
        mMapView.setInfoWindowAdapter(new MapView.InfoWindowAdapter() {

            private int tenDp = (int) getResources().getDimension(R.dimen.attr_margin);

            @Override
            public View getInfoWindow(Marker marker) {
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

        mMapView.setCenterCoordinate(new LatLng(47.798202, 7.573781));
        mMapView.setZoomLevel(4);
    }

    private MarkerOptions generateMarker(String title, double lat, double lng, String color) {
        MarkerOptions marker = new MarkerOptions();
        marker.title(title);
        marker.snippet(color);
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
    public void onPause() {
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
