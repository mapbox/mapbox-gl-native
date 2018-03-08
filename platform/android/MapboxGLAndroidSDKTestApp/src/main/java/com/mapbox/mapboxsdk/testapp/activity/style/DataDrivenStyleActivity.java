package com.mapbox.mapboxsdk.testapp.activity.style;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.Toast;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.style.sources.Source;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.ResourceUtils;

import java.io.IOException;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.expressions.Expression.exponential;
import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.expressions.Expression.interpolate;
import static com.mapbox.mapboxsdk.style.expressions.Expression.linear;
import static com.mapbox.mapboxsdk.style.expressions.Expression.literal;
import static com.mapbox.mapboxsdk.style.expressions.Expression.match;
import static com.mapbox.mapboxsdk.style.expressions.Expression.rgb;
import static com.mapbox.mapboxsdk.style.expressions.Expression.rgba;
import static com.mapbox.mapboxsdk.style.expressions.Expression.step;
import static com.mapbox.mapboxsdk.style.expressions.Expression.stop;
import static com.mapbox.mapboxsdk.style.expressions.Expression.zoom;
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

    mapView.getMapAsync(map -> {
      // Store for later
      mapboxMap = map;

      // Add a parks layer
      addParksLayer();

      // Add debug overlay
      setupDebugZoomView();

      // Center and Zoom (Amsterdam, zoomed to streets)
      mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(52.379189, 4.899431), 14));
    });
  }

  private void setupDebugZoomView() {
    final TextView textView = (TextView) findViewById(R.id.textZoom);
    mapboxMap.setOnCameraChangeListener(new MapboxMap.OnCameraChangeListener() {
      @Override
      public void onCameraChange(CameraPosition position) {
        textView.setText(String.format(getString(R.string.debug_zoom), position.zoom));
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
        interpolate(
          exponential(0.5f), zoom(),
          stop(1, rgb(255, 0, 0)),
          stop(5, rgb(0, 0, 255)),
          stop(10, rgb(0, 255, 0))
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
        step(zoom(),
          rgba(0.0f, 255.0f, 255.0f, 1.0f),
          stop(1, rgba(255.0f, 0.0f, 0.0f, 1.0f)),
          stop(5, rgba(0.0f, 0.0f, 255.0f, 1.0f)),
          stop(10, rgba(0.0f, 255.0f, 0.0f, 1.0f))
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
        interpolate(
          exponential(0.5f),
          get("stroke-width"),
          stop(1f, rgba(255.0f, 0.0f, 0.0f, 1.0f)),
          stop(5f, rgba(0.0f, 0.0f, 255.0f, 1.0f)),
          stop(10f, rgba(0.0f, 255.0f, 0.0f, 1.0f))
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
        match(
          get("name"),
          literal("Westerpark"), rgba(255.0f, 0.0f, 0.0f, 1.0f),
          literal("Jordaan"), rgba(0.0f, 0.0f, 255.0f, 1.0f),
          literal("Prinseneiland"), rgba(0.0f, 255.0f, 0.0f, 1.0f),
          rgba(0.0f, 255.0f, 255.0f, 1.0f)
        )
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
        get("fill-opacity")
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
        step(
          get("stroke-width"),
          rgba(0.0f, 255.0f, 255.0f, 1.0f),
          stop(1f, rgba(255.0f, 0.0f, 0.0f, 1.0f)),
          stop(2f, rgba(0.0f, 0.0f, 255.0f, 1.0f)),
          stop(3f, rgba(0.0f, 255.0f, 0.0f, 1.0f))
        )
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
        interpolate(
          exponential(1f),
          zoom(),
          stop(12, step(
            get("stroke-width"),
            rgba(255.0f, 255.0f, 255.0f, 1.0f),
            stop(1f, rgba(255.0f, 0.0f, 0.0f, 1.0f)),
            stop(2f, rgba(0.0f, 0.0f, 0.0f, 1.0f)),
            stop(3f, rgba(0.0f, 0.0f, 255.0f, 1.0f))
          )),
          stop(15, step(
            get("stroke-width"),
            rgba(255.0f, 255.0f, 255.0f, 1.0f),
            stop(1f, rgba(255.0f, 255.0f, 0.0f, 1.0f)),
            stop(2f, rgba(211.0f, 211.0f, 211.0f, 1.0f)),
            stop(3f, rgba(0.0f, 255.0f, 255.0f, 1.0f))
          )),
          stop(18, step(
            get("stroke-width"),
            rgba(255.0f, 255.0f, 255.0f, 1.0f),
            stop(1f, rgba(0.0f, 0.0f, 0.0f, 1.0f)),
            stop(2f, rgba(128.0f, 128.0f, 128.0f, 1.0f)),
            stop(3f, rgba(0.0f, 255.0f, 0.0f, 1.0f)))
          )
        )
      )
    );

    Timber.i("Fill color: %s", layer.getFillColor());
  }

  private void addCompositeIntervalFunction() {
    Timber.i("Add composite interval function");
    FillLayer layer = mapboxMap.getLayerAs(AMSTERDAM_PARKS_LAYER);
    assert layer != null;
    layer.setProperties(
      fillColor(
        interpolate(
          linear(),
          zoom(),
          stop(12, step(
            get("stroke-width"),
            rgba(255.0f, 255.0f, 255.0f, 1.0f),
            stop(1f, rgba(255.0f, 0.0f, 0.0f, 1.0f)),
            stop(2f, rgba(0.0f, 0.0f, 0.0f, 1.0f)),
            stop(3f, rgba(0.0f, 0.0f, 255.0f, 1.0f))
          )),
          stop(15, step(
            get("stroke-width"),
            rgba(255.0f, 255.0f, 255.0f, 1.0f),
            stop(1f, rgba(255.0f, 255.0f, 0.0f, 1.0f)),
            stop(2f, rgba(211.0f, 211.0f, 211.0f, 1.0f)),
            stop(3f, rgba(0.0f, 255.0f, 255.0f, 1.0f))
          )),
          stop(18, step(
            get("stroke-width"),
            rgba(255.0f, 255.0f, 255.0f, 1.0f),
            stop(1f, rgba(0.0f, 0.0f, 0.0f, 1.0f)),
            stop(2f, rgba(128.0f, 128.0f, 128.0f, 1.0f)),
            stop(3f, rgba(0.0f, 255.0f, 0.0f, 1.0f))
          ))
        )
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
        step(zoom(),
          rgba(255.0f, 255.0f, 255.0f, 1.0f),
          stop(7f, match(
            get("name"),
            literal("Westerpark"), rgba(255.0f, 0.0f, 0.0f, 1.0f),
            rgba(255.0f, 255.0f, 255.0f, 1.0f)
          )),
          stop(8f, match(
            get("name"),
            literal("Westerpark"), rgba(0.0f, 0.0f, 255.0f, 1.0f),
            rgba(255.0f, 255.0f, 255.0f, 1.0f)
          )),
          stop(9f, match(
            get("name"),
            literal("Westerpark"), rgba(255.0f, 0.0f, 0.0f, 1.0f),
            rgba(255.0f, 255.0f, 255.0f, 1.0f)
          )),
          stop(10f, match(
            get("name"),
            literal("Westerpark"), rgba(0.0f, 0.0f, 255.0f, 1.0f),
            rgba(255.0f, 255.0f, 255.0f, 1.0f)
          )),
          stop(11f, match(
            get("name"),
            literal("Westerpark"), rgba(255.0f, 0.0f, 0.0f, 1.0f),
            rgba(255.0f, 255.0f, 255.0f, 1.0f)
          )),
          stop(12f, match(
            get("name"),
            literal("Westerpark"), rgba(0.0f, 0.0f, 255.0f, 1.0f),
            rgba(255.0f, 255.0f, 255.0f, 1.0f)
          )),
          stop(13f, match(
            get("name"),
            literal("Westerpark"), rgba(255.0f, 0.0f, 0.0f, 1.0f),
            rgba(255.0f, 255.0f, 255.0f, 1.0f)
          )),
          stop(14f, match(
            get("name"),
            literal("Westerpark"), rgba(0.0f, 0.0f, 255.0f, 1.0f),
            literal("Jordaan"), rgba(0.0f, 255.0f, 0.0f, 1.0f),
            literal("PrinsenEiland"), rgba(0.0f, 0.0f, 0.0f, 1.0f),
            rgba(255.0f, 255.0f, 255.0f, 1.0f)
          )),
          stop(15f, match(
            get("name"),
            literal("Westerpark"), rgba(255.0f, 0.0f, 0.0f, 1.0f),
            rgba(255.0f, 255.0f, 255.0f, 1.0f)
          )),
          stop(16f, match(
            get("name"),
            literal("Westerpark"), rgba(0.0f, 0.0f, 255.0f, 1.0f),
            rgba(255.0f, 255.0f, 255.0f, 1.0f)
          )),
          stop(17f, match(
            get("name"),
            literal("Westerpark"), rgba(255.0f, 0.0f, 0.0f, 1.0f),
            rgba(255.0f, 255.0f, 255.0f, 1.0f)
          )),
          stop(18f, match(
            get("name"),
            literal("Westerpark"), rgba(0.0f, 0.0f, 255.0f, 1.0f),
            literal("Jordaan"), rgba(0.0f, 255.0f, 255.0f, 1.0f),
            rgba(255.0f, 255.0f, 255.0f, 1.0f)
          )),
          stop(19f, match(
            get("name"),
            literal("Westerpark"), rgba(255.0f, 0.0f, 0.0f, 1.0f),
            rgba(255.0f, 255.0f, 255.0f, 1.0f)
          )),
          stop(20f, match(
            get("name"),
            literal("Westerpark"), rgba(0.0f, 0.0f, 255.0f, 1.0f),
            rgba(255.0f, 255.0f, 255.0f, 1.0f)
          )),
          stop(21f, match(
            get("name"),
            literal("Westerpark"), rgba(255.0f, 0.0f, 0.0f, 1.0f),
            rgba(255.0f, 255.0f, 255.0f, 1.0f)
          )),
          stop(22f, match(
            get("name"),
            literal("Westerpark"), rgba(0.0f, 0.0f, 255.0f, 1.0f),
            rgba(255.0f, 255.0f, 255.0f, 1.0f)
          ))
        )
      )
    );

    Timber.i("Fill color: %s", layer.getFillColor());
  }

  private void addParksLayer() {
    // Add a source
    Source source;
    try {
      source = new GeoJsonSource("amsterdam-parks-source", ResourceUtils.readRawResource(this, R.raw.amsterdam));
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
        fillColor(rgba(0.0f, 0.0f, 0.0f, 0.5f)),
        fillOutlineColor(rgb(0, 0, 255)),
        fillAntialias(true)
      )
    );
  }
}
