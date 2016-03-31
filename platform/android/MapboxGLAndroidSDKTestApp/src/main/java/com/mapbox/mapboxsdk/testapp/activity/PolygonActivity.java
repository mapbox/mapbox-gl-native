package com.mapbox.mapboxsdk.testapp.activity;

import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.mapboxsdk.annotations.PolygonOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;
import java.util.List;

public class PolygonActivity extends AppCompatActivity {

    private MapView mapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // configure inital map state
        MapboxMapOptions options = new MapboxMapOptions()
                .accessToken(getString(R.string.mapbox_access_token))
                .styleUrl(Style.MAPBOX_STREETS)
                .camera(new CameraPosition.Builder()
                        .target(new LatLng(45.520486, -122.673541))
                        .zoom(12)
                        .tilt(40)
                        .build());

        // create map
        mapView = new MapView(this, options);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(MapboxMap mapboxMap) {
                List<LatLng> polygon = new ArrayList<>();
                polygon.add(new LatLng(45.522585, -122.685699));
                polygon.add(new LatLng(45.534611, -122.708873));
                polygon.add(new LatLng(45.530883, -122.678833));
                polygon.add(new LatLng(45.547115, -122.667503));
                polygon.add(new LatLng(45.530643, -122.660121));
                polygon.add(new LatLng(45.533529, -122.636260));
                polygon.add(new LatLng(45.521743, -122.659091));
                polygon.add(new LatLng(45.510677, -122.648792));
                polygon.add(new LatLng(45.515008, -122.664070));
                polygon.add(new LatLng(45.502496, -122.669048));
                polygon.add(new LatLng(45.515369, -122.678489));
                polygon.add(new LatLng(45.506346, -122.702007));
                polygon.add(new LatLng(45.522585, -122.685699));
                mapboxMap.addPolygon(new PolygonOptions()
                        .addAll(polygon)
                        .fillColor(Color.parseColor("#3bb2d0")));
            }
        });

        // set as content view
        setContentView(mapView);
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
}
