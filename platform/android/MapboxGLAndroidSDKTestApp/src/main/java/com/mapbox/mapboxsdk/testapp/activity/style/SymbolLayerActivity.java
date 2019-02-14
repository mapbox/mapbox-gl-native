package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.PointF;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.ViewGroup;
import com.google.gson.JsonObject;
import com.google.gson.JsonPrimitive;
import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.utils.BitmapUtils;

import java.util.List;
import java.util.Objects;

import static com.mapbox.mapboxsdk.style.expressions.Expression.FormatOption.formatFontScale;
import static com.mapbox.mapboxsdk.style.expressions.Expression.FormatOption.formatTextFont;
import static com.mapbox.mapboxsdk.style.expressions.Expression.concat;
import static com.mapbox.mapboxsdk.style.expressions.Expression.format;
import static com.mapbox.mapboxsdk.style.expressions.Expression.formatEntry;
import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.expressions.Expression.id;
import static com.mapbox.mapboxsdk.style.expressions.Expression.literal;
import static com.mapbox.mapboxsdk.style.expressions.Expression.match;
import static com.mapbox.mapboxsdk.style.expressions.Expression.stop;
import static com.mapbox.mapboxsdk.style.expressions.Expression.switchCase;
import static com.mapbox.mapboxsdk.style.expressions.Expression.toBool;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAnchor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconIgnorePlacement;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconSize;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textAnchor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textField;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textFont;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textIgnorePlacement;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textSize;

/**
 * Test activity showcasing runtime manipulation of symbol layers.
 * <p>
 * Showcases the ability to offline render a symbol layer by using a packaged style and fonts from the assets folder.
 * </p>
 */
public class SymbolLayerActivity extends AppCompatActivity implements MapboxMap.OnMapClickListener, OnMapReadyCallback {

  private static final String MARKER_SOURCE = "marker-source";
  private static final String MARKER_LAYER = "marker-layer";
  private static final String MARKER_ICON = "my-layers-image";
  public static final String ID_FEATURE_PROPERTY = "id";
  public static final String SELECTED_FEATURE_PROPERTY = "selected";
  public static final String TITLE_FEATURE_PROPERTY = "title";

