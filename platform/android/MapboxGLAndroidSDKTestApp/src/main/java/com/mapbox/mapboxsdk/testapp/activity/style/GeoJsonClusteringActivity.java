package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Color;
import android.graphics.Point;
import android.graphics.PointF;
import android.os.Bundle;
import androidx.core.content.res.ResourcesCompat;
import androidx.appcompat.app.AppCompatActivity;
import android.view.MenuItem;
import android.widget.Toast;
import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonOptions;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.utils.BitmapUtils;
import timber.log.Timber;

import java.net.URI;
import java.net.URISyntaxException;
import java.util.List;
import java.util.Objects;

import static com.mapbox.mapboxsdk.style.expressions.Expression.all;
import static com.mapbox.mapboxsdk.style.expressions.Expression.accumulated;
import static com.mapbox.mapboxsdk.style.expressions.Expression.concat;
import static com.mapbox.mapboxsdk.style.expressions.Expression.division;
import static com.mapbox.mapboxsdk.style.expressions.Expression.exponential;
import static com.mapbox.mapboxsdk.style.expressions.Expression.get;
import static com.mapbox.mapboxsdk.style.expressions.Expression.gt;
import static com.mapbox.mapboxsdk.style.expressions.Expression.gte;
import static com.mapbox.mapboxsdk.style.expressions.Expression.has;
import static com.mapbox.mapboxsdk.style.expressions.Expression.interpolate;
import static com.mapbox.mapboxsdk.style.expressions.Expression.literal;
import static com.mapbox.mapboxsdk.style.expressions.Expression.lt;
import static com.mapbox.mapboxsdk.style.expressions.Expression.max;
import static com.mapbox.mapboxsdk.style.expressions.Expression.neq;
import static com.mapbox.mapboxsdk.style.expressions.Expression.rgb;
import static com.mapbox.mapboxsdk.style.expressions.Expression.stop;
import static com.mapbox.mapboxsdk.style.expressions.Expression.toNumber;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleRadius;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconSize;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textAllowOverlap;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textField;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textIgnorePlacement;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.textSize;

/**
 * Test activity showcasing using a geojson source and visualise that source as a cluster by using filters.
 */
public class GeoJsonClusteringActivity extends AppCompatActivity {

  private static final double CAMERA_ZOOM_DELTA = 0.01;
  private MapView mapView;
  private MapboxMap mapboxMap;

  private GeoJsonSource clusterSource;
  private int clickOptionCounter;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_geojson_clustering);

    // Initialize map as normal
    mapView = findViewById(R.id.mapView);
    // noinspection ConstantConditions
    mapView.onCreate(savedInstanceState);

    mapView.getMapAsync(map -> {
      mapboxMap = map;
      mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(37.7749, 122.4194), 0));

      final int[][] clusterLayers = new int[][] {
        new int[] {150, ResourcesCompat.getColor(getResources(), R.color.redAccent, getTheme())},
        new int[] {20, ResourcesCompat.getColor(getResources(), R.color.greenAccent, getTheme())},
        new int[] {0, ResourcesCompat.getColor(getResources(), R.color.blueAccent, getTheme())}
      };

      try {
        mapboxMap.setStyle(new Style.Builder()
          .fromUri(Style.LIGHT)
          .withSource(clusterSource = createClusterSource())
          .withLayer(createSymbolLayer())
          .withLayer(createClusterLevelLayer(0, clusterLayers))
          .withLayer(createClusterLevelLayer(1, clusterLayers))
          .withLayer(createClusterLevelLayer(2, clusterLayers))
          .withLayer(createClusterTextLayer())
          .withImage("icon-id", Objects.requireNonNull(
            BitmapUtils.getBitmapFromDrawable(getResources().getDrawable(R.drawable.ic_hearing_black_24dp))), true
          )
        );
      } catch (URISyntaxException exception) {
        Timber.e(exception);
      }

      mapboxMap.addOnMapClickListener(latLng -> {
        PointF point = mapboxMap.getProjection().toScreenLocation(latLng);
        List<Feature> features = mapboxMap.queryRenderedFeatures(point, "cluster-0", "cluster-1", "cluster-2");
        if (!features.isEmpty()) {
          onClusterClick(features.get(0), new Point((int) point.x, (int) point.y));
        }
        return true;
      });
    });

    findViewById(R.id.fab).setOnClickListener(v -> {
      updateClickOptionCounter();
      notifyClickOptionUpdate();
    });
  }

  private void onClusterClick(Feature cluster, Point clickPoint) {
    if (clickOptionCounter == 0) {
      double nextZoomLevel = clusterSource.getClusterExpansionZoom(cluster);
      double zoomDelta = nextZoomLevel - mapboxMap.getCameraPosition().zoom;
      mapboxMap.animateCamera(CameraUpdateFactory.zoomBy(zoomDelta + CAMERA_ZOOM_DELTA, clickPoint));
      Toast.makeText(this, "Zooming to " + nextZoomLevel, Toast.LENGTH_SHORT).show();
    } else if (clickOptionCounter == 1) {
      FeatureCollection collection = clusterSource.getClusterChildren(cluster);
      Toast.makeText(this, "Children: " + collection.toJson(), Toast.LENGTH_SHORT).show();
    } else {
      FeatureCollection collection = clusterSource.getClusterLeaves(cluster, 2, 1);
      Toast.makeText(this, "Leaves: " + collection.toJson(), Toast.LENGTH_SHORT).show();
    }
  }

  private GeoJsonSource createClusterSource() throws URISyntaxException {
    return new GeoJsonSource("earthquakes", new URI("asset://earthquakes.geojson"), new GeoJsonOptions()
      .withCluster(true)
      .withClusterMaxZoom(14)
      .withClusterRadius(50)
      .withClusterProperty("max", max(accumulated(), get("max")), get("mag"))
      .withClusterProperty("sum", literal("+"), get("mag"))
      .withClusterProperty("felt", literal("any"), neq(get("felt"), literal("null")))
    );
  }

  private SymbolLayer createSymbolLayer() {
    return new SymbolLayer("unclustered-points", "earthquakes")
      .withProperties(
        iconImage("icon-id"),
        iconSize(
          division(
            get("mag"), literal(4.0f)
          )
        ),
        iconColor(
          interpolate(exponential(1), get("mag"),
            stop(2.0, rgb(0, 255, 0)),
            stop(4.5, rgb(0, 0, 255)),
            stop(7.0, rgb(255, 0, 0))
          )
        )
      )
      .withFilter(has("mag"));
  }

  private CircleLayer createClusterLevelLayer(int level, int[][] layerColors) {
    CircleLayer circles = new CircleLayer("cluster-" + level, "earthquakes");
    circles.setProperties(
      circleColor(layerColors[level][1]),
      circleRadius(18f)
    );

    Expression pointCount = toNumber(get("point_count"));
    circles.setFilter(
      level == 0
        ? all(has("point_count"),
        gte(pointCount, literal(layerColors[level][0]))
      ) : all(has("point_count"),
        gt(pointCount, literal(layerColors[level][0])),
        lt(pointCount, literal(layerColors[level - 1][0]))
      )
    );
    return circles;
  }

  private SymbolLayer createClusterTextLayer() {
    return new SymbolLayer("property", "earthquakes")
      .withProperties(
        textField(concat(get("point_count"), literal(", "), get("max"))),
        textSize(12f),
        textColor(Color.WHITE),
        textIgnorePlacement(true),
        textAllowOverlap(true)
      );
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

  private void updateClickOptionCounter() {
    if (clickOptionCounter == 2) {
      clickOptionCounter = 0;
    } else {
      clickOptionCounter++;
    }
  }

  private void notifyClickOptionUpdate() {
    if (clickOptionCounter == 0) {
      Toast.makeText(
        GeoJsonClusteringActivity.this,
        "Clicking a cluster will zoom to the level where it dissolves",
        Toast.LENGTH_SHORT).show();
    } else if (clickOptionCounter == 1) {
      Toast.makeText(
        GeoJsonClusteringActivity.this,
        "Clicking a cluster will show the details of the cluster children",
        Toast.LENGTH_SHORT).show();
    } else {
      Toast.makeText(
        GeoJsonClusteringActivity.this,
        "Clicking a cluster will show the details of the cluster leaves with an offset and limit",
        Toast.LENGTH_SHORT).show();
    }
  }
}
