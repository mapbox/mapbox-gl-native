package com.mapbox.mapboxsdk.testapp.activity.style;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.PointF;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.layers.Filter;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.ResourceUtils;


import java.io.IOException;
import java.util.HashMap;
import java.util.List;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.expressions.Expression.concat;
import static com.mapbox.mapboxsdk.style.expressions.Expression.division;
import static com.mapbox.mapboxsdk.style.expressions.Expression.downcase;
import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.expressions.Expression.literal;
import static com.mapbox.mapboxsdk.style.expressions.Expression.pi;
import static com.mapbox.mapboxsdk.style.expressions.Expression.product;
import static com.mapbox.mapboxsdk.style.expressions.Expression.upcase;
import static com.mapbox.mapboxsdk.style.layers.Property.ICON_ANCHOR_BOTTOM;
import static com.mapbox.mapboxsdk.style.layers.Property.TEXT_ANCHOR_TOP;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAnchor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconOffset;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconSize;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textAnchor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textField;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textSize;

/**
 * Test activity showcasing using a symbol generator that generates Bitmaps from Android SDK Views.
 */
public class SymbolGeneratorActivity extends AppCompatActivity implements OnMapReadyCallback {

  private static final String SOURCE_ID = "com.mapbox.mapboxsdk.style.layers.symbol.source.id";
  private static final String LAYER_ID = "com.mapbox.mapboxsdk.style.layers.symbol.layer.id";
  private static final String FEATURE_ID = "brk_name";
  private static final String FEATURE_RANK = "scalerank";
  private static final String FEATURE_NAME = "name_sort";
  private static final String FEATURE_TYPE = "type";
  private static final String FEATURE_REGION = "continent";

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
  public void onMapReady(final MapboxMap map) {
    mapboxMap = map;
    addSymbolClickListener();
    new LoadDataTask(this).execute();
  }

  private void addSymbolClickListener() {
    mapboxMap.setOnMapClickListener(point -> {
      PointF screenPoint = mapboxMap.getProjection().toScreenLocation(point);
      List<Feature> features = mapboxMap.queryRenderedFeatures(screenPoint, LAYER_ID);
      if (!features.isEmpty()) {
        Feature feature = features.get(0);
        Timber.v("Feature was clicked with data: %s", feature.toJson());
        Toast.makeText(
          SymbolGeneratorActivity.this,
          "hello from: " + feature.getStringProperty(FEATURE_NAME),
          Toast.LENGTH_LONG).show();
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
      return true;
    } else if (item.getItemId() == R.id.menu_action_filter) {
      SymbolLayer layer = mapboxMap.getLayerAs(LAYER_ID);
      layer.setFilter(Filter.eq(FEATURE_RANK, 1));
      //layer.setFilter(eq(get(FEATURE_RANK), 1));
      return true;
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

  private static class LoadDataTask extends AsyncTask<Void, Void, FeatureCollection> {

    private SymbolGeneratorActivity activity;

    LoadDataTask(SymbolGeneratorActivity activity) {
      this.activity = activity;
    }

    @Override
    protected FeatureCollection doInBackground(Void... params) {
      try {
        // read local geojson from raw folder
        String tinyCountriesJson = ResourceUtils.readRawResource(activity, R.raw.tiny_countries);
        return FeatureCollection.fromJson(tinyCountriesJson);

      } catch (IOException exception) {
        return null;
      }
    }

    @Override
    protected void onPostExecute(FeatureCollection featureCollection) {
      super.onPostExecute(featureCollection);
      if (featureCollection == null || activity == null) {
        return;
      }

      activity.onDataLoaded(featureCollection);
    }
  }

  public void onDataLoaded(@NonNull FeatureCollection featureCollection) {
    // add a geojson to the map
    Source source = new GeoJsonSource(SOURCE_ID, featureCollection);
    mapboxMap.addSource(source);

    // create layer use
    mapboxMap.addLayer(new SymbolLayer(LAYER_ID, SOURCE_ID)
      .withProperties(

        // icon configuration
        iconImage(get(literal(FEATURE_ID))),
        iconAllowOverlap(false),
        iconSize(
          division(get(literal(FEATURE_RANK)), literal(2))
        ),
        iconAnchor(ICON_ANCHOR_BOTTOM),
        iconOffset(new Float[] {0.0f, -5.0f}),

        // text field configuration
        textField(
          concat(
            upcase(literal("a ")),
            get(literal(FEATURE_TYPE)),
            downcase(literal(" IN ")),
            get(literal(FEATURE_REGION))
          )
        ),
        textSize(
          product(get(literal(FEATURE_RANK)), pi())
        ),
        textAnchor(TEXT_ANCHOR_TOP)
      )
    );

    new GenerateSymbolTask(mapboxMap, this).execute(featureCollection);
  }

  private static class GenerateSymbolTask extends AsyncTask<FeatureCollection, Void, HashMap<String, Bitmap>> {

    private MapboxMap mapboxMap;
    private Context context;

    GenerateSymbolTask(MapboxMap mapboxMap, Context context) {
      this.mapboxMap = mapboxMap;
      this.context = context;
    }

    @SuppressWarnings("WrongThread")
    @Override
    protected HashMap<String, Bitmap> doInBackground(FeatureCollection... params) {
      FeatureCollection featureCollection = params[0];

      HashMap<String, Bitmap> imagesMap = new HashMap<>();
      for (Feature feature : featureCollection.features()) {
        String countryName = feature.getStringProperty(FEATURE_ID);
        TextView textView = new TextView(context);
        textView.setBackgroundColor(context.getResources().getColor(R.color.blueAccent));
        textView.setPadding(10, 5, 10, 5);
        textView.setTextColor(Color.WHITE);
        textView.setText(countryName);
        imagesMap.put(countryName, SymbolGenerator.generate(textView));
      }
      return imagesMap;
    }

    @Override
    protected void onPostExecute(HashMap<String, Bitmap> bitmapHashMap) {
      super.onPostExecute(bitmapHashMap);
      mapboxMap.addImages(bitmapHashMap);
    }
  }
}