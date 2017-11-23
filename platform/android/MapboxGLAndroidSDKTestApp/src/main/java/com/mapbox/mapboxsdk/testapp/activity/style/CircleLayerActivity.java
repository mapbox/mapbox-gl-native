package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Color;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.layers.LineLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;

import java.net.MalformedURLException;
import java.net.URL;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.layers.Filter.in;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleRadius;

/**
 * Test activity showcasing adding a Circle Layer to the Map
 * <p>
 * Uses bus stop data from Singapore as a source and allows to filter into 1 specific route with a line layer.
 * </p>
 */
public class CircleLayerActivity extends AppCompatActivity implements View.OnClickListener {

  private MapView mapView;
  private MapboxMap mapboxMap;

  private FloatingActionButton styleFab;
  private FloatingActionButton routeFab;

  private CircleLayer layer;
  private GeoJsonSource source;

  private int currentStyleIndex = 0;
  private boolean isLoadingStyle = true;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_circle_layer);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;
      addBusStopSource();
      addBusStopCircleLayer();
      initFloatingActionButtons();
      isLoadingStyle = false;
    });
  }

  private void addBusStopSource() {
    try {
      source = new GeoJsonSource("bus_stop",
        new URL("https://raw.githubusercontent.com/cheeaun/busrouter-sg/master/data/2/bus-stops.geojson"));
    } catch (MalformedURLException exception) {
      Timber.e(exception, "That's not an url... ");
    }
    mapboxMap.addSource(source);
  }

  private void addBusStopCircleLayer() {
    layer = new CircleLayer("stops_layer", "bus_stop");
    layer.setProperties(
      circleColor(Color.parseColor("#FF9800")),
      circleRadius(2.0f)
    );
    mapboxMap.addLayer(layer);
  }

  private void initFloatingActionButtons() {
    routeFab = (FloatingActionButton) findViewById(R.id.fab_route);
    routeFab.setColorFilter(ContextCompat.getColor(CircleLayerActivity.this, R.color.primary));
    routeFab.setOnClickListener(CircleLayerActivity.this);

    styleFab = (FloatingActionButton) findViewById(R.id.fab_style);
    styleFab.setOnClickListener(CircleLayerActivity.this);
  }

  @Override
  public void onClick(View view) {
    if (isLoadingStyle) {
      return;
    }

    if (view.getId() == R.id.fab_route) {
      showBusRoute();
    } else if (view.getId() == R.id.fab_style) {
      changeMapStyle();
    }
  }

  private void showBusRoute() {
    removeFabs();
    applyBusRouteFilterToBusStopSource();
    addBusRouteSource();
    addBusRouteLayer();
  }

  private void removeFabs() {
    routeFab.setVisibility(View.GONE);
    styleFab.setVisibility(View.GONE);
  }

  private void applyBusRouteFilterToBusStopSource() {
    layer.setFilter(in("number", (Object[]) Data.STOPS_FOR_ROUTE));
  }

  private void addBusRouteSource() {
    try {
      mapboxMap.addSource(new GeoJsonSource("bus_route",
        new URL("https://gist.githubusercontent.com/tobrun/7fbc0fe7e9ffea509f7608cda2601d5d/raw/"
          + "a4b8cc289020f91fa2e1553524820054395e36f5/line.geojson")));
    } catch (MalformedURLException malformedUrlException) {
      Timber.e(malformedUrlException, "That's not an url... ");
    }
  }

  private void addBusRouteLayer() {
    LineLayer lineLayer = new LineLayer("route_layer", "bus_route");
    mapboxMap.addLayerBelow(lineLayer, "stops_layer");
    mapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(
      new CameraPosition.Builder()
        .target(new LatLng(1.3896777, 103.9874997))
        .bearing(225)
        .tilt(45)
        .zoom(13)
        .build()
    ), 1750);
  }

  private void changeMapStyle() {
    isLoadingStyle = true;
    removeBusStop();
    loadNewStyle();
  }

  private void removeBusStop() {
    mapboxMap.removeLayer(layer);
    mapboxMap.removeSource(source);
  }

  private void loadNewStyle() {
    mapboxMap.setStyleUrl(getNextStyle(), style -> {
      addBusStop();
      isLoadingStyle = false;
    });
  }

  private void addBusStop() {
    mapboxMap.addLayer(layer);
    mapboxMap.addSource(source);
  }

  private String getNextStyle() {
    currentStyleIndex++;
    if (currentStyleIndex == Data.STYLES.length) {
      currentStyleIndex = 0;
    }
    return Data.STYLES[currentStyleIndex];
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

  private static class Data {
    private static final String[] STOPS_FOR_ROUTE = new String[] {"99009", "99131", "99049", "99039", "99029", "99019",
      "98079", "98069", "97099", "97089", "97079", "97069", "97209", "97059", "97049", "97039", "97019", "96069",
      "96059", "96049", "96099", "96089", "96079", "85079", "85089", "85069", "85059", "85099", "84069", "84059",
      "84049", "84039", "84029", "84019", "83099", "83079", "83059", "83049", "83029", "82069", "82049", "82029",
      "82109", "81069", "81049", "81029", "80089", "80069", "80049", "80039", "80029", "01319", "01219", "01129",
      "01059", "01119", "01019", "04159", "04149", "04229", "04239", "05059", "05049", "05039", "05019", "10589"};

    private static final String[] STYLES = new String[] {
      Style.MAPBOX_STREETS,
      Style.OUTDOORS,
      Style.LIGHT,
      Style.DARK,
      Style.SATELLITE,
      Style.SATELLITE_STREETS
    };
  }
}
