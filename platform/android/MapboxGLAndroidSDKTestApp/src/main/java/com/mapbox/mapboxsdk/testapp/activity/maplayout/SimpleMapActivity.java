package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.graphics.Bitmap;
import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.MenuItem;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.PropertyFactory;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.NavUtils;
import com.mapbox.mapboxsdk.utils.BitmapUtils;

/**
 * Test activity showcasing a simple MapView without any MapboxMap interaction.
 */
public class SimpleMapActivity extends AppCompatActivity {

  private MapView mapView;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_simple);
    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(mapboxMap -> mapboxMap.setStyle(
      new Style.Builder().fromUrl(Style.MAPBOX_STREETS), new Style.OnStyleLoaded() {
        @Override
        public void onStyleLoaded(@NonNull Style style) {
          mapboxMap.setDebugActive(true);

          Feature feature1 = Feature.fromGeometry(Point.fromLngLat(16, 50));
          feature1.addStringProperty("icon", "car-15" /*MAKI*/);

          Feature feature2 = Feature.fromGeometry(Point.fromLngLat(-15, 40));
          feature2.addStringProperty("icon", "sdf-image");

          Bitmap bitmap = BitmapUtils.getBitmapFromDrawable(ContextCompat.getDrawable(SimpleMapActivity.this,
            R.drawable.mapbox_user_puck_icon));
          if (bitmap != null) {
            style.addImage("sdf-image", bitmap, true /*SDF*/);
          }

          style.addSource(new GeoJsonSource("source",
            FeatureCollection.fromFeatures(new Feature[] {feature1, feature2})));
          style.addLayer(new SymbolLayer("layer", "source")
            .withProperties(
              PropertyFactory.iconImage(Expression.get("icon")),
              PropertyFactory.iconColor(Color.RED))
          );
        }
      }
    ));
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
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {
      case android.R.id.home:
        // activity uses singleInstance for testing purposes
        // code below provides a default navigation when using the app
        onBackPressed();
        return true;
    }
    return super.onOptionsItemSelected(item);
  }

  @Override
  public void onBackPressed() {
    // activity uses singleInstance for testing purposes
    // code below provides a default navigation when using the app
    NavUtils.navigateHome(this);
  }
}
