package com.mapbox.mapboxsdk.testapp.activity.style;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.content.ContextCompat;
import android.support.v4.content.res.ResourcesCompat;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;

import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;

import java.net.MalformedURLException;
import java.net.URL;

import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleRadius;

/**
 * Example to add a circle as a layer using runtime style API, contains adding
 * GeoJSON as source that contains 1 Feature with a Point geometry, adding a circle layer and
 * changing circle layer properties as size and colors.
 */
public class CircleLayerActivity extends AppCompatActivity {

    private MapboxMap mapboxMap;
    private MapView mapView;
    private Layer circleLayer;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_circlelayer);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mapView = (MapView) findViewById(R.id.mapView);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull final MapboxMap map) {
                mapboxMap = map;
                FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
                fab.setColorFilter(ContextCompat.getColor(CircleLayerActivity.this, R.color.primary));
                fab.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        if (circleLayer == null) {
                            // first time
                            try {
                                mapboxMap.addSource(new GeoJsonSource("point", new URL("https://gist.githubusercontent.com/anonymous/87eca90e80a72b1b42be9d0201ec3c8e/raw/acbb46384fd56044a504f122950d0637d98b4e7a/map.geojson")));
                            } catch (MalformedURLException malformedUrlException) {
                                Log.e(
                                    MapboxConstants.TAG,
                                    "That's not an url... " + malformedUrlException.getMessage()
                                );
                            }

                            circleLayer = new CircleLayer("circleLayer", "point");
                            circleLayer.setProperties(
                                    circleColor(ResourcesCompat.getColor(getResources(), R.color.primary_dark, getTheme())),
                                    circleRadius(getResources().getDimension(R.dimen.circle_size))
                            );

                            // lets add a circle below labels!
                            mapboxMap.addLayer(circleLayer, "waterway-label");
                        } else {
                            // change size and color
                            circleLayer.setProperties(
                                    circleRadius(mapView.getTag() == null
                                        ? getResources().getDimension(R.dimen.activity_horizontal_margin)
                                        : getResources().getDimension(R.dimen.circle_size)),
                                    circleColor(mapView.getTag() == null ? ResourcesCompat.getColor(
                                        getResources(), R.color.blue_accent, getTheme()) : ResourcesCompat.getColor(
                                        getResources(), R.color.green_accent, getTheme())));
                            mapView.setTag(mapView.getTag() == null ? mapboxMap : null);
                        }
                    }
                });
            }
        });
    }

    @Override
    protected void onStart() {
        super.onStart();
        mapView.onStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mapView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mapView.onPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
        mapView.onStop();
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mapView.onSaveInstanceState(outState);
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mapView.onLowMemory();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mapView.onDestroy();
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