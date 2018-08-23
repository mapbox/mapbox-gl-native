package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Color;
import android.graphics.PointF;
import android.os.Bundle;
import android.support.v4.content.res.ResourcesCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.MenuItem;
import com.mapbox.geojson.Feature;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonOptions;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.utils.BitmapUtils;
import timber.log.Timber;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.List;

import static com.mapbox.mapboxsdk.style.expressions.Expression.all;
import static com.mapbox.mapboxsdk.style.expressions.Expression.exponential;
import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.expressions.Expression.gt;
import static com.mapbox.mapboxsdk.style.expressions.Expression.gte;
import static com.mapbox.mapboxsdk.style.expressions.Expression.has;
import static com.mapbox.mapboxsdk.style.expressions.Expression.interpolate;
import static com.mapbox.mapboxsdk.style.expressions.Expression.literal;
import static com.mapbox.mapboxsdk.style.expressions.Expression.lt;
import static com.mapbox.mapboxsdk.style.expressions.Expression.rgb;
import static com.mapbox.mapboxsdk.style.expressions.Expression.stop;
import static com.mapbox.mapboxsdk.style.expressions.Expression.toNumber;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleRadius;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconIgnorePlacement;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textField;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textIgnorePlacement;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textSize;

/**
 * Test activity showcasing using a geojson source and visualise that source as a cluster by using filters.
 */
public class GeoJsonClusteringActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;
  private GeoJsonSource source;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_geojson_clustering);

    // Initialize map as normal
    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;
      mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(37.7749, 122.4194), 0));
      mapboxMap.addImage(
        "icon-id",
        BitmapUtils.getBitmapFromDrawable(getResources().getDrawable(R.drawable.ic_hearing_black_24dp)),
        true
      );
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
      mapboxMap.addSource(source =
        new GeoJsonSource("earthquakes",
          new URL("https://www.mapbox.com/mapbox-gl-js/assets/earthquakes.geojson"),
          new GeoJsonOptions()
            .withCluster(true)
            .withClusterMaxZoom(14)
            .withClusterRadius(50)
        )
      );
    } catch (MalformedURLException malformedUrlException) {
      Timber.e(malformedUrlException, "That's not an url... ");
    }

    // Add unclustered layer
    int[][] layers = new int[][] {
      new int[] {150, ResourcesCompat.getColor(getResources(), R.color.redAccent, getTheme())},
      new int[] {20, ResourcesCompat.getColor(getResources(), R.color.greenAccent, getTheme())},
      new int[] {0, ResourcesCompat.getColor(getResources(), R.color.blueAccent, getTheme())}
    };

    SymbolLayer unclustered = new SymbolLayer("unclustered-points", "earthquakes");
    unclustered.setProperties(
      iconImage("icon-id"),
      iconAllowOverlap(true),
      iconIgnorePlacement(true),
      iconColor(
        interpolate(exponential(1), get("mag"),
          stop(2.0, rgb(0, 255, 0)),
          stop(4.5, rgb(0, 0, 255)),
          stop(7.0, rgb(255, 0, 0))
        )
      )
    );

    mapboxMap.addLayer(unclustered);


    String[] layerIds = new String[layers.length];

    for (int i = 0; i < layers.length; i++) {
      // Add some nice circles
      layerIds[i] = "cluster-" + i;
      CircleLayer circles = new CircleLayer(layerIds[i], "earthquakes");
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
      mapboxMap.addLayer(circles);
    }

    // Add the count labels
    SymbolLayer count = new SymbolLayer("count", "earthquakes");
    count.setProperties(
      textField(Expression.toString(get("point_count"))),
      textSize(12f),
      textColor(Color.WHITE),
      textIgnorePlacement(true),
      textAllowOverlap(true)
    );
    mapboxMap.addLayer(count);


    mapboxMap.addOnMapClickListener(latLng -> {
      PointF point = mapboxMap.getProjection().toScreenLocation(latLng);
      List<Feature> features = mapboxMap.queryRenderedFeatures(point, layerIds);
      if (!features.isEmpty()) {
        moveCameraToClusterExpansion(features.get(0), latLng);
        recursiveLoopClusterFeatures(features);
      }
    });

    // Zoom out to start
    mapboxMap.animateCamera(CameraUpdateFactory.zoomTo(1));
  }

  private void moveCameraToClusterExpansion(Feature feature, LatLng latLng) {
    if (feature.hasProperty("cluster") && feature.getBooleanProperty("cluster")) {
      double newZoom = source.getClusterExpansionZoom((long) feature.getNumberProperty("cluster_id"));
      mapboxMap.animateCamera(
        CameraUpdateFactory.newLatLngZoom(latLng, newZoom + 0.01), 750
      );
    }
  }

  private void recursiveLoopClusterFeatures(List<Feature> features) {
    for (Feature feature : features) {
      boolean cluster = feature.hasProperty("cluster") && feature.getBooleanProperty("cluster");
      if (cluster) {
        long pointCount = (long) feature.getNumberProperty("point_count");
        long clusterId = (long) feature.getNumberProperty("cluster_id");
        double expansionZoom = source.getClusterExpansionZoom(clusterId);
        Timber.e(
          "Cluster= (id=%s) with %s points, cluster will expand at zoom %s",
          clusterId, pointCount, expansionZoom
        );
        recursiveLoopClusterFeatures(source.getLeaves(clusterId, 10, 0));
      } else {
        Timber.e("Point data: %s", feature.toJson());
      }
    }
  }
}
