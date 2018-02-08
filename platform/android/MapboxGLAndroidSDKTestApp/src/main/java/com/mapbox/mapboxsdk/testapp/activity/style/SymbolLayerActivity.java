package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.PointF;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;

import com.google.gson.JsonObject;
import com.google.gson.JsonPrimitive;
import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;


import java.util.Arrays;
import java.util.List;

import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconSize;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textField;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textFont;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textSize;

/**
 * Test activity showcasing runtime manipulation of symbol layers.
 */
public class SymbolLayerActivity extends AppCompatActivity implements MapboxMap.OnMapClickListener {

  public static final String MARKER_SOURCE = "marker-source";
  public static final String MARKER_LAYER = "marker-layer";
  private MapboxMap mapboxMap;
  private MapView mapView;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_symbollayer);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;

      // Add a image for the makers
      mapboxMap.addImage(
        "my-marker-image",
        BitmapFactory.decodeResource(SymbolLayerActivity.this.getResources(),
          R.drawable.mapbox_marker_icon_default)
      );

      // Add a source
      FeatureCollection markers = FeatureCollection.fromFeatures(new Feature[] {
        Feature.fromGeometry(Point.fromLngLat(4.91638, 52.35673), featureProperties("Marker 1")),
        Feature.fromGeometry(Point.fromLngLat(4.91638, 52.34673), featureProperties("Marker 2"))
      });
      mapboxMap.addSource(new GeoJsonSource(MARKER_SOURCE, markers));

      // Add the symbol-layer
      mapboxMap.addLayer(
        new SymbolLayer(MARKER_LAYER, MARKER_SOURCE)
          .withProperties(
            iconImage("my-marker-image"),
            iconAllowOverlap(true),
            textField("{title}"),
            textColor(Color.RED),
            textSize(10f)
          )
      );

      // Show
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(52.35273, 4.91638), 14));

      // Set a click-listener so we can manipulate the map
      mapboxMap.setOnMapClickListener(SymbolLayerActivity.this);
    });
  }

  @Override
  public void onMapClick(@NonNull LatLng point) {
    // Query which features are clicked
    PointF screenLoc = mapboxMap.getProjection().toScreenLocation(point);
    List<Feature> features = mapboxMap.queryRenderedFeatures(screenLoc, MARKER_LAYER);

    SymbolLayer layer = mapboxMap.getLayerAs(MARKER_LAYER);
    if (features.size() == 0) {
      // Reset
      layer.setProperties(iconSize(1f));
    } else {
      layer.setProperties(iconSize(3f));
    }
  }

  private void toggleTextSize() {
    SymbolLayer layer = mapboxMap.getLayerAs(MARKER_LAYER);
    layer.setProperties(layer.getTextSize().getValue() > 10 ? textSize(10f) : textSize(20f));
  }

  private void toggleTextField() {
    SymbolLayer layer = mapboxMap.getLayerAs(MARKER_LAYER);
    layer.setProperties("{title}".equals(layer.getTextField().getValue()) ? textField("āA") : textField("{title}"));
  }

  private void toggleTextFont() {
    SymbolLayer layer = mapboxMap.getLayerAs(MARKER_LAYER);

    String[] fonts = layer.getTextFont().getValue();
    if (fonts == null || fonts.length == 0 || Arrays.asList(fonts).contains("Arial Unicode MS Regular")) {
      layer.setProperties(textFont(new String[] {"DIN Offc Pro Bold", "Arial Unicode MS Bold"}));
    } else {
      layer.setProperties(textFont(new String[] {"DIN Offc Pro Medium", "Arial Unicode MS Regular"}));
    }
  }

  private JsonObject featureProperties(String title) {
    JsonObject object = new JsonObject();
    object.add("title", new JsonPrimitive(title));
    return object;
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
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_symbol_layer, menu);
    return true;
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {
      case R.id.action_toggle_text_size:
        toggleTextSize();
        return true;
      case R.id.action_toggle_text_field:
        toggleTextField();
        return true;
      case R.id.action_toggle_text_font:
        toggleTextFont();
        return true;
      default:
        return super.onOptionsItemSelected(item);
    }
  }
}
