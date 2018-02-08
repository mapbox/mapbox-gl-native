package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.LineLayer;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.PropertyValue;
import com.mapbox.mapboxsdk.style.layers.RasterLayer;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.layers.TransitionOptions;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.style.sources.RasterSource;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.mapboxsdk.style.sources.TileSet;
import com.mapbox.mapboxsdk.style.sources.VectorSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.ResourceUtils;


import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.expressions.Expression.color;
import static com.mapbox.mapboxsdk.style.expressions.Expression.exponential;
import static com.mapbox.mapboxsdk.style.expressions.Expression.interpolate;
import static com.mapbox.mapboxsdk.style.expressions.Expression.stop;
import static com.mapbox.mapboxsdk.style.expressions.Expression.zoom;
import static com.mapbox.mapboxsdk.style.layers.Filter.all;
import static com.mapbox.mapboxsdk.style.layers.Filter.eq;
import static com.mapbox.mapboxsdk.style.layers.Filter.gte;
import static com.mapbox.mapboxsdk.style.layers.Filter.lt;
import static com.mapbox.mapboxsdk.style.layers.Property.FILL_TRANSLATE_ANCHOR_MAP;
import static com.mapbox.mapboxsdk.style.layers.Property.NONE;
import static com.mapbox.mapboxsdk.style.layers.Property.SYMBOL_PLACEMENT_POINT;
import static com.mapbox.mapboxsdk.style.layers.Property.VISIBLE;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.backgroundOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillAntialias;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillOutlineColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillTranslateAnchor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineCap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineJoin;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineWidth;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.symbolPlacement;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.visibility;

/**
 * Test activity showcasing the runtime style API.
 */
