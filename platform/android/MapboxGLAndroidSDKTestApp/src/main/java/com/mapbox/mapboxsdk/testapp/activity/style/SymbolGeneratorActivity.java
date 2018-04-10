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
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.ResourceUtils;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.HashMap;
import java.util.List;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.expressions.Expression.concat;
import static com.mapbox.mapboxsdk.style.expressions.Expression.division;
import static com.mapbox.mapboxsdk.style.expressions.Expression.downcase;
import static com.mapbox.mapboxsdk.style.expressions.Expression.eq;
import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.expressions.Expression.literal;
import static com.mapbox.mapboxsdk.style.expressions.Expression.match;
import static com.mapbox.mapboxsdk.style.expressions.Expression.number;
import static com.mapbox.mapboxsdk.style.expressions.Expression.pi;
import static com.mapbox.mapboxsdk.style.expressions.Expression.product;
import static com.mapbox.mapboxsdk.style.expressions.Expression.rgba;
import static com.mapbox.mapboxsdk.style.expressions.Expression.step;
import static com.mapbox.mapboxsdk.style.expressions.Expression.stop;
import static com.mapbox.mapboxsdk.style.expressions.Expression.string;
import static com.mapbox.mapboxsdk.style.expressions.Expression.upcase;
import static com.mapbox.mapboxsdk.style.expressions.Expression.zoom;
import static com.mapbox.mapboxsdk.style.layers.Property.ICON_ANCHOR_BOTTOM;
import static com.mapbox.mapboxsdk.style.layers.Property.TEXT_ANCHOR_TOP;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAnchor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconOffset;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconSize;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textAnchor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textColor;
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
      layer.setFilter(eq(get(FEATURE_RANK), literal(1)));
      Timber.e("Filter that was set: %s", layer.getFilter());
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

    private WeakReference<SymbolGeneratorActivity> activity;

    LoadDataTask(SymbolGeneratorActivity activity) {
      this.activity = new WeakReference<>(activity);
    }

    @Override
    protected FeatureCollection doInBackground(Void... params) {
      Context context = activity.get();
      if (context != null) {
        try {
          // read local geojson from raw folder
          String tinyCountriesJson = ResourceUtils.readRawResource(context, R.raw.tiny_countries);
          return FeatureCollection.fromJson(tinyCountriesJson);

        } catch (IOException exception) {
          Timber.e(exception);
        }
      }
      return null;
    }

    @Override
    protected void onPostExecute(FeatureCollection featureCollection) {
      super.onPostExecute(featureCollection);
      SymbolGeneratorActivity activity = this.activity.get();
      if (featureCollection == null || activity == null) {
        return;
      }

      activity.onDataLoaded(featureCollection);
    }
  }

  public void onDataLoaded(@NonNull FeatureCollection featureCollection) {
    // create expressions
    Expression iconImageExpression = string(get(literal(FEATURE_ID)));
    Expression iconSizeExpression = division(number(get(literal(FEATURE_RANK))), literal(2.0f));
    Expression textSizeExpression = product(get(literal(FEATURE_RANK)), pi());
    Expression textFieldExpression = concat(upcase(literal("a ")), upcase(string(get(literal(FEATURE_TYPE)))),
      downcase(literal(" IN ")), string(get(literal(FEATURE_REGION)))
    );
    Expression textColorExpression = match(get(literal(FEATURE_RANK)),
      literal(1), rgba(255, 0, 0, 1.0f),
      literal(2), rgba(0, 0, 255.0f, 1.0f),
      rgba(0.0f, 255.0f, 0.0f, 1.0f)
    );

    rgba(
      division(literal(255), get(FEATURE_RANK)),
      literal(0.0f),
      literal(0.0f),
      literal(1.0f)
    );

    // create symbol layer
    SymbolLayer symbolLayer = new SymbolLayer(LAYER_ID, SOURCE_ID)
      .withProperties(
        // icon configuration
        iconImage(iconImageExpression),
        iconAllowOverlap(false),
        iconSize(iconSizeExpression),
        iconAnchor(ICON_ANCHOR_BOTTOM),
        iconOffset(step(zoom(), literal(new float[] {0f, 0f}),
          stop(1, new Float[] {0f, 0f}),
          stop(10, new Float[] {0f, -35f})
        )),

        // text field configuration
        textField(textFieldExpression),
        textSize(textSizeExpression),
        textAnchor(TEXT_ANCHOR_TOP),
        textColor(textColorExpression)
      );

    // add a geojson source to the map
    Source source = new GeoJsonSource(SOURCE_ID, featureCollection);
    mapboxMap.addSource(source);

    // add symbol layer
    mapboxMap.addLayer(symbolLayer);

    // get expressions
    Expression iconImageExpressionResult = symbolLayer.getIconImage().getExpression();
    Expression iconSizeExpressionResult = symbolLayer.getIconSize().getExpression();
    Expression textSizeExpressionResult = symbolLayer.getTextSize().getExpression();
    Expression textFieldExpressionResult = symbolLayer.getTextField().getExpression();
    Expression textColorExpressionResult = symbolLayer.getTextColor().getExpression();

    // log expressions
    Timber.e(iconImageExpressionResult.toString());
    Timber.e(iconSizeExpressionResult.toString());
    Timber.e(textSizeExpressionResult.toString());
    Timber.e(textFieldExpressionResult.toString());
    Timber.e(textColorExpressionResult.toString());

    // reset expressions
    symbolLayer.setProperties(
      iconImage(iconImageExpressionResult),
      iconSize(iconSizeExpressionResult),
      textSize(textSizeExpressionResult),
      textField(textFieldExpressionResult),
      textColor(textColorExpressionResult)
    );

    new GenerateSymbolTask(mapboxMap, this).execute(featureCollection);
  }

  private static class GenerateSymbolTask extends AsyncTask<FeatureCollection, Void, HashMap<String, Bitmap>> {

    private MapboxMap mapboxMap;
    private WeakReference<Context> context;

    GenerateSymbolTask(MapboxMap mapboxMap, Context context) {
      this.mapboxMap = mapboxMap;
      this.context = new WeakReference<>(context);
    }

    @SuppressWarnings("WrongThread")
    @Override
    protected HashMap<String, Bitmap> doInBackground(FeatureCollection... params) {
      HashMap<String, Bitmap> imagesMap = new HashMap<>();
      Context context = this.context.get();
      List<Feature> features = params[0].features();
      if (context != null && features != null) {
        for (Feature feature : features) {
          String countryName = feature.getStringProperty(FEATURE_ID);
          TextView textView = new TextView(context);
          textView.setBackgroundColor(context.getResources().getColor(R.color.blueAccent));
          textView.setPadding(10, 5, 10, 5);
          textView.setTextColor(Color.WHITE);
          textView.setText(countryName);
          imagesMap.put(countryName, SymbolGenerator.generate(textView));
        }
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