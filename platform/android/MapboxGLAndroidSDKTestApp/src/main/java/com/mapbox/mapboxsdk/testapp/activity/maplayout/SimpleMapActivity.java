package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.view.MenuItem;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.NavUtils;
import com.mapbox.mapboxsdk.utils.BitmapUtils;

import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconIgnorePlacement;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textField;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textIgnorePlacement;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textOffset;

/**
 * Test activity showcasing a simple MapView without any MapboxMap interaction.
 */
public class SimpleMapActivity extends AppCompatActivity {

  private MapView mapView;
  private static final String ICON_SOURCE_ID = "ICON_SOURCE_ID";
  private static final String ICON_LAYER_ID = "ICON_LAYER_ID";
  private static final String PROFILE_NAME = "PROFILE_NAME";
  private static final String CARLOS = "Carlos";
  private static final String ANTONY = "Antony";

  private static final String MARIA = "Maria";
  private static final String LUCIANA = "Luciana";
  private MapboxMap mapboxMap;


  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_simple);
    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(@NonNull final MapboxMap mapboxMap) {

        // Add Features which represent the location of each profile photo SymbolLayer icon
        Feature carlosFeature = Feature.fromGeometry(Point.fromLngLat(-7.9760742,
          41.2778064));
        carlosFeature.addStringProperty(PROFILE_NAME, CARLOS);

        Feature antonyFeature = Feature.fromGeometry(Point.fromLngLat(-8.0639648,
          37.5445773));
        antonyFeature.addStringProperty(PROFILE_NAME, ANTONY);

        Feature mariaFeature = Feature.fromGeometry(Point.fromLngLat(-9.1845703,
          38.9764924));
        mariaFeature.addStringProperty(PROFILE_NAME, MARIA);

        Feature lucianaFeature = Feature.fromGeometry(Point.fromLngLat(-7.5146484,
          40.2459915));
        lucianaFeature.addStringProperty(PROFILE_NAME, LUCIANA);

        // Use a URL to build and add a Style object to the map. Then add a source to the Style.
        mapboxMap.setStyle(
          new Style.Builder().fromUri(Style.LIGHT)
            .withSource(new GeoJsonSource(ICON_SOURCE_ID,
              FeatureCollection.fromFeatures(new Feature[] {
                carlosFeature,
                antonyFeature,
                mariaFeature,
                lucianaFeature}))),
          new Style.OnStyleLoaded() {
            @Override
            public void onStyleLoaded(@NonNull Style style) {
              SimpleMapActivity.this.mapboxMap = mapboxMap;

              // Add a SymbolLayer to the style. iconImage is set to a value that will
              // be used later in the addOnStyleImageMissingListener below
              style.addLayer(new SymbolLayer(ICON_LAYER_ID, ICON_SOURCE_ID).withProperties(
                iconImage(get(PROFILE_NAME)),
                iconIgnorePlacement(true),
                iconAllowOverlap(true),
                textField(get(PROFILE_NAME)),
                textIgnorePlacement(true),
                textAllowOverlap(true),
                textOffset(new Float[] {0f, 2f})
              ));
            }
          });
      }
    });

    // Use the listener to match the id with the appropriate person. The correct profile photo is
    // given to the map during "runtime".
    mapView.addOnStyleImageMissingListener(new MapView.OnStyleImageMissingListener() {
      @Override
      public void onStyleImageMissing(@NonNull String id) {
        switch (id) {
          case CARLOS:
            addImage(id, R.drawable.carlos);
            break;
          case ANTONY:
            addImage(id, R.drawable.antony);
            break;
          case MARIA:
            addImage(id, R.drawable.maria);
            break;
          case LUCIANA:
            addImage(id, R.drawable.luciana);
            break;
          default:
            addImage(id, R.drawable.carlos);
            break;
        }
      }
    });
  }

  private void addImage(String id, int drawableImage) {
    Style style = mapboxMap.getStyle();
    if (style != null) {
      style.addImageAsync(id, BitmapUtils.getBitmapFromDrawable(
        getResources().getDrawable(drawableImage)));
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
