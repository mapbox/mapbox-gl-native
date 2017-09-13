package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.PointF;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;

import com.google.gson.JsonObject;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.services.commons.geojson.Feature;
import com.mapbox.services.commons.geojson.FeatureCollection;
import com.mapbox.services.commons.geojson.Point;
import com.mapbox.services.commons.models.Position;

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

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_zoom_symbol_layer);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(@NonNull final MapboxMap map) {
        mapboxMap = map;
        updateSource(false);
        addLayer();
        addMapClickListener();
      }
    });
  }

  private void updateSource(boolean selected) {
    FeatureCollection featureCollection = createFeatureCollection(selected);
    if (source != null) {
      source.setGeoJson(featureCollection);
    } else {
      source = new GeoJsonSource(SOURCE_ID, featureCollection);
      mapboxMap.addSource(source);
    }
  }

  private FeatureCollection createFeatureCollection(boolean selected) {
    Point point = Point.fromCoordinates(Position.fromCoordinates(-74.016181, 40.701745));
    Feature feature = Feature.fromGeometry(point);
    JsonObject properties = new JsonObject();
    properties.addProperty(KEY_PROPERTY_SELECTED, selected);
    feature.setProperties(properties);
    return FeatureCollection.fromFeatures(new Feature[] {feature});
  }

  private void addLayer() {
    SymbolLayer layer = new SymbolLayer(LAYER_ID, SOURCE_ID);
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
    mapboxMap.setOnMapClickListener(new MapboxMap.OnMapClickListener() {
      @Override
      public void onMapClick(@NonNull LatLng point) {
        PointF screenPoint = mapboxMap.getProjection().toScreenLocation(point);
        List<Feature> featureList = mapboxMap.queryRenderedFeatures(screenPoint, LAYER_ID);
        if (!featureList.isEmpty()) {
          Feature feature = featureList.get(0);
          boolean isSelected = feature.getBooleanProperty(KEY_PROPERTY_SELECTED);
          updateSource(!isSelected);
        } else {
          Timber.e("No features found");
        }
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
}