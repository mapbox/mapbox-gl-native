package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Color;
import android.os.Bundle;
import com.google.android.material.floatingactionbutton.FloatingActionButton;
import androidx.core.content.ContextCompat;
import androidx.core.content.res.ResourcesCompat;
import androidx.appcompat.app.AppCompatActivity;
import android.view.View;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonOptions;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import timber.log.Timber;

import java.net.URI;
import java.net.URISyntaxException;

import static com.mapbox.mapboxsdk.style.expressions.Expression.all;
import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.expressions.Expression.gt;
import static com.mapbox.mapboxsdk.style.expressions.Expression.gte;
import static com.mapbox.mapboxsdk.style.expressions.Expression.has;
import static com.mapbox.mapboxsdk.style.expressions.Expression.literal;
import static com.mapbox.mapboxsdk.style.expressions.Expression.lt;
import static com.mapbox.mapboxsdk.style.expressions.Expression.toNumber;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleRadius;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textField;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textIgnorePlacement;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textSize;


/**
 * Test activity showcasing adding a Circle Layer to the Map
 * <p>
 * Uses bus stop data from Singapore as a source and allows to filter into 1 specific route with a line layer.
 * </p>
 */
public class CircleLayerActivity extends AppCompatActivity implements View.OnClickListener {

  public static final String SOURCE_ID = "bus_stop";
  public static final String SOURCE_ID_CLUSTER = "bus_stop_cluster";
  public static final String URL_BUS_ROUTES = "https://raw.githubusercontent.com/cheeaun/busrouter-sg/master/data/2/bus-stops.geojson";
  public static final String LAYER_ID = "stops_layer";
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

    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;
      mapboxMap.setStyle(Style.SATELLITE_STREETS);
      mapView.addOnDidFinishLoadingStyleListener(() -> {
        Style style = mapboxMap.getStyle();
        addBusStopSource(style);
        addBusStopCircleLayer(style);
        initFloatingActionButtons();
        isLoadingStyle = false;
      });
    });
  }

  private void addBusStopSource(Style style) {
    try {
      source = new GeoJsonSource(SOURCE_ID, new URI(URL_BUS_ROUTES));
    } catch (URISyntaxException exception) {
      Timber.e(exception, "That's not an url... ");
    }
    style.addSource(source);
  }

  private void addBusStopCircleLayer(Style style) {
    layer = new CircleLayer(LAYER_ID, SOURCE_ID);
    layer.setProperties(
      circleColor(Color.parseColor("#FF9800")),
      circleRadius(2.0f)
    );
    style.addLayerBelow(layer, "waterway-label");
  }

  private void initFloatingActionButtons() {
    routeFab = findViewById(R.id.fab_route);
    routeFab.setColorFilter(ContextCompat.getColor(CircleLayerActivity.this, R.color.primary));
    routeFab.setOnClickListener(CircleLayerActivity.this);

    styleFab = findViewById(R.id.fab_style);
    styleFab.setOnClickListener(CircleLayerActivity.this);
  }

  @Override
  public void onClick(View view) {
    if (isLoadingStyle) {
      return;
    }

    if (view.getId() == R.id.fab_route) {
      showBusCluster();
    } else if (view.getId() == R.id.fab_style) {
      changeMapStyle();
    }
  }

  private void showBusCluster() {
    removeFabs();
    removeOldSource();
    addClusteredSource();
  }

  private void removeOldSource() {
    mapboxMap.getStyle().removeSource(SOURCE_ID);
    mapboxMap.getStyle().removeLayer(LAYER_ID);
  }

  private void addClusteredSource() {
    try {
      mapboxMap.getStyle().addSource(
        new GeoJsonSource(SOURCE_ID_CLUSTER,
          new URI(URL_BUS_ROUTES),
          new GeoJsonOptions()
            .withCluster(true)
            .withClusterMaxZoom(14)
            .withClusterRadius(50)
        )
      );
    } catch (URISyntaxException malformedUrlException) {
      Timber.e(malformedUrlException, "That's not an url... ");
    }

    // Add unclustered layer
    int[][] layers = new int[][] {
      new int[] {150, ResourcesCompat.getColor(getResources(), R.color.redAccent, getTheme())},
      new int[] {20, ResourcesCompat.getColor(getResources(), R.color.greenAccent, getTheme())},
      new int[] {0, ResourcesCompat.getColor(getResources(), R.color.blueAccent, getTheme())}
    };

    SymbolLayer unclustered = new SymbolLayer("unclustered-points", SOURCE_ID_CLUSTER);
    unclustered.setProperties(
      iconImage("bus-15")
    );

    mapboxMap.getStyle().addLayer(unclustered);

    for (int i = 0; i < layers.length; i++) {
      // Add some nice circles
      CircleLayer circles = new CircleLayer("cluster-" + i, SOURCE_ID_CLUSTER);
      circles.setProperties(
        circleColor(layers[i][1]),
        circleRadius(18f)
      );

      Expression pointCount = toNumber(get("point_count"));
      circles.setFilter(
        i == 0
          ? all(has("point_count"),
          gte(pointCount, literal(layers[i][0]))
        ) : all(has("point_count"),
          gt(pointCount, literal(layers[i][0])),
          lt(pointCount, literal(layers[i - 1][0]))
        )
      );
      mapboxMap.getStyle().addLayer(circles);
    }

    // Add the count labels
    SymbolLayer count = new SymbolLayer("count", SOURCE_ID_CLUSTER);
    count.setProperties(
      textField(Expression.toString(get("point_count"))),
      textSize(12f),
      textColor(Color.WHITE),
      textIgnorePlacement(true),
      textAllowOverlap(true)
    );
    mapboxMap.getStyle().addLayer(count);
  }

  private void removeFabs() {
    routeFab.setVisibility(View.GONE);
    styleFab.setVisibility(View.GONE);
  }

  private void changeMapStyle() {
    isLoadingStyle = true;
    removeBusStop();
    loadNewStyle();
  }

  private void removeBusStop() {
    mapboxMap.getStyle().removeLayer(layer);
    mapboxMap.getStyle().removeSource(source);
  }

  private void loadNewStyle() {
    mapboxMap.setStyle(new Style.Builder().fromUri(getNextStyle()));
  }

  private void addBusStop() {
    mapboxMap.getStyle().addLayer(layer);
    mapboxMap.getStyle().addSource(source);
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
