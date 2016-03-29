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

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.annotations.CityStateMarker;
import com.mapbox.mapboxsdk.testapp.annotations.CityStateMarkerOptions;
import com.mapbox.mapboxsdk.testapp.utils.ApiAccess;
import com.mapbox.mapboxsdk.maps.MapView;

public class InfoWindowAdapterActivity extends AppCompatActivity {

    private MapView mMapView;
    private IconFactory mIconFactory;
    private Drawable mIconDrawable;

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

        mIconFactory = IconFactory.getInstance(this);
        mIconDrawable = ContextCompat.getDrawable(this, R.drawable.ic_location_city_24dp);

        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                // add markers
                mapboxMap.addMarker(generateCityStateMarker("Andorra", 42.505777, 1.52529, "#F44336"));
                mapboxMap.addMarker(generateCityStateMarker("Luxembourg", 49.815273, 6.129583, "#3F51B5"));
                mapboxMap.addMarker(generateCityStateMarker("Monaco", 43.738418, 7.424616, "#673AB7"));
                mapboxMap.addMarker(generateCityStateMarker("Vatican City", 41.902916, 12.453389, "#009688"));
                mapboxMap.addMarker(generateCityStateMarker("San Marino", 43.942360, 12.457777, "#795548"));
                mapboxMap.addMarker(generateCityStateMarker("Liechtenstein", 47.166000, 9.555373, "#FF5722"));

                // add custom window adapter
                mapboxMap.setInfoWindowAdapter(new MapboxMap.InfoWindowAdapter() {

                    private int tenDp = (int) getResources().getDimension(R.dimen.attr_margin);

                    @Override
                    public View getInfoWindow(@NonNull Marker marker) {
                        TextView textView = new TextView(InfoWindowAdapterActivity.this);
                        textView.setText(marker.getTitle());
                        textView.setTextColor(Color.WHITE);

                        if(marker instanceof CityStateMarker){
                            CityStateMarker cityStateMarker = (CityStateMarker)marker;
                            textView.setBackgroundColor(Color.parseColor(cityStateMarker.getInfoWindowBackgroundColor()));
                        }

                        textView.setPadding(tenDp, tenDp, tenDp, tenDp);
                        return textView;
                    }
                });
            }
        });
    }

    private CityStateMarkerOptions generateCityStateMarker(String title, double lat, double lng, String color) {
        CityStateMarkerOptions marker = new CityStateMarkerOptions();
        marker.title(title);
        marker.position(new LatLng(lat, lng));
        marker.infoWindowBackground(color);

        mIconDrawable.setColorFilter(Color.parseColor(color), PorterDuff.Mode.SRC_IN);
        Icon icon = mIconFactory.fromDrawable(mIconDrawable);
        marker.icon(icon);
        return marker;
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
