package com.mapbox.mapboxsdk.testapp.activity.style;

import android.os.Bundle;
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

import static com.mapbox.mapboxsdk.style.functions.Function.zoom;
import static com.mapbox.mapboxsdk.style.functions.stops.Stop.stop;
import static com.mapbox.mapboxsdk.style.functions.stops.Stops.interval;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;

public class ZoomFunctionSymbolLayerActivity extends AppCompatActivity {

  private static final String SOURCE_URL = "https://gist.githubusercontent.com/anonymous/"
    + "70ddad0e58520307cd1699e704f4b626/raw/a5deaccdc4517c12d63b1f8abcb7becf5e36506a/map.geojson";
  private static final String LAYER_ID = "symbolLayer";
  private static final String SOURCE_ID = "poiSource";
  private static final String BUS_MAKI_ICON_ID = "bus-11";
  private static final String CAFE_MAKI_ICON_ID = "cafe-11";
  private static final float ZOOM_STOP_MIN_VALUE = 7.0f;
  private static final float ZOOM_STOP_MAX_VALUE = 12.0f;

  private MapView mapView;
  private MapboxMap mapboxMap;
  private GeoJsonSource source;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_zoom_symbol_layer);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(@NonNull final MapboxMap map) {
        mapboxMap = map;
        addSource();
        addLayer();
      }
    });
  }

  private void addSource() {
    try {
      source = new GeoJsonSource(SOURCE_ID, new URL(SOURCE_URL));
    } catch (MalformedURLException exception) {
      Timber.e(exception, "That's not an url... ");
    }
    mapboxMap.addSource(source);
  }

  private void addLayer() {
    SymbolLayer layer = new SymbolLayer(LAYER_ID, SOURCE_ID);
    layer.setProperties(
      iconImage(
        zoom(
          interval(
            stop(ZOOM_STOP_MIN_VALUE, iconImage(BUS_MAKI_ICON_ID)),
            stop(ZOOM_STOP_MAX_VALUE, iconImage(CAFE_MAKI_ICON_ID))
          )
        )
      ),
      iconAllowOverlap(true)
    );
    mapboxMap.addLayer(layer);
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
}