package com.mapbox.mapboxsdk.testapp.activity.feature;

import android.graphics.BitmapFactory;
import android.graphics.RectF;
import android.os.Bundle;
import android.support.annotation.RawRes;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.services.commons.geojson.Feature;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;
import java.io.Writer;
import java.util.List;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;

/**
 * Test activity showcasing using the query rendered features API to count Symbols in a rectangle.
 */
public class QueryRenderedFeaturesBoxSymbolCountActivity extends AppCompatActivity {

  public MapView mapView;
  private MapboxMap mapboxMap;

  private Toast toast;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_query_features_box);

    final View selectionBox = findViewById(R.id.selection_box);

    // Initialize map as normal
    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {
      @SuppressWarnings("ConstantConditions")
      @Override
      public void onMapReady(final MapboxMap mapboxMap) {
        QueryRenderedFeaturesBoxSymbolCountActivity.this.mapboxMap = mapboxMap;

        // Add a symbol layer (also works with annotations)
        try {
          mapboxMap.addSource(new GeoJsonSource("symbols-source", readRawResource(R.raw.test_points_utrecht)));
        } catch (IOException ioException) {
          Timber.e("Could not load geojson: " + ioException.getMessage());
          return;
        }
        mapboxMap.addImage(
          "test-icon",
          BitmapFactory.decodeResource(getResources(),
            R.drawable.mapbox_marker_icon_default)
        );
        mapboxMap.addLayer(new SymbolLayer("symbols-layer", "symbols-source").withProperties(iconImage("test-icon")));

        selectionBox.setOnClickListener(new View.OnClickListener() {
          @Override
          public void onClick(View view) {
            // Query
            int top = selectionBox.getTop() - mapView.getTop();
            int left = selectionBox.getLeft() - mapView.getLeft();
            RectF box = new RectF(left, top, left + selectionBox.getWidth(), top + selectionBox.getHeight());
            Timber.i(String.format("Querying box %s", box));
            List<Feature> features = mapboxMap.queryRenderedFeatures(box, "symbols-layer");

            // Show count
            if (toast != null) {
              toast.cancel();
            }
            toast = Toast.makeText(
              QueryRenderedFeaturesBoxSymbolCountActivity.this,
              String.format("%s features in box", features.size()),
              Toast.LENGTH_SHORT);
            toast.show();
          }
        });
      }
    });
  }

  private String readRawResource(@RawRes int rawResource) throws IOException {
    InputStream is = getResources().openRawResource(rawResource);
    Writer writer = new StringWriter();
    char[] buffer = new char[1024];
    try {
      Reader reader = new BufferedReader(new InputStreamReader(is, "UTF-8"));
      int numRead;
      while ((numRead = reader.read(buffer)) != -1) {
        writer.write(buffer, 0, numRead);
      }
    } finally {
      is.close();
    }

    return writer.toString();
  }

  public MapboxMap getMapboxMap() {
    return mapboxMap;
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
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }
}