public class RuntimeStyleActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_runtime_style);

    // Initialize map as normal
    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);


    mapView.getMapAsync(map -> {
      // Store for later
      mapboxMap = map;

      // Center and Zoom (Amsterdam, zoomed to streets)
      mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(52.379189, 4.899431), 14));

      mapboxMap.setTransitionDuration(250);
      mapboxMap.setTransitionDelay(50);
    });
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_runtime_style, menu);
    return true;
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
      case R.id.action_list_layers:
        listLayers();
        return true;
      case R.id.action_list_sources:
        listSources();
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
      case R.id.action_add_dynamic_parks_layer:
        addDynamicParksLayer();
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
      case R.id.action_fill_filter:
        styleFillFilterLayer();
        return true;
      case R.id.action_line_filter:
        styleLineFilterLayer();
        return true;
      case R.id.action_numeric_filter:
        styleNumericFillLayer();
        return true;
      default:
        return super.onOptionsItemSelected(item);
    }
  }

  private void listLayers() {
    List<Layer> layers = mapboxMap.getLayers();
    StringBuilder builder = new StringBuilder("Layers:");
    for (Layer layer : layers) {
      builder.append("\n");
      builder.append(layer.getId());
    }
    Toast.makeText(this, builder.toString(), Toast.LENGTH_LONG).show();
  }

  private void listSources() {
    List<Source> sources = mapboxMap.getSources();
    StringBuilder builder = new StringBuilder("Sources:");
    for (Source source : sources) {
      builder.append("\n");
      builder.append(source.getId());
    }
    Toast.makeText(this, builder.toString(), Toast.LENGTH_LONG).show();
  }

  private void setLayerInvisible() {
    String[] roadLayers = new String[] {"water"};
    for (String roadLayer : roadLayers) {
      Layer layer = mapboxMap.getLayer(roadLayer);
      if (layer != null) {
        layer.setProperties(visibility(NONE));
      }
    }
  }

  private void setRoadSymbolPlacement() {
    // Zoom so that the labels are visible first
    mapboxMap.animateCamera(CameraUpdateFactory.zoomTo(14), new DefaultCallback() {
      @Override
      public void onFinish() {
        String[] roadLayers = new String[] {"road-label-small", "road-label-medium", "road-label-large"};
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
    FillLayer water = mapboxMap.getLayerAs("water");
    if (water != null) {
      water.setFillColorTransition(new TransitionOptions(7500, 1000));
      water.setProperties(
        visibility(VISIBLE),
        fillColor(Color.RED)
      );
    } else {
      Toast.makeText(RuntimeStyleActivity.this, "No water layer in this style", Toast.LENGTH_SHORT).show();
    }
  }

  private void removeBuildings() {
    // Zoom to see buildings first
    mapboxMap.removeLayer("building");
  }

  private void addParksLayer() {
    // Add a source
    Source source;
    try {
      source = new GeoJsonSource("amsterdam-spots", ResourceUtils.readRawResource(this, R.raw.amsterdam));
    } catch (IOException ioException) {
      Toast.makeText(
        RuntimeStyleActivity.this,
        "Couldn't add source: " + ioException.getMessage(),
        Toast.LENGTH_SHORT).show();
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

    // Only show me parks (except westerpark with stroke-width == 3)
    layer.setFilter(all(eq("type", "park"), eq("stroke-width", 2)));

    mapboxMap.addLayerBelow(layer, "building");
    // layer.setPaintProperty(fillColor(Color.RED)); // XXX But not after the object is attached

    // Or get the object later and set it. It's all good.
    mapboxMap.getLayer("parksLayer").setProperties(fillColor(Color.RED));

    // You can get a typed layer, if you're sure it's of that type. Use with care
    layer = mapboxMap.getLayerAs("parksLayer");
    // And get some properties
    PropertyValue<Boolean> fillAntialias = layer.getFillAntialias();
    Timber.d("Fill anti alias: %s", fillAntialias.getValue());
    layer.setProperties(fillTranslateAnchor(FILL_TRANSLATE_ANCHOR_MAP));
    PropertyValue<String> fillTranslateAnchor = layer.getFillTranslateAnchor();
    Timber.d("Fill translate anchor: %s", fillTranslateAnchor.getValue());
    PropertyValue<String> visibility = layer.getVisibility();
    Timber.d("Visibility: %s", visibility.getValue());

    // Get a good look at it all
    mapboxMap.animateCamera(CameraUpdateFactory.zoomTo(12));
  }

  private void addDynamicParksLayer() {
    // Load some data
    FeatureCollection parks;
    try {
      String json = ResourceUtils.readRawResource(this, R.raw.amsterdam);
      parks = FeatureCollection.fromJson(json);
    } catch (IOException ioException) {
      Toast.makeText(
        RuntimeStyleActivity.this,
        "Couldn't add source: " + ioException.getMessage(),
        Toast.LENGTH_SHORT
      ).show();
      return;
    }

    // Add an empty source
    mapboxMap.addSource(new GeoJsonSource("dynamic-park-source"));

    FillLayer layer = new FillLayer("dynamic-parks-layer", "dynamic-park-source");
    layer.setProperties(
      fillColor(Color.GREEN),
      fillOutlineColor(Color.GREEN),
      fillOpacity(0.8f),
      fillAntialias(true)
    );

    // Only show me parks
    layer.setFilter(all(eq("type", "park")));

    mapboxMap.addLayer(layer);

    // Get a good look at it all
    mapboxMap.animateCamera(CameraUpdateFactory.zoomTo(12));

    // Animate the parks source
    animateParksSource(parks, 0);
  }

  private void animateParksSource(final FeatureCollection parks, final int counter) {
    Handler handler = new Handler(getMainLooper());
    handler.postDelayed(() -> {
      if (mapboxMap == null) {
        return;
      }

      Timber.d("Updating parks source");
      // change the source
      int park = counter < parks.features().size() - 1 ? counter : 0;

      GeoJsonSource source = mapboxMap.getSourceAs("dynamic-park-source");

      if (source == null) {
        Timber.e("Source not found");
        Toast.makeText(RuntimeStyleActivity.this, "Source not found", Toast.LENGTH_SHORT).show();
        return;
      }

      List<Feature> features = new ArrayList<>();
      features.add(parks.features().get(park));
      source.setGeoJson(FeatureCollection.fromFeatures(features));

      // Re-post
      animateParksSource(parks, park + 1);
    }, counter == 0 ? 100 : 1000);
  }

  private void addTerrainLayer() {
    // Add a source
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

    // adding layers below "road" layers
    List<Layer> layers = mapboxMap.getLayers();
    Layer latestLayer = null;
    Collections.reverse(layers);
    for (Layer currentLayer : layers) {
      if (currentLayer instanceof FillLayer && ((FillLayer) currentLayer).getSourceLayer().equals("road")) {
        latestLayer = currentLayer;
      } else if (currentLayer instanceof CircleLayer && ((CircleLayer) currentLayer).getSourceLayer().equals("road")) {
        latestLayer = currentLayer;
      } else if (currentLayer instanceof SymbolLayer && ((SymbolLayer) currentLayer).getSourceLayer().equals("road")) {
        latestLayer = currentLayer;
      } else if (currentLayer instanceof LineLayer && ((LineLayer) currentLayer).getSourceLayer().equals("road")) {
        latestLayer = currentLayer;
      }
    }

    if (latestLayer != null) {
      mapboxMap.addLayerBelow(layer, latestLayer.getId());
    }

    // Need to get a fresh handle
    layer = mapboxMap.getLayerAs("terrainLayer");

    // Make sure it's also applied after the fact
    layer.setMinZoom(10);
    layer.setMaxZoom(15);

    layer = (LineLayer) mapboxMap.getLayer("terrainLayer");
    Toast.makeText(this, String.format(
      "Set min/max zoom to %s - %s", layer.getMinZoom(), layer.getMaxZoom()), Toast.LENGTH_SHORT).show();
  }

  private void addSatelliteLayer() {
    // Add a source
    Source source = new RasterSource("my-raster-source", "mapbox://mapbox.satellite", 512);
    mapboxMap.addSource(source);

    // Add a layer
    mapboxMap.addLayer(new RasterLayer("satellite-layer", "my-raster-source"));
  }

  private void updateWaterColorOnZoom() {
    FillLayer layer = mapboxMap.getLayerAs("water");
    if (layer == null) {
      return;
    }

    // Set a zoom function to update the color of the water
    layer.setProperties(
      fillColor(
        interpolate(
          exponential(0.8f),
          zoom(),
          stop(1, color(Color.GREEN)),
          stop(4, color(Color.BLUE)),
          stop(12, color(Color.RED)),
          stop(20, color(Color.BLACK))
        )
      )
    );

    // do some animations to show it off properly
    mapboxMap.animateCamera(CameraUpdateFactory.zoomTo(1), 1500);
  }

  private void addCustomTileSource() {
    // Add a source
    TileSet tileSet = new TileSet("2.1.0", "https://d25uarhxywzl1j.cloudfront.net/v0.1/{z}/{x}/{y}.mvt");
    tileSet.setMinZoom(0);
    tileSet.setMaxZoom(14);
    Source source = new VectorSource("custom-tile-source", tileSet);
    mapboxMap.addSource(source);

    // Add a layer
    LineLayer lineLayer = new LineLayer("custom-tile-layers", "custom-tile-source");
    lineLayer.setSourceLayer("mapillary-sequences");
    lineLayer.setProperties(
      lineCap(Property.LINE_CAP_ROUND),
      lineJoin(Property.LINE_JOIN_ROUND),
      lineOpacity(0.6f),
      lineWidth(2.0f),
      lineColor(Color.GREEN)
    );
    mapboxMap.addLayer(lineLayer);
  }

  private void styleFillFilterLayer() {
    mapboxMap.setStyleUrl("asset://fill_filter_style.json");
    mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(31, -100), 3));

    Handler handler = new Handler(getMainLooper());
    handler.postDelayed(() -> {
      if (mapboxMap == null) {
        return;
      }

      Timber.d("Styling filtered fill layer");

      FillLayer states = (FillLayer) mapboxMap.getLayer("states");

      if (states != null) {
        states.setFilter(eq("name", "Texas"));
        states.setFillOpacityTransition(new TransitionOptions(2500, 0));
        states.setFillColorTransition(new TransitionOptions(2500, 0));
        states.setProperties(
          fillColor(Color.RED),
          fillOpacity(0.25f)
        );
      } else {
        Toast.makeText(RuntimeStyleActivity.this, "No states layer in this style", Toast.LENGTH_SHORT).show();
      }
    }, 2000);
  }

  private void styleLineFilterLayer() {
    mapboxMap.setStyleUrl("asset://line_filter_style.json");
    mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(40, -97), 5));

    Handler handler = new Handler(getMainLooper());
    handler.postDelayed(() -> {
      if (mapboxMap == null) {
        return;
      }

      Timber.d("Styling filtered line layer");

      LineLayer counties = (LineLayer) mapboxMap.getLayer("counties");

      if (counties != null) {
        counties.setFilter(eq("NAME10", "Washington"));

        counties.setProperties(
          lineColor(Color.RED),
          lineOpacity(0.75f),
          lineWidth(5f)
        );
      } else {
        Toast.makeText(RuntimeStyleActivity.this, "No counties layer in this style", Toast.LENGTH_SHORT).show();
      }
    }, 2000);
  }

  private void styleNumericFillLayer() {
    mapboxMap.setStyleUrl("asset://numeric_filter_style.json");
    mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(40, -97), 5));

    Handler handler = new Handler(getMainLooper());
    handler.postDelayed(() -> {
      if (mapboxMap == null) {
        return;
      }

      Timber.d("Styling numeric fill layer");

      FillLayer regions = (FillLayer) mapboxMap.getLayer("regions");

      if (regions != null) {
        regions.setFilter(all(gte("HRRNUM", 200), lt("HRRNUM", 300)));

        regions.setProperties(
          fillColor(Color.BLUE),
          fillOpacity(0.5f)
        );
      } else {
        Toast.makeText(RuntimeStyleActivity.this, "No regions layer in this style", Toast.LENGTH_SHORT).show();
      }
    }, 2000);
  }

  private static class DefaultCallback implements MapboxMap.CancelableCallback {

    @Override
    public void onCancel() {
      // noop
    }

    @Override
    public void onFinish() {
      // noop
    }
  }
}
