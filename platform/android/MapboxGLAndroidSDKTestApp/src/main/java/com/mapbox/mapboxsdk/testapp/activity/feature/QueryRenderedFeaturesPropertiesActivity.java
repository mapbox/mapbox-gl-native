package com.mapbox.mapboxsdk.testapp.activity.feature;

import android.graphics.Color;
import android.graphics.PointF;
import android.os.Bundle;
import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.google.gson.JsonElement;
import com.mapbox.mapboxsdk.annotations.BaseMarkerOptions;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.services.commons.geojson.Feature;

import java.util.List;
import java.util.Map;

/**
 * Demo's query rendered features
 */
public class QueryRenderedFeaturesPropertiesActivity extends AppCompatActivity {
    private static final String TAG = QueryRenderedFeaturesPropertiesActivity.class.getSimpleName();

    public MapView mapView;
    private MapboxMap mapboxMap;
    private Marker marker;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_query_features_point);
        setupActionBar();

        final float density = getResources().getDisplayMetrics().density;

        //Initialize map as normal
        mapView = (MapView) findViewById(R.id.mapView);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(final MapboxMap mapboxMap) {
                QueryRenderedFeaturesPropertiesActivity.this.mapboxMap = mapboxMap;

                //Add custom window adapter
                addCustomInfoWindowAdapter(mapboxMap);

                //Add a click listener
                mapboxMap.setOnMapClickListener(new MapboxMap.OnMapClickListener() {
                    @Override
                    public void onMapClick(@NonNull LatLng point) {
                        //Query
                        final PointF pixel = mapboxMap.getProjection().toScreenLocation(point);
                        Log.i(TAG, String.format("Requesting features for %sx%s (%sx%s adjusted for density)", pixel.x, pixel.y, pixel.x / density, pixel.y / density));
                        List<Feature> features = mapboxMap.queryRenderedFeatures(pixel);

                        //Debug output
                        debugOutput(features);

                        //Remove any previous markers
                        if (marker != null) {
                            mapboxMap.removeMarker(marker);
                        }

                        //Add a marker on the clicked point
                        marker = mapboxMap.addMarker(new CustomMarkerOptions().position(point).features(features));
                        mapboxMap.selectMarker(marker);
                    }
                });

                //Little taste of home
                mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(52.0907, 5.1214), 16));
            }
        });

    }

    private void debugOutput(List<Feature> features) {
        Log.i(TAG, String.format("Got %s features", features.size()));
        for (Feature feature : features) {
            if (feature != null) {
                Log.i(TAG, String.format("Got feature %s with %s properties and Geometry %s",
                        feature.getId(),
                        feature.getProperties() != null ? feature.getProperties().entrySet().size() : "<null>",
                        feature.getGeometry() != null ? feature.getGeometry().getClass().getSimpleName() : "<null>")
                );
                if (feature.getProperties() != null) {
                    for (Map.Entry<String, JsonElement> entry : feature.getProperties().entrySet()) {
                        Log.i(TAG, String.format("Prop %s - %s", entry.getKey(), entry.getValue()));
                    }
                }
            } else {
                Log.i(TAG, "Got NULL feature %s");
            }
        }
    }

    private void addCustomInfoWindowAdapter(MapboxMap mapboxMap) {
        mapboxMap.setInfoWindowAdapter(new MapboxMap.InfoWindowAdapter() {

            private TextView row(String text) {
                TextView view = new TextView(QueryRenderedFeaturesPropertiesActivity.this);
                view.setText(text);
                return view;
            }

            private int tenDp = (int) getResources().getDimension(R.dimen.attr_margin);

            @Override
            public View getInfoWindow(@NonNull Marker marker) {
                CustomMarker customMarker = (CustomMarker) marker;
                LinearLayout view = new LinearLayout(QueryRenderedFeaturesPropertiesActivity.this);
                view.setOrientation(LinearLayout.VERTICAL);
                view.setBackgroundColor(Color.WHITE);

                if (customMarker.features.size() > 0) {
                    view.addView(row(String.format("Found %s features", customMarker.features.size())));
                    Feature feature = customMarker.features.get(0);
                    for (Map.Entry<String, JsonElement> prop : feature.getProperties().entrySet()) {
                        view.addView(row(String.format("%s: %s", prop.getKey(), prop.getValue())));
                    }
                } else {
                    view.addView(row("No features here"));
                }

                return view;
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

    private void setupActionBar() {
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        final ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }
    }

    private static class CustomMarker extends Marker {

        private final List<Feature> features;

        public CustomMarker(BaseMarkerOptions baseMarkerOptions, List<Feature> features) {
            super(baseMarkerOptions);
            this.features = features;
        }
    }

    private static class CustomMarkerOptions extends BaseMarkerOptions<CustomMarker, CustomMarkerOptions> {


        private List<Feature> features;

        public CustomMarkerOptions features(List<Feature> features) {
            this.features = features;
            return this;
        }

        public CustomMarkerOptions() {
        }

        private CustomMarkerOptions(Parcel in) {
            //Should implement this
        }

        @Override
        public CustomMarkerOptions getThis() {
            return this;
        }

        @Override
        public CustomMarker getMarker() {
            return new CustomMarker(this, features);
        }

        public static final Parcelable.Creator<CustomMarkerOptions> CREATOR
                = new Parcelable.Creator<CustomMarkerOptions>() {
            public CustomMarkerOptions createFromParcel(Parcel in) {
                return new CustomMarkerOptions(in);
            }

            public CustomMarkerOptions[] newArray(int size) {
                return new CustomMarkerOptions[size];
            }
        };

        @Override
        public int describeContents() {
            return 0;
        }

        @Override
        public void writeToParcel(Parcel out, int flags) {
            //Should implement this
        }
    }
}
