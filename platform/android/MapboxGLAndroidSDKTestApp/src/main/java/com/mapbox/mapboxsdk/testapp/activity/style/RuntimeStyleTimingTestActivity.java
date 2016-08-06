package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.sources.VectorSource;
import com.mapbox.mapboxsdk.testapp.R;

import static com.mapbox.mapboxsdk.style.layers.Property.*;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.*;

/**
 * Test activity for unit test execution
 */
public class RuntimeStyleTimingTestActivity extends AppCompatActivity {
    private static final String TAG = RuntimeStyleTimingTestActivity.class.getSimpleName();

    public MapView mapView;
    private MapboxMap mapboxMap;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_runtime_style);

        //Initialize map as normal
        mapView = (MapView) findViewById(R.id.mapView);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(MapboxMap mapboxMap) {
                RuntimeStyleTimingTestActivity.this.mapboxMap = mapboxMap;
                VectorSource museums = new VectorSource("museums_source", "mapbox://mapbox.2opop9hr");
                mapboxMap.addSource(museums);

                CircleLayer museumsLayer = new CircleLayer("museums", "museums_source");
                museumsLayer.setSourceLayer("museum-cusco");
                museumsLayer.setProperties(
                        visibility(VISIBLE),
                        circleRadius(8f),
                        circleColor(Color.argb(1, 55, 148, 179))
                );

                mapboxMap.addLayer(museumsLayer);
            }
        });
    }

    public MapboxMap getMapboxMap() {
        return mapboxMap;
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
