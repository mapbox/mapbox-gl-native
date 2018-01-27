package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.PointF;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;

import com.google.gson.JsonObject;
import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;


import java.util.List;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.functions.Function.property;
import static com.mapbox.mapboxsdk.style.functions.Function.zoom;
import static com.mapbox.mapboxsdk.style.functions.stops.Stop.stop;
import static com.mapbox.mapboxsdk.style.functions.stops.Stops.categorical;
import static com.mapbox.mapboxsdk.style.functions.stops.Stops.interval;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconSize;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.visibility;

/**
 * Test activity showcasing changing the icon with a zoom function and adding selection state to a SymbolLayer.
 */
public class ZoomFunctionSymbolLayerActivity extends AppCompatActivity {

  private static final String LAYER_ID = "symbolLayer";
  private static final String SOURCE_ID = "poiSource";
  private static final String BUS_MAKI_ICON_ID = "bus-11";
  private static final String CAFE_MAKI_ICON_ID = "cafe-11";
  private static final String KEY_PROPERTY_SELECTED = "selected";
  private static final float ZOOM_STOP_MIN_VALUE = 7.0f;
  private static final float ZOOM_STOP_MAX_VALUE = 12.0f;

  private MapView mapView;
  private MapboxMap mapboxMap;
  private GeoJsonSource source;
  private SymbolLayer layer;

  private boolean isInitialPosition = true;
  private boolean isSelected = false;
  private boolean isShowingSymbolLayer = true;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_zoom_symbol_layer);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;
      updateSource();
      addLayer();
      addMapClickListener();
    });
  }

  private void updateSource() {
    FeatureCollection featureCollection = createFeatureCollection();
    if (source != null) {
      source.setGeoJson(featureCollection);
    } else {
      source = new GeoJsonSource(SOURCE_ID, featureCollection);
      mapboxMap.addSource(source);
    }
  }

  private void toggleSymbolLayerVisibility() {
    layer.setProperties(
      visibility(isShowingSymbolLayer ? Property.NONE : Property.VISIBLE)
    );
    isShowingSymbolLayer = !isShowingSymbolLayer;
  }

  private FeatureCollection createFeatureCollection() {
    Point point = isInitialPosition
      ? Point.fromLngLat(-74.01618140, 40.701745)
      : Point.fromLngLat(-73.988097, 40.749864);

    JsonObject properties = new JsonObject();
    properties.addProperty(KEY_PROPERTY_SELECTED, isSelected);
    Feature feature = Feature.fromGeometry(point, properties);
    return FeatureCollection.fromFeatures(new Feature[] {feature});
  }

  private void addLayer() {
    layer = new SymbolLayer(LAYER_ID, SOURCE_ID);
    layer.setProperties(
      iconImage(
        zoom(
          interval(
            stop(ZOOM_STOP_MIN_VALUE, iconImage(BUS_MAKI_ICON_ID)),
            stop(ZOOM_STOP_MAX_VALUE, iconImage(CAFE_MAKI_ICON_ID))
          )
        )
      ),
      iconSize(
        property(
          KEY_PROPERTY_SELECTED,
          categorical(
            stop(true, iconSize(3.0f)),
            stop(false, iconSize(1.0f))
          )
        )
      ),
      iconAllowOverlap(true)
    );
    mapboxMap.addLayer(layer);
  }

  private void addMapClickListener() {
    mapboxMap.setOnMapClickListener(point -> {
      PointF screenPoint = mapboxMap.getProjection().toScreenLocation(point);
      List<Feature> featureList = mapboxMap.queryRenderedFeatures(screenPoint, LAYER_ID);
      if (!featureList.isEmpty()) {
        Feature feature = featureList.get(0);
        boolean selectedNow = feature.getBooleanProperty(KEY_PROPERTY_SELECTED);
        isSelected = !selectedNow;
        updateSource();
      } else {
        Timber.e("No features found");
      }
    });
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_symbols, menu);
    return true;
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    if (mapboxMap != null) {
      if (item.getItemId() == R.id.menu_action_change_location) {
        isInitialPosition = !isInitialPosition;
        updateSource();
      } else if (item.getItemId() == R.id.menu_action_toggle_source) {
        toggleSymbolLayerVisibility();
      }
    }
    return super.onOptionsItemSelected(item);
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
}