package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.RawRes;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.layers.Function;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.LineLayer;
import com.mapbox.mapboxsdk.style.layers.NoSuchLayerException;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.PropertyValue;
import com.mapbox.mapboxsdk.style.layers.RasterLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.style.sources.RasterSource;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.mapboxsdk.style.sources.TileSet;
import com.mapbox.mapboxsdk.style.sources.VectorSource;
import com.mapbox.mapboxsdk.testapp.R;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;
import java.io.Writer;

import static com.mapbox.mapboxsdk.style.layers.Filter.*;
import static com.mapbox.mapboxsdk.style.layers.Function.*;
import static com.mapbox.mapboxsdk.style.layers.Property.*;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.*;

/**
 * Sample Activity to show a typical location picker use case
 */
public class RuntimeStyleActivity extends AppCompatActivity {
    private static final String TAG = RuntimeStyleActivity.class.getSimpleName();

    private MapView mapView;
    private MapboxMap mapboxMap;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_runtime_style);

        setupActionBar();

        //Initialize map as normal
        mapView = (MapView) findViewById(R.id.mapView);
        mapView.onCreate(savedInstanceState);


        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(MapboxMap map) {
                //Store for later
                mapboxMap = map;

                //Center and Zoom (Amsterdam, zoomed to streets)
                mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(52.379189, 4.899431), 14));
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_runtime_style, menu);
        return true;
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
            case R.id.action_water_color:
                setWaterColor();
                return true;
            case R.id.action_background_opacity:
                setBackgroundOpacity();
                return true;
            case R.id.action_road_avoid_edges:
                setRoadSymbolPlacement();
                return true;
            case R.id.action_layer_visibility:
                setLayerInvisible();
                return true;
            case R.id.action_remove_layer:
                removeBuildings();
                return true;
            case R.id.action_add_parks_layer:
                addParksLayer();
                return true;
            case R.id.action_add_terrain_layer:
                addTerrainLayer();
                return true;
            case R.id.action_add_satellite_layer:
                addSatelliteLayer();
                return true;
            case R.id.action_update_water_color_on_zoom:
                updateWaterColorOnZoom();
                return true;
            case R.id.action_add_custom_tiles:
                addCustomTileSource();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private void setLayerInvisible() {
        String[] roadLayers = new String[]{"water"};
        for (String roadLayer : roadLayers) {
            Layer layer = mapboxMap.getLayer(roadLayer);
            if (layer != null) {
                layer.setProperties(visibility(NONE));
            }
        }
    }

    private void setRoadSymbolPlacement() {
        //Zoom so that the labels are visible first
        mapboxMap.animateCamera(CameraUpdateFactory.zoomTo(14), new DefaultCallback() {
            @Override
            public void onFinish() {
                String[] roadLayers = new String[]{"road-label-small", "road-label-medium", "road-label-large"};
                for (String roadLayer : roadLayers) {
                    Layer layer = mapboxMap.getLayer(roadLayer);
                    if (layer != null) {
                        layer.setProperties(symbolPlacement(SYMBOL_PLACEMENT_POINT));
                    }
                }
            }
        });
    }

    private void setBackgroundOpacity() {
        Layer background = mapboxMap.getLayer("background");
        if (background != null) {
            background.setProperties(backgroundOpacity(0.2f));
        }
    }

    private void setWaterColor() {
        Layer water = mapboxMap.getLayer("water");
        if (water != null) {
            water.setProperties(
                    visibility(VISIBLE),
                    fillColor(Color.RED)
            );
        } else {
            Toast.makeText(RuntimeStyleActivity.this, "No water layer in this style", Toast.LENGTH_SHORT).show();
        }
    }

    private void removeBuildings() {
        //Zoom to see buildings first
        try {
            mapboxMap.removeLayer("building");
        } catch (NoSuchLayerException e) {
            Toast.makeText(RuntimeStyleActivity.this, e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }

    private void addParksLayer() {
        //Add a source
        Source source;
        try {
            source = new GeoJsonSource("amsterdam-spots", readRawResource(R.raw.amsterdam));
        } catch (IOException e) {
            Toast.makeText(RuntimeStyleActivity.this, "Couldn't add source: " + e.getMessage(), Toast.LENGTH_SHORT).show();
            return;
        }

        mapboxMap.addSource(source);

        FillLayer layer = new FillLayer("parksLayer", "amsterdam-spots");
        layer.setProperties(
                fillColor(Color.RED),
                fillOutlineColor(Color.BLUE),
                fillOpacity(0.3f),
                fillAntialias(true)
        );

        //Only show me parks
        layer.setFilter(eq("type", "park"));

        mapboxMap.addLayer(layer, "building");
        //layer.setPaintProperty(fillColor(Color.RED)); //XXX But not after the object is attached

        //Or get the object later and set it. It's all good.
        mapboxMap.getLayer("parksLayer").setProperties(fillColor(Color.RED));

        //You can get a typed layer, if you're sure it's of that type. Use with care
        layer = mapboxMap.getLayerAs("parksLayer");
        //And get some properties
        PropertyValue<Boolean> fillAntialias = layer.getFillAntialias();
        Log.d(TAG, "Fill anti alias: " + fillAntialias.getValue());
        layer.setProperties(fillTranslateAnchor(FILL_TRANSLATE_ANCHOR_MAP));
        PropertyValue<String> fillTranslateAnchor = layer.getFillTranslateAnchor();
        Log.d(TAG, "Fill translate anchor: " + fillTranslateAnchor.getValue());
        PropertyValue<String> visibility = layer.getVisibility();
        Log.d(TAG, "Visibility: " + visibility.getValue());

        //Get a good look at it all
        mapboxMap.animateCamera(CameraUpdateFactory.zoomTo(12));
    }

    private void addTerrainLayer() {
        //Add a source
        Source source = new VectorSource("my-terrain-source", "mapbox://mapbox.mapbox-terrain-v2");
        mapboxMap.addSource(source);

        LineLayer layer = new LineLayer("terrainLayer", "my-terrain-source");
        layer.setSourceLayer("contour");
        layer.setProperties(
                lineJoin(Property.LINE_JOIN_ROUND),
                lineCap(Property.LINE_CAP_ROUND),
                lineColor(Color.RED),
                lineWidth(20f)
        );

        mapboxMap.addLayer(layer);

        //Need to get a fresh handle
        layer = mapboxMap.getLayerAs("terrainLayer");

        //Make sure it's also applied after the fact
        layer.setMinZoom(10);
        layer.setMaxZoom(15);

        layer = (LineLayer) mapboxMap.getLayer("terrainLayer");
        Toast.makeText(this, String.format("Set min/max zoom to %s - %s", layer.getMinZoom(), layer.getMaxZoom()), Toast.LENGTH_SHORT).show();
    }

    private void addSatelliteLayer() {
        //Add a source
        Source source = new RasterSource("my-raster-source", "mapbox://mapbox.satellite").withTileSize(512);
        mapboxMap.addSource(source);

        //Add a layer
        mapboxMap.addLayer(new RasterLayer("satellite-layer", "my-raster-source"));
    }

    private void updateWaterColorOnZoom() {
        FillLayer layer = mapboxMap.getLayerAs("water");
        if (layer == null) {
            return;
        }

        //Set a zoom function to update the color of the water
        layer.setProperties(fillColor(zoom(0.8f,
                stop(1, fillColor(Color.GREEN)),
                stop(4, fillColor(Color.BLUE)),
                stop(12, fillColor(Color.RED)),
                stop(20, fillColor(Color.BLACK))
        )));

        //do some animations to show it off properly
        mapboxMap.animateCamera(CameraUpdateFactory.zoomTo(1), 1500);

        PropertyValue<String> fillColor = layer.getFillColor();
        Function<String> function = fillColor.getFunction();
        Log.d(TAG, "Fill color base: " + function.getBase());
        Log.d(TAG, "Fill color #stops: " + function.getStops().length);
    }

    private String readRawResource(@RawRes int rawResource) throws IOException {
        InputStream is = getResources().openRawResource(rawResource);
        Writer writer = new StringWriter();
        char[] buffer = new char[1024];
        try {
            Reader reader = new BufferedReader(new InputStreamReader(is, "UTF-8"));
            int n;
            while ((n = reader.read(buffer)) != -1) {
                writer.write(buffer, 0, n);
            }
        } finally {
            is.close();
        }

        return writer.toString();
    }

    private void setupActionBar() {
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }
    }

    private void addCustomTileSource() {
        //Add a source
        Source source = new VectorSource("custom-tile-source", new TileSet("2.1.0", "https://vector.mapzen.com/osm/all/{z}/{x}/{y}.mvt?api_key=vector-tiles-LM25tq4"));
        mapboxMap.addSource(source);

        //Add a layer
        mapboxMap.addLayer(
                new FillLayer("custom-tile-layers", "custom-tile-source")
                        .withSourceLayer("water")
        );
    }

    private static class DefaultCallback implements MapboxMap.CancelableCallback {

        @Override
        public void onCancel() {
            //noop
        }

        @Override
        public void onFinish() {
            //noop
        }
    }
}
