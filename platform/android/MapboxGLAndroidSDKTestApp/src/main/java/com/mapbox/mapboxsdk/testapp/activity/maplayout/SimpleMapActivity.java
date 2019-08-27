package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.animation.ObjectAnimator;
import android.animation.TypeEvaluator;
import android.animation.ValueAnimator;
import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
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

  private ObjectAnimator animator;

  private LatLng[] latlngs = new LatLng[] {new LatLng(), new LatLng(50, 16)};

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_simple);
    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(mapboxMap -> mapboxMap.setStyle(
      new Style.Builder().fromUri(Style.MAPBOX_STREETS), new Style.OnStyleLoaded() {
        @Override
        public void onStyleLoaded(@NonNull Style style) {
          GeoJsonSource source = new GeoJsonSource("source");

          CircleLayer circle = new CircleLayer("circle", "source")
            .withProperties(
              PropertyFactory.circleRadius(50f),
              PropertyFactory.circleColor(Color.RED)
            );

          style.addImage("image1", BitmapUtils.getBitmapFromDrawable(
            ContextCompat.getDrawable(SimpleMapActivity.this, R.drawable.ic_car_top)));
          SymbolLayer symbol1 = new SymbolLayer("symbol1", "source")
            .withProperties(
              PropertyFactory.iconImage("image1"),
              PropertyFactory.iconAllowOverlap(true),
              PropertyFactory.iconIgnorePlacement(true)
            );

          style.addImage("image2", BitmapUtils.getBitmapFromDrawable(
            ContextCompat.getDrawable(SimpleMapActivity.this, R.drawable.ic_delete)));
          SymbolLayer symbol2 = new SymbolLayer("symbol2", "source")
            .withProperties(
              PropertyFactory.iconImage("image2"),
              PropertyFactory.iconAllowOverlap(true),
              PropertyFactory.iconIgnorePlacement(true)
            );

          style.addSource(source);
          style.addLayer(circle);
          style.addLayer(symbol1);
          style.addLayer(symbol2);

          animator = new ObjectAnimator();
          animator.setObjectValues(latlngs);
          animator.setEvaluator(new LatLngEvaluator());
          animator.setDuration(2000);
          animator.setRepeatCount(ValueAnimator.INFINITE);
          animator.setRepeatMode(ValueAnimator.REVERSE);
          animator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
            @Override
            public void onAnimationUpdate(ValueAnimator animation) {
              LatLng latLng = (LatLng) animation.getAnimatedValue();
              source.setGeoJson(Point.fromLngLat(latLng.getLongitude(), latLng.getLatitude()));
            }
          });
          animator.start();

          Button button = new Button(SimpleMapActivity.this);
          button.setText("toggle image");
          ViewGroup.LayoutParams params = new ViewGroup.LayoutParams(
            ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
          button.setLayoutParams(params);
          button.setOnClickListener(new View.OnClickListener() {

            private boolean hasImage = true;

            @Override
            public void onClick(View v) {
              if (hasImage) {
                style.removeImage("image2");
              } else {
                style.addImage("image2",
                  BitmapUtils.getBitmapFromDrawable(
                    ContextCompat.getDrawable(SimpleMapActivity.this, R.drawable.ic_delete)));
              }

              hasImage = !hasImage;
            }
          });

          mapView.addView(button);
        }
      }
    ));
  }

  class LatLngEvaluator implements TypeEvaluator<LatLng> {

    private final LatLng latLng = new LatLng();

    @NonNull
    @Override
    public LatLng evaluate(float fraction, @NonNull LatLng startValue, @NonNull LatLng endValue) {
      latLng.setLatitude(startValue.getLatitude()
        + ((endValue.getLatitude() - startValue.getLatitude()) * fraction));
      latLng.setLongitude(startValue.getLongitude()
        + ((endValue.getLongitude() - startValue.getLongitude()) * fraction));
      return latLng;
    }
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
    if (animator != null) {
      animator.cancel();
    }
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
