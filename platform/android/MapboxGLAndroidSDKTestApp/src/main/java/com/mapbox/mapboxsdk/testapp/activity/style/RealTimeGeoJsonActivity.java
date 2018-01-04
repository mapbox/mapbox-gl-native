package com.mapbox.mapboxsdk.testapp.activity.style;

import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;

import java.net.MalformedURLException;
import java.net.URL;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;

/**
 * Test activity showcasing using realtime GeoJSON to move a symbol on your map
 * <p>
 * GL-native equivalent of https://www.mapbox.com/mapbox-gl-js/example/live-geojson/
 * </p>
 */
public class RealTimeGeoJsonActivity extends AppCompatActivity implements OnMapReadyCallback {

  private static final String ID_GEOJSON_LAYER = "wanderdrone";
  private static final String ID_GEOJSON_SOURCE = ID_GEOJSON_LAYER;
  private static final String URL_GEOJSON_SOURCE = "https://wanderdrone.appspot.com/";

  private MapView mapView;
  private MapboxMap mapboxMap;

  private Handler handler;
  private Runnable runnable;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_default);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
  }

  @Override
  public void onMapReady(@NonNull final MapboxMap map) {
    mapboxMap = map;

    // add source
    try {
      mapboxMap.addSource(new GeoJsonSource(ID_GEOJSON_SOURCE, new URL(URL_GEOJSON_SOURCE)));
    } catch (MalformedURLException malformedUrlException) {
      Timber.e(malformedUrlException, "Invalid URL");
    }

    // add layer
    SymbolLayer layer = new SymbolLayer(ID_GEOJSON_LAYER, ID_GEOJSON_SOURCE);
    layer.setProperties(iconImage("rocket-15"));
    mapboxMap.addLayer(layer);

    // loop refresh geojson
    handler = new Handler();
    runnable = new RefreshGeoJsonRunnable(mapboxMap, handler);
    handler.postDelayed(runnable, 2000);
  }

  @Override
  protected void onStart() {
    super.onStart();
    mapView.onStart();
  }

  @Override
  public void onResume() {
    super.onResume();
    mapView.onResume();
  }

  @Override
  public void onPause() {
    super.onPause();
    mapView.onPause();
  }

  @Override
  protected void onStop() {
    super.onStop();
    mapView.onStop();
    handler.removeCallbacks(runnable);
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

  private static class RefreshGeoJsonRunnable implements Runnable {

    private MapboxMap mapboxMap;
    private Handler handler;

    RefreshGeoJsonRunnable(MapboxMap mapboxMap, Handler handler) {
      this.mapboxMap = mapboxMap;
      this.handler = handler;
    }

    @Override
    public void run() {
      ((GeoJsonSource) mapboxMap.getSource(ID_GEOJSON_SOURCE)).setUrl(URL_GEOJSON_SOURCE);
      handler.postDelayed(this, 2000);
    }
  }
}
