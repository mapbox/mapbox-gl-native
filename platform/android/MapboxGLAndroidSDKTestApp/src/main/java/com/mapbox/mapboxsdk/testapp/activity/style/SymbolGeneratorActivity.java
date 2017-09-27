package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.support.v7.app.AppCompatActivity;
import android.graphics.Color;
import android.graphics.PointF;
import android.os.Bundle;
import android.support.annotation.NonNull;

import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.google.gson.GsonBuilder;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.ResourceUtils;
import com.mapbox.services.commons.geojson.Feature;
import com.mapbox.services.commons.geojson.FeatureCollection;
import com.mapbox.services.commons.geojson.Geometry;
import com.mapbox.services.commons.geojson.custom.GeometryDeserializer;
import com.mapbox.services.commons.geojson.custom.PositionDeserializer;
import com.mapbox.services.commons.models.Position;

import java.io.IOException;

import java.util.List;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;

/**
 * Test activity showcasing using a symbol generator that generates Bitmaps from Android SDK Views.
 */
public class SymbolGeneratorActivity extends AppCompatActivity implements OnMapReadyCallback {

  private static final String SOURCE_ID = "com.mapbox.mapboxsdk.style.layers.symbol.source.id";
  private static final String LAYER_ID = "com.mapbox.mapboxsdk.style.layers.symbol.layer.id";
  private static final String FEATURE_ID = "brk_name";
  private static final String FEATURE_VALUE = "name_sort";

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_symbol_generator);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
  }

  @Override
  public void onMapReady(MapboxMap map) {
    mapboxMap = map;
    try {
      // read local geojson from raw folder
      String tinyCountriesJson = ResourceUtils.readRawResource(this, R.raw.tiny_countries);

      // convert geojson to a model
      FeatureCollection featureCollection = new GsonBuilder()
        .registerTypeAdapter(Geometry.class, new GeometryDeserializer())
        .registerTypeAdapter(Position.class, new PositionDeserializer())
        .create().fromJson(tinyCountriesJson, FeatureCollection.class);

      // add a geojson to the map
      Source source = new GeoJsonSource(SOURCE_ID, featureCollection);
      mapboxMap.addSource(source);

      // for each feature add a symbolLayer
      for (Feature feature : featureCollection.getFeatures()) {
        String countryName = feature.getStringProperty(FEATURE_ID);

        // create View
        TextView textView = new TextView(this);
        textView.setBackgroundColor(getResources().getColor(R.color.blueAccent));
        textView.setPadding(10, 5, 10, 5);
        textView.setTextColor(Color.WHITE);
        textView.setText(countryName);

        // create bitmap from view
        mapboxMap.addImage(countryName, SymbolGenerator.generate(textView));
      }

      // create layer use
      mapboxMap.addLayer(new SymbolLayer(LAYER_ID, SOURCE_ID)
        .withProperties(
          iconImage("{" + FEATURE_ID + "}"), // { } is a token notation
          iconAllowOverlap(false)
        )
      );

      addSymbolClickListener();
    } catch (IOException exception) {
      Timber.e(exception);
    }
  }

  private void addSymbolClickListener() {
    mapboxMap.setOnMapClickListener(new MapboxMap.OnMapClickListener() {
      @Override
      public void onMapClick(@NonNull LatLng point) {
        PointF screenPoint = mapboxMap.getProjection().toScreenLocation(point);
        List<Feature> features = mapboxMap.queryRenderedFeatures(screenPoint, LAYER_ID);
        if (!features.isEmpty()) {
          Feature feature = features.get(0);
          Timber.v("Feature was clicked with data: %s", feature.toJson());
          Toast.makeText(
            SymbolGeneratorActivity.this,
            "hello from: " + feature.getStringProperty(FEATURE_VALUE),
            Toast.LENGTH_LONG).show();
        }
      }
    });
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_generator_symbol, menu);
    return super.onCreateOptionsMenu(menu);
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    if (item.getItemId() == R.id.menu_action_icon_overlap) {
      SymbolLayer layer = mapboxMap.getLayerAs(LAYER_ID);
      layer.setProperties(iconAllowOverlap(!layer.getIconAllowOverlap().getValue()));
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

  /**
   * Utility class to generate Bitmaps for Symbol.
   * <p>
   * Bitmaps can be added to the map with {@link com.mapbox.mapboxsdk.maps.MapboxMap#addImage(String, Bitmap)}
   * </p>
   */
  private static class SymbolGenerator {

    /**
     * Generate a Bitmap from an Android SDK View.
     *
     * @param view the View to be drawn to a Bitmap
     * @return the generated bitmap
     */
    public static Bitmap generate(@NonNull View view) {
      int measureSpec = View.MeasureSpec.makeMeasureSpec(0, View.MeasureSpec.UNSPECIFIED);
      view.measure(measureSpec, measureSpec);

      int measuredWidth = view.getMeasuredWidth();
      int measuredHeight = view.getMeasuredHeight();

      view.layout(0, 0, measuredWidth, measuredHeight);
      Bitmap bitmap = Bitmap.createBitmap(measuredWidth, measuredHeight, Bitmap.Config.ARGB_8888);
      bitmap.eraseColor(Color.TRANSPARENT);
      Canvas canvas = new Canvas(bitmap);
      view.draw(canvas);
      return bitmap;
    }
  }
}
