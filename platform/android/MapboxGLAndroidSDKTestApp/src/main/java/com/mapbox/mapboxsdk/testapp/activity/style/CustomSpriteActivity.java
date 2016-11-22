package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.Drawable;
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

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.services.commons.geojson.Feature;
import com.mapbox.services.commons.geojson.FeatureCollection;
import com.mapbox.services.commons.geojson.Point;
import com.mapbox.services.commons.models.Position;

import java.net.MalformedURLException;
import java.net.URL;

import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleRadius;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;

/**
 * Example to add a sprite image and use it in a Symbol Layer
 */
public class CustomSpriteActivity extends AppCompatActivity {
    private static final String TAG = CustomSpriteActivity.class.getSimpleName();
    private static final String CUSTOM_ICON = "custom-icon";

    private MapboxMap mapboxMap;
    private MapView mapView;
    private Layer layer;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_add_sprite);

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
                final FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
                fab.setColorFilter(ContextCompat.getColor(CustomSpriteActivity.this, R.color.primary));
                fab.setOnClickListener(new View.OnClickListener() {
                    private Point point;

                    @Override
                    public void onClick(View view) {
                        if (point == null) {
                            Log.i(TAG, "First click -> Car");
                            // Add an icon to reference later
                            mapboxMap.addImage(CUSTOM_ICON, BitmapFactory.decodeResource(getResources(), R.drawable.ic_car_top));

                            //Add a source with a geojson point
                            point = Point.fromCoordinates(Position.fromCoordinates(13.400972d, 52.519003d));
                            mapboxMap.addSource(new GeoJsonSource("point", FeatureCollection.fromFeatures(new Feature[]{Feature.fromGeometry(point)})));

                            //Add a symbol layer that references that point source
                            layer = new SymbolLayer("layer", "point");
                            layer.setProperties(
                                    //Set the id of the sprite to use
                                    iconImage(CUSTOM_ICON)
                            );

                            // lets add a circle below labels!
                            mapboxMap.addLayer(layer, "waterway-label");

                            fab.setImageResource(R.drawable.ic_directions_car_black_24dp);
                        } else {
                            //Update point
                            point = Point.fromCoordinates(Position.fromCoordinates(point.getCoordinates().getLongitude() + 0.001, point.getCoordinates().getLatitude() + 0.001));
                            GeoJsonSource source = mapboxMap.getSourceAs("point");
                            source.setGeoJson(FeatureCollection.fromFeatures(new Feature[]{Feature.fromGeometry(point)}));

                            //Move the camera as well
                            mapboxMap.moveCamera(CameraUpdateFactory.newLatLng(new LatLng(point.getCoordinates().getLatitude(), point.getCoordinates().getLongitude())));
                        }
                    }
                });
            }
        });
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