package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Color;
import android.os.Bundle;
import android.support.v4.content.res.ResourcesCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonOptions;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;

import java.net.MalformedURLException;
import java.net.URL;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.layers.Filter.all;
import static com.mapbox.mapboxsdk.style.layers.Filter.gte;
import static com.mapbox.mapboxsdk.style.layers.Filter.lt;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleRadius;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textField;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textSize;

/**
 * Test activity showcasing using a geojson source and visualise that source as a cluster by using filters.
 */
public class GeoJsonClusteringActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_geojson_clustering);

    // Initialize map as normal
    mapView = (MapView) findViewById(R.id.mapView);
    // noinspection ConstantConditions
    mapView.onCreate(savedInstanceState);

    mapView.getMapAsync(map -> {
      mapboxMap = map;
      mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(37.7749, 122.4194), 0));

      // Add a clustered source with some layers
      addClusteredGeoJsonSource();
    });
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
      case android.R.id.home:
        onBackPressed();
        return true;
      default:
        return super.onOptionsItemSelected(item);
    }
  }

  private void addClusteredGeoJsonSource() {
    // Add a clustered source
    try {
      mapboxMap.addSource(
        new GeoJsonSource("earthquakes",
          new URL("https://www.mapbox.com/mapbox-gl-js/assets/earthquakes.geojson"),
          new GeoJsonOptions()
            .withCluster(true)
            .withClusterMaxZoom(14)
            .withClusterRadius(50)
        )
      );
    } catch (MalformedURLException malformedUrlException) {
      Timber.e(malformedUrlException,"That's not an url... ");
    }

    // Add unclustered layer
    int[][] layers = new int[][] {
      new int[] {150, ResourcesCompat.getColor(getResources(), R.color.redAccent, getTheme())},
      new int[] {20, ResourcesCompat.getColor(getResources(), R.color.greenAccent, getTheme())},
      new int[] {0, ResourcesCompat.getColor(getResources(), R.color.blueAccent, getTheme())}
    };

    SymbolLayer unclustered = new SymbolLayer("unclustered-points", "earthquakes");
    unclustered.setProperties(iconImage("marker-15"));
    mapboxMap.addLayer(unclustered);

    for (int i = 0; i < layers.length; i++) {
      // Add some nice circles
      CircleLayer circles = new CircleLayer("cluster-" + i, "earthquakes");
      circles.setProperties(
        circleColor(layers[i][1]),
        circleRadius(18f)
      );
      circles.setFilter(
        i == 0
          ? gte("point_count", layers[i][0]) :
          all(gte("point_count", layers[i][0]), lt("point_count", layers[i - 1][0]))
      );
      mapboxMap.addLayer(circles);
    }

    // Add the count labels
    SymbolLayer count = new SymbolLayer("count", "earthquakes");
    count.setProperties(
      textField("{point_count}"),
      textSize(12f),
      textColor(Color.WHITE)
    );
    mapboxMap.addLayer(count);


    // Zoom out to start
    mapboxMap.animateCamera(CameraUpdateFactory.zoomTo(1));
  }
}
