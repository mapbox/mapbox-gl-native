package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.RawRes;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.functions.stops.Stops;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.mapboxsdk.testapp.R;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;
import java.io.Writer;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.functions.Function.composite;
import static com.mapbox.mapboxsdk.style.functions.Function.property;
import static com.mapbox.mapboxsdk.style.functions.Function.zoom;
import static com.mapbox.mapboxsdk.style.functions.stops.Stop.stop;
import static com.mapbox.mapboxsdk.style.functions.stops.Stops.categorical;
import static com.mapbox.mapboxsdk.style.functions.stops.Stops.exponential;
import static com.mapbox.mapboxsdk.style.functions.stops.Stops.interval;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillAntialias;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillOutlineColor;

/**
 * Test activity showcasing the data driven runtime style API.
 */
public class DataDrivenStyleActivity extends AppCompatActivity {

  public static final String AMSTERDAM_PARKS_LAYER = "amsterdam-parks-layer";
  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_data_driven_style);

    // Initialize map as normal
    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);


    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(MapboxMap map) {
        // Store for later
        mapboxMap = map;

        // Add a parks layer
        addParksLayer();

        // Center and Zoom (Amsterdam, zoomed to streets)
        mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(52.379189, 4.899431), 14));
      }
    });
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_data_driven_style, menu);
    return true;
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

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    switch (item.getItemId()) {
      case R.id.action_add_exponential_zoom_function:
        addExponentialZoomFunction();
        return true;
      case R.id.action_add_interval_zoom_function:
        addIntervalZoomFunction();
        return true;
      case R.id.action_add_categorical_source_function:
        addCategoricalSourceFunction();
        return true;
      case R.id.action_add_exponential_source_function:
        addExponentialSourceFunction();
        return true;
      case R.id.action_add_identity_source_function:
        addIdentitySourceFunction();
        return true;
      case R.id.action_add_interval_source_function:
        addIntervalSourceFunction();
        return true;
      case R.id.action_add_composite_categorical_function:
        addCompositeCategoricalFunction();
        return true;
      case R.id.action_add_composite_exponential_function:
        addCompositeExponentialFunction();
        return true;
      case R.id.action_add_composite_interval_function:
        addCompositeIntervalFunction();
        return true;
      default:
        return super.onOptionsItemSelected(item);
    }
  }


  private void addExponentialZoomFunction() {
    Timber.i("Add exponential zoom function");
    FillLayer layer = mapboxMap.getLayerAs("water");
    assert layer != null;
    layer.setProperties(
      fillColor(
        zoom(
          exponential(
            stop(1, fillColor(Color.RED)),
            stop(5, fillColor(Color.BLUE)),
            stop(10, fillColor(Color.GREEN))
          ).withBase(0.5f)
        )
      )
    );

    Timber.i("Fill color: %s", layer.getFillColor());
  }

  private void addIntervalZoomFunction() {
    Timber.i("Add interval zoom function");
    FillLayer layer = mapboxMap.getLayerAs("water");
    assert layer != null;
    layer.setProperties(
      fillColor(
        zoom(
          interval(
            stop(1, fillColor(Color.RED)),
            stop(5, fillColor(Color.BLUE)),
            stop(10, fillColor(Color.GREEN))
          )
        )
      )
    );

    Timber.i("Fill color: %s", layer.getFillColor());
  }

  private void addExponentialSourceFunction() {
    Timber.i("Add exponential source function");
    FillLayer layer = mapboxMap.getLayerAs(AMSTERDAM_PARKS_LAYER);
    assert layer != null;
    layer.setProperties(
      fillColor(
        property(
          "stroke-width",
          exponential(
            stop(1f, fillColor(Color.RED)),
            stop(5f, fillColor(Color.BLUE)),
            stop(10f, fillColor(Color.GREEN))
          ).withBase(0.5f)
        )
      )
    );

    Timber.i("Fill color: %s", layer.getFillColor());
  }

  private void addCategoricalSourceFunction() {
    Timber.i("Add categorical source function");
    FillLayer layer = mapboxMap.getLayerAs(AMSTERDAM_PARKS_LAYER);
    assert layer != null;
    layer.setProperties(
      fillColor(
        property(
          "name",
          categorical(
            stop("Westerpark", fillColor(Color.RED)),
            stop("Jordaan", fillColor(Color.BLUE)),
            stop("Prinseneiland", fillColor(Color.GREEN))
          ))
      )
    );

    Timber.i("Fill color: %s", layer.getFillColor());
  }

  private void addIdentitySourceFunction() {
    Timber.i("Add identity source function");
    FillLayer layer = mapboxMap.getLayerAs(AMSTERDAM_PARKS_LAYER);
    assert layer != null;
    layer.setProperties(
      fillOpacity(
        property(
          "fill-opacity",
          Stops.<Float>identity())
      )
    );

    Timber.i("Fill opacity: %s", layer.getFillOpacity());
  }

  private void addIntervalSourceFunction() {
    Timber.i("Add interval source function");
    FillLayer layer = mapboxMap.getLayerAs(AMSTERDAM_PARKS_LAYER);
    assert layer != null;
    layer.setProperties(
      fillColor(
        property(
          "stroke-width",
          interval(
            stop(1f, fillColor(Color.RED)),
            stop(5f, fillColor(Color.BLUE)),
            stop(10f, fillColor(Color.GREEN))
          ))
      )
    );

    Timber.i("Fill color: %s", layer.getFillColor());
  }

  private void addCompositeExponentialFunction() {
    Timber.i("Add composite exponential function");
    FillLayer layer = mapboxMap.getLayerAs(AMSTERDAM_PARKS_LAYER);
    assert layer != null;
    layer.setProperties(
      fillColor(
        composite(
          "stroke-width",
          exponential(
            stop(1, 1, fillColor(Color.RED)),
            stop(10, 2, fillColor(Color.BLUE)),
            stop(22, 3, fillColor(Color.GREEN)),
            stop(1, 1, fillColor(Color.CYAN)),
            stop(10, 2, fillColor(Color.GRAY)),
            stop(22, 3, fillColor(Color.YELLOW))
          ).withBase(1f)
        )
      )
    );

    Timber.i("Fill color: %s", layer.getFillColor());
  }

  private void addCompositeIntervalFunction() {
    Timber.i("Add composite exponential function");
    FillLayer layer = mapboxMap.getLayerAs(AMSTERDAM_PARKS_LAYER);
    assert layer != null;
    layer.setProperties(
      fillColor(
        composite(
          "stroke-width",
          interval(
            stop(1, 1, fillColor(Color.RED)),
            stop(10, 2, fillColor(Color.BLUE)),
            stop(22, 3, fillColor(Color.GREEN)),
            stop(1, 1, fillColor(Color.CYAN)),
            stop(10, 2, fillColor(Color.GRAY)),
            stop(22, 3, fillColor(Color.YELLOW))
          ))
      )
    );

    Timber.i("Fill color: %s", layer.getFillColor());
  }

  private void addCompositeCategoricalFunction() {
    Timber.i("Add composite categorical function");
    FillLayer layer = mapboxMap.getLayerAs(AMSTERDAM_PARKS_LAYER);
    assert layer != null;
    layer.setProperties(
      fillColor(
        composite(
          "name",
          categorical(
            stop(7f, "Westerpark", fillColor(Color.RED)),
            stop(8f, "Westerpark", fillColor(Color.BLUE)),
            stop(9f, "Westerpark", fillColor(Color.RED)),
            stop(10f, "Westerpark", fillColor(Color.BLUE)),
            stop(11f, "Westerpark", fillColor(Color.RED)),
            stop(12f, "Westerpark", fillColor(Color.BLUE)),
            stop(13f, "Westerpark", fillColor(Color.RED)),
            stop(14f, "Westerpark", fillColor(Color.BLUE)),
            stop(15f, "Westerpark", fillColor(Color.RED)),
            stop(16f, "Westerpark", fillColor(Color.BLUE)),
            stop(17f, "Westerpark", fillColor(Color.RED)),
            stop(18f, "Westerpark", fillColor(Color.BLUE)),
            stop(19f, "Westerpark", fillColor(Color.RED)),
            stop(20f, "Westerpark", fillColor(Color.BLUE)),
            stop(21f, "Westerpark", fillColor(Color.RED)),
            stop(22f, "Westerpark", fillColor(Color.BLUE)),
            stop(14f, "Jordaan", fillColor(Color.GREEN)),
            stop(18f, "Jordaan", fillColor(Color.CYAN)),
            stop(14f, "Prinseneiland", fillColor(Color.WHITE)),
            stop(18f, "Prinseneiland", fillColor(Color.BLACK))
          ))
      )
    );

    Timber.i("Fill color: %s", layer.getFillColor());
  }

  private void addParksLayer() {
    // Add a source
    Source source;
    try {
      source = new GeoJsonSource("amsterdam-parks-source", readRawResource(R.raw.amsterdam));
      mapboxMap.addSource(source);
    } catch (IOException ioException) {
      Toast.makeText(
        DataDrivenStyleActivity.this,
        "Couldn't add source: " + ioException.getMessage(),
        Toast.LENGTH_SHORT).show();
      return;
    }


    // Add a fill layer
    mapboxMap.addLayer(new FillLayer(AMSTERDAM_PARKS_LAYER, source.getId())
      .withProperties(
        fillColor(Color.BLACK),
        fillOutlineColor(Color.BLUE),
        fillAntialias(true)
      )
    );
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
}