  private GeoJsonSource geoJsonSource;
  private FeatureCollection markerCollection;
  private SymbolLayer symbolLayer;
  private MapboxMap mapboxMap;
  private MapView mapView;
  private boolean initialFont;
  private boolean styleLoaded;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_symbollayer);

    // Create map configuration
    MapboxMapOptions mapboxMapOptions = new MapboxMapOptions();
    mapboxMapOptions.camera(new CameraPosition.Builder().target(
      new LatLng(52.35273, 4.91638))
      .zoom(13)
      .build()
    );

    // Create map programmatically, add to view hierarchy
    mapView = new MapView(this, mapboxMapOptions);
    mapView.getMapAsync(this);
    mapView.onCreate(savedInstanceState);
    ((ViewGroup) findViewById(R.id.container)).addView(mapView);
  }

  @Override
  public void onMapReady(@NonNull MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
    // image
    Drawable icLayersDrawable = getResources().getDrawable(R.drawable.ic_layers);
    Bitmap icLayersBitmap = BitmapUtils.getBitmapFromDrawable(icLayersDrawable);

    // source
    markerCollection = FeatureCollection.fromFeatures(new Feature[] {
      Feature.fromGeometry(Point.fromLngLat(4.91638, 52.35673), featureProperties("1", "Marker 1")),
      Feature.fromGeometry(Point.fromLngLat(4.91638, 52.34673), featureProperties("2", "Marker 2"))
    });
    geoJsonSource = new GeoJsonSource(MARKER_SOURCE, markerCollection);

    // layer
    symbolLayer = new SymbolLayer(MARKER_LAYER, MARKER_SOURCE)
      .withProperties(
        iconImage(MARKER_ICON),
        iconIgnorePlacement(true),
        iconAllowOverlap(true),
        iconSize(switchCase(toBool(get(SELECTED_FEATURE_PROPERTY)), literal(3.0f), literal(1.0f))),
        iconAnchor(Property.ICON_ANCHOR_BOTTOM),
        iconColor(Color.RED),
        textField(
          format(
            formatEntry("this is", formatFontScale(0.75)),
            formatEntry(
              concat(literal("\n"), get(TITLE_FEATURE_PROPERTY)),
              formatFontScale(1.25),
              formatTextFont(new String[] {"DIN Offc Pro Bold", "Arial Unicode MS Regular"})
            )
          )
        ),
        textFont(new String[] {"DIN Offc Pro Regular", "Arial Unicode MS Regular"}),
        textColor(Color.RED),
        textAllowOverlap(true),
        textIgnorePlacement(true),
        textAnchor(Property.TEXT_ANCHOR_TOP),
        textSize(10f)
      );

    mapboxMap.setStyle(new Style.Builder()
        .fromUrl("asset://streets.json")
        .withImage(MARKER_ICON, Objects.requireNonNull(icLayersBitmap), true)
        .withSource(geoJsonSource)
        .withLayer(symbolLayer),
      style -> styleLoaded = true
    );

    // Set a click-listener so we can manipulate the map
    mapboxMap.addOnMapClickListener(SymbolLayerActivity.this);
  }

  @Override
  public boolean onMapClick(@NonNull LatLng point) {
    // Query which features are clicked
    PointF screenLoc = mapboxMap.getProjection().toScreenLocation(point);
    List<Feature> features = mapboxMap.queryRenderedFeatures(screenLoc, MARKER_LAYER);
    if (!features.isEmpty()) {
      for (Feature feature : markerCollection.features()) {
        if (feature.getStringProperty(TITLE_FEATURE_PROPERTY)
          .equals(features.get(0).getStringProperty(TITLE_FEATURE_PROPERTY))) {

          // use DDS
          boolean selected = feature.getBooleanProperty(SELECTED_FEATURE_PROPERTY);
          feature.addBooleanProperty(SELECTED_FEATURE_PROPERTY, !selected);

          // validate symbol flicker regression for #13407
          symbolLayer.setProperties(iconOpacity(match(
            get(ID_FEATURE_PROPERTY), literal(1.0f),
            stop(feature.getStringProperty("id"), 0.3f)
          )));
        }
      }
      geoJsonSource.setGeoJson(markerCollection);


    }

    return false;
  }

  private void toggleTextSize() {
    SymbolLayer layer = mapboxMap.getStyle().getLayerAs(MARKER_LAYER);
    layer.setProperties(layer.getTextSize().getValue() > 10 ? textSize(10f) : textSize(20f));
  }

  private void toggleTextField() {
    SymbolLayer layer = mapboxMap.getStyle().getLayerAs(MARKER_LAYER);
    layer.setProperties("{title}".equals(layer.getTextField().getValue()) ? textField("āA") : textField("{title}"));
  }

  private void toggleTextFont() {
    SymbolLayer layer = mapboxMap.getStyle().getLayerAs(MARKER_LAYER);
    if (initialFont) {
      layer.setProperties(textFont(new String[] {"DIN Offc Pro Bold", "Arial Unicode MS Bold"}));
    } else {
      layer.setProperties(textFont(new String[] {"DIN Offc Pro Medium", "Arial Unicode MS Regular"}));
    }
    initialFont = !initialFont;
  }

  private JsonObject featureProperties(String id, String title) {
    JsonObject object = new JsonObject();
    object.add(ID_FEATURE_PROPERTY, new JsonPrimitive(id));
    object.add(TITLE_FEATURE_PROPERTY, new JsonPrimitive(title));
    object.add(SELECTED_FEATURE_PROPERTY, new JsonPrimitive(false));
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
    if (mapboxMap != null) {
      mapboxMap.removeOnMapClickListener(this);
    }
    mapView.onDestroy();
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_symbol_layer, menu);
    return true;
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    if (!styleLoaded) {
      return super.onOptionsItemSelected(item);
    }

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
