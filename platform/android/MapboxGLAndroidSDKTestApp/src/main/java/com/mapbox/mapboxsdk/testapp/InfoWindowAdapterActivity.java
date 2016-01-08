package com.mapbox.mapboxsdk.testapp;

import android.graphics.Color;
import android.graphics.PorterDuff;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;

import com.mapbox.mapboxsdk.MapFragment;
import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngZoom;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;
import com.mapbox.mapboxsdk.views.MapboxMap;
import com.mapbox.mapboxsdk.views.OnMapReadyCallback;

public class InfoWindowAdapterActivity extends AppCompatActivity {

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

        MapFragment mapFragment = (MapFragment) getSupportFragmentManager().findFragmentById(R.id.mapFragment);
        mapFragment.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                mapboxMap.setStyle(Style.MAPBOX_STREETS);
                mapboxMap.setLatLng(new LatLngZoom(47.798202, 7.573781, 4));
                mapboxMap.setInfoWindowAdapter(new MapboxMap.InfoWindowAdapter() {

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
                mapboxMap.addMarker(generateMarker("Andorra", 42.505777, 1.52529, "#F44336"));
                mapboxMap.addMarker(generateMarker("Luxembourg", 49.815273, 6.129583, "#3F51B5"));
                mapboxMap.addMarker(generateMarker("Monaco", 43.738418, 7.424616, "#673AB7"));
                mapboxMap.addMarker(generateMarker("Vatican City", 41.902916, 12.453389, "#009688"));
                mapboxMap.addMarker(generateMarker("San Marino", 43.942360, 12.457777, "#795548"));
                mapboxMap.addMarker(generateMarker("Liechtenstein", 47.166000, 9.555373, "#FF5722"));
            }
        });
    }

    private MarkerOptions generateMarker(String title, double lat, double lng, String color) {
        MarkerOptions marker = new MarkerOptions();
        marker.title(title);
        marker.snippet(color);
        marker.position(new LatLng(lat, lng));

        Drawable drawable = ContextCompat.getDrawable(this, R.drawable.ic_location_city_24dp);
        drawable.setColorFilter(Color.parseColor(marker.getSnippet()), PorterDuff.Mode.SRC_IN);
        Icon icon = IconFactory.getInstance(this).fromDrawable(drawable);
        marker.icon(icon);
        return marker;
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
