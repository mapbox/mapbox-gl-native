package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.PointF;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import com.google.gson.JsonObject;
import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import timber.log.Timber;

import java.util.List;

import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.expressions.Expression.literal;
import static com.mapbox.mapboxsdk.style.expressions.Expression.step;
import static com.mapbox.mapboxsdk.style.expressions.Expression.stop;
import static com.mapbox.mapboxsdk.style.expressions.Expression.switchCase;
import static com.mapbox.mapboxsdk.style.expressions.Expression.zoom;
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
  private static final float ZOOM_STOP_MAX_VALUE = 12.0f;

  private MapView mapView;
  private MapboxMap mapboxMap;
  private GeoJsonSource source;
  private SymbolLayer layer;

  private boolean isInitialPosition = true;
  private boolean isSelected = false;
  private boolean isShowingSymbolLayer = true;

  private MapboxMap.OnMapClickListener mapClickListener = new MapboxMap.OnMapClickListener() {
    @Override
    public boolean onMapClick(@NonNull LatLng point) {
      PointF screenPoint = mapboxMap.getProjection().toScreenLocation(point);
      List<Feature> featureList = mapboxMap.queryRenderedFeatures(screenPoint, LAYER_ID);
      if (!featureList.isEmpty()) {
        Feature feature = featureList.get(0);
        boolean selectedNow = feature.getBooleanProperty(KEY_PROPERTY_SELECTED);
        isSelected = !selectedNow;
        updateSource(mapboxMap.getStyle());
      } else {
        Timber.e("No features found");
      }
      return true;
    }
  };

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_zoom_symbol_layer);

    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;

      map.setStyle(Style.MAPBOX_STREETS, style -> {
        updateSource(style);
        addLayer(style);
        map.addOnMapClickListener(mapClickListener);
      });
    });
  }

  private void updateSource(Style style) {
    FeatureCollection featureCollection = createFeatureCollection();
    if (source != null) {
      source.setGeoJson(featureCollection);
    } else {
      source = new GeoJsonSource(SOURCE_ID, featureCollection);
      style.addSource(source);
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

  private void addLayer(Style style) {
    layer = new SymbolLayer(LAYER_ID, SOURCE_ID);
    layer.setProperties(
      iconImage(
        step(zoom(), literal(BUS_MAKI_ICON_ID),
          stop(ZOOM_STOP_MAX_VALUE, CAFE_MAKI_ICON_ID)
        )
      ),
      iconSize(
        switchCase(
          get(KEY_PROPERTY_SELECTED), literal(3.0f),
          literal(1.0f)
        )
      ),
      iconAllowOverlap(true)
    );
    style.addLayer(layer);
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
        updateSource(mapboxMap.getStyle());
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