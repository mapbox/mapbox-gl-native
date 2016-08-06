package com.mapbox.mapboxsdk.testapp.activity.customlayer;

import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.style.layers.CustomLayer;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.layers.NoSuchLayerException;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.model.customlayer.ExampleCustomLayer;

public class CustomLayerActivity extends AppCompatActivity {
    private static final String TAG = CustomLayerActivity.class.getSimpleName();

    private MapboxMap mapboxMap;
    private MapView mapView;

    private boolean isShowingCustomLayer = false;
    private FloatingActionButton fab;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_custom_layer);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        final ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mapView = (MapView) findViewById(R.id.mapView);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(MapboxMap map) {
                mapboxMap = map;

                mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(39.91448, -243.60947), 10));

            }
        });

        fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setColorFilter(ContextCompat.getColor(this, R.color.primary));
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mapboxMap != null) {
                    swapCustomLayer();
                }
            }
        });
    }

    private void swapCustomLayer() {

        if (isShowingCustomLayer) {
            try {
                mapboxMap.removeLayer("custom");
            } catch (NoSuchLayerException e) {
                Log.e(TAG, "No custom layer to remove");
            }
            fab.setImageResource(R.drawable.ic_layers_24dp);
        } else {
            mapboxMap.addLayer(new CustomLayer("custom",
                    ExampleCustomLayer.createContext(),
                    ExampleCustomLayer.InitializeFunction,
                    ExampleCustomLayer.RenderFunction,
                    ExampleCustomLayer.DeinitializeFunction), null);
            fab.setImageResource(R.drawable.ic_layers_clear_24dp);
        }

        isShowingCustomLayer = !isShowingCustomLayer;
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
}
