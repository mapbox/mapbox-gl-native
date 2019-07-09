package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.PointF;
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
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.utils.BitmapUtils;
import timber.log.Timber;

import java.util.Arrays;
import java.util.List;
import java.util.Objects;
import java.util.Random;

import static com.mapbox.mapboxsdk.style.expressions.Expression.FormatOption.formatFontScale;
import static com.mapbox.mapboxsdk.style.expressions.Expression.FormatOption.formatTextColor;
import static com.mapbox.mapboxsdk.style.expressions.Expression.FormatOption.formatTextFont;
import static com.mapbox.mapboxsdk.style.expressions.Expression.concat;
import static com.mapbox.mapboxsdk.style.expressions.Expression.format;
import static com.mapbox.mapboxsdk.style.expressions.Expression.formatEntry;
import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.expressions.Expression.literal;
import static com.mapbox.mapboxsdk.style.expressions.Expression.match;
import static com.mapbox.mapboxsdk.style.expressions.Expression.rgb;
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
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textRotationAlignment;
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
  private static final String ID_FEATURE_PROPERTY = "id";
  private static final String SELECTED_FEATURE_PROPERTY = "selected";
  private static final String TITLE_FEATURE_PROPERTY = "title";

  private static final String[] NORMAL_FONT_STACK = new String[] {"DIN Offc Pro Regular", "Arial Unicode MS Regular"};
  private static final String[] BOLD_FONT_STACK = new String[] {"DIN Offc Pro Bold", "Arial Unicode MS Regular"};

  private static final String MAPBOX_SIGN_SOURCE = "mapbox-sign-source";
  private static final String MAPBOX_SIGN_LAYER = "mapbox-sign-layer";

  private static final Expression TEXT_FIELD_EXPRESSION =
    switchCase(toBool(get(SELECTED_FEATURE_PROPERTY)),
      format(
        formatEntry(
          get(TITLE_FEATURE_PROPERTY),
          formatTextFont(BOLD_FONT_STACK)
        ),
        formatEntry("\nis fun!", formatFontScale(0.75))
      ),
      format(
        formatEntry("This is", formatFontScale(0.75)),
        formatEntry(
          concat(literal("\n"), get(TITLE_FEATURE_PROPERTY)),
          formatFontScale(1.25),
          formatTextFont(BOLD_FONT_STACK)
        )
      )
    );

  private final Random random = new Random();
  private GeoJsonSource markerSource;
  private FeatureCollection markerCollection;
  private SymbolLayer markerSymbolLayer;
  private SymbolLayer mapboxSignSymbolLayer;
  private MapboxMap mapboxMap;
  private MapView mapView;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_symbollayer);

    // Create map configuration
    MapboxMapOptions mapboxMapOptions = MapboxMapOptions.createFromAttributes(this);
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

    // Use OnStyleImageMissing API to lazily load an icon
    mapView.addOnStyleImageMissingListener(id -> {
      Style style = mapboxMap.getStyle();
      if (style != null) {
        Timber.e("Adding image with id: %s", id);
        Bitmap androidIcon = BitmapUtils.getBitmapFromDrawable(getResources().getDrawable(R.drawable.ic_android_2));
        style.addImage(id, Objects.requireNonNull(androidIcon));
      }
    });
  }

  @Override
  public void onMapReady(@NonNull MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
    Bitmap carBitmap = BitmapUtils.getBitmapFromDrawable(
      getResources().getDrawable(R.drawable.ic_directions_car_black));

    // marker source
    markerCollection = FeatureCollection.fromFeatures(new Feature[] {
      Feature.fromGeometry(Point.fromLngLat(4.91638, 52.35673), featureProperties("1", "Android")),
      Feature.fromGeometry(Point.fromLngLat(4.91638, 52.34673), featureProperties("2", "Car"))
    });
    markerSource = new GeoJsonSource(MARKER_SOURCE, markerCollection);

    // marker layer
    markerSymbolLayer = new SymbolLayer(MARKER_LAYER, MARKER_SOURCE)
      .withProperties(
        iconImage(get(TITLE_FEATURE_PROPERTY)),
        iconIgnorePlacement(true),
        iconAllowOverlap(true),
        iconSize(switchCase(toBool(get(SELECTED_FEATURE_PROPERTY)), literal(1.5f), literal(1.0f))),
        iconAnchor(Property.ICON_ANCHOR_BOTTOM),
        iconColor(Color.BLUE),
        textField(TEXT_FIELD_EXPRESSION),
        textFont(NORMAL_FONT_STACK),
        textColor(Color.BLUE),
        textAllowOverlap(true),
        textIgnorePlacement(true),
        textAnchor(Property.TEXT_ANCHOR_TOP),
        textSize(10f)
      );

    // mapbox sign layer
    Source mapboxSignSource = new GeoJsonSource(MAPBOX_SIGN_SOURCE, Point.fromLngLat(4.91638, 52.3510));
    mapboxSignSymbolLayer = new SymbolLayer(MAPBOX_SIGN_LAYER, MAPBOX_SIGN_SOURCE);
    shuffleMapboxSign();

    mapboxMap.setStyle(new Style.Builder()
      .fromUri("asset://streets.json")
      .withImage("Car", Objects.requireNonNull(carBitmap), false)
      .withSources(markerSource, mapboxSignSource)
      .withLayers(markerSymbolLayer, mapboxSignSymbolLayer)
    );

    // Set a click-listener so we can manipulate the map
    mapboxMap.addOnMapClickListener(SymbolLayerActivity.this);
  }

  @Override
  public boolean onMapClick(@NonNull LatLng point) {
    // Query which features are clicked
    PointF screenLoc = mapboxMap.getProjection().toScreenLocation(point);
    List<Feature> markerFeatures = mapboxMap.queryRenderedFeatures(screenLoc, MARKER_LAYER);
    if (!markerFeatures.isEmpty()) {
      for (Feature feature : Objects.requireNonNull(markerCollection.features())) {
        if (feature.getStringProperty(ID_FEATURE_PROPERTY)
          .equals(markerFeatures.get(0).getStringProperty(ID_FEATURE_PROPERTY))) {

          // use DDS
          boolean selected = feature.getBooleanProperty(SELECTED_FEATURE_PROPERTY);
          feature.addBooleanProperty(SELECTED_FEATURE_PROPERTY, !selected);

          // validate symbol flicker regression for #13407
          markerSymbolLayer.setProperties(iconOpacity(match(
            get(ID_FEATURE_PROPERTY), literal(1.0f),
            stop(feature.getStringProperty("id"), selected ? 0.3f :  1.0f)
          )));
        }
      }
      markerSource.setGeoJson(markerCollection);
    } else {
      List<Feature> mapboxSignFeatures = mapboxMap.queryRenderedFeatures(screenLoc, MAPBOX_SIGN_LAYER);
      if (!mapboxSignFeatures.isEmpty()) {
        shuffleMapboxSign();
      }
    }

    return false;
  }

  private void toggleTextSize() {
    if (markerSymbolLayer != null) {
      Number size = markerSymbolLayer.getTextSize().getValue();
      if (size != null) {
        markerSymbolLayer.setProperties((float) size > 10 ? textSize(10f) : textSize(20f));
      }
    }
  }

  private void toggleTextField() {
    if (markerSymbolLayer != null) {
      if (TEXT_FIELD_EXPRESSION.equals(markerSymbolLayer.getTextField().getExpression())) {
        markerSymbolLayer.setProperties(textField("āA"));
      } else {
        markerSymbolLayer.setProperties(textField(TEXT_FIELD_EXPRESSION));
      }
    }
  }

  private void toggleTextFont() {
    if (markerSymbolLayer != null) {
      if (Arrays.equals(markerSymbolLayer.getTextFont().getValue(), NORMAL_FONT_STACK)) {
        markerSymbolLayer.setProperties(textFont(BOLD_FONT_STACK));
      } else {
        markerSymbolLayer.setProperties(textFont(NORMAL_FONT_STACK));
      }
    }
  }

  private void shuffleMapboxSign() {
    if (mapboxSignSymbolLayer != null) {
      mapboxSignSymbolLayer.setProperties(
        textField(
          format(
            formatEntry("M", formatFontScale(2)),
            getRandomColorEntryForString("a"),
            getRandomColorEntryForString("p"),
            getRandomColorEntryForString("b"),
            getRandomColorEntryForString("o"),
            getRandomColorEntryForString("x")
          )
        ),
        textColor(Color.BLACK),
        textFont(BOLD_FONT_STACK),
        textSize(25f),
        textRotationAlignment(Property.TEXT_ROTATION_ALIGNMENT_MAP)
      );
    }
  }

  private Expression.FormatEntry getRandomColorEntryForString(@NonNull String string) {
    return formatEntry(string,
      formatTextColor(
        rgb(
          random.nextInt(256),
          random.nextInt(256),
          random.nextInt(256)
        )
      ));
  }

  private JsonObject featureProperties(@NonNull String id, @NonNull String title) {
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
