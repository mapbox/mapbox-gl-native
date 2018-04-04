package com.mapbox.mapboxsdk.testapp.activity.feature;

import android.graphics.Color;
import android.graphics.RectF;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.Toast;

import com.google.gson.JsonObject;
import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.LineString;
import com.mapbox.geojson.Point;
import com.mapbox.geojson.Polygon;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.layers.CircleLayer;
import com.mapbox.mapboxsdk.style.layers.FillLayer;
import com.mapbox.mapboxsdk.style.layers.LineLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.maps.MapView.DID_FINISH_RENDERING_MAP_FULLY_RENDERED;
import static com.mapbox.mapboxsdk.style.expressions.Expression.eq;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.circleColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillOpacity;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.lineColor;

/**
 * Test activity for GeoJSON round-trips
 */
public class QueryRenderedFeaturesLimitsActivity extends AppCompatActivity {

  public static final String GEOMETRY_SOURCE = "geometry-source";
  public MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_query_limits);

    // Initialize map as normal
    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(mapboxMap -> {
      QueryRenderedFeaturesLimitsActivity.this.mapboxMap = mapboxMap;
      int multiplier = 2;
      mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(9 - multiplier));

      // Start a background test task
      new TestTask(mapboxMap, multiplier).execute();

      // Query
      mapView.addOnMapChangedListener(change -> {
        if (change == DID_FINISH_RENDERING_MAP_FULLY_RENDERED) {
          GeoJsonSource source = mapboxMap.getSourceAs(GEOMETRY_SOURCE);
          if (source != null) {
            List<Feature> result = source.querySourceFeatures(null);
            dumpQueryResult(result);
          }
        }
      });

      mapboxMap.addOnMapClickListener(point -> {
        GeoJsonSource source = mapboxMap.getSourceAs(GEOMETRY_SOURCE);
        if (source != null) {
          int top = mapView.getTop();
          int left = mapView.getLeft();
          RectF box = new RectF(left, top, left + mapView.getWidth(), top + mapView.getHeight());
          List<Feature> result = mapboxMap.queryRenderedFeatures(box);
          dumpQueryResult(result);
        }
      });
    });
  }

  private void dumpQueryResult(List<Feature> result) {
    Timber.i("Query resulted in %s features", result.size());
    Toast.makeText(this, String.format("Query resulted in %s features", result.size()), Toast.LENGTH_SHORT).show();
    int points = 0;
    int properties = 0;
    for (Feature f : result) {
      if (f.geometry() instanceof Point) {
        points++;
      } else if (f.geometry() instanceof LineString) {
        points += ((LineString)f.geometry()).coordinates().size();
      } else if (f.geometry() instanceof Polygon) {
        points += ((Polygon)f.geometry()).coordinates().get(0).size();
      }
      properties += f.properties().size();
    }
    Timber.i("Converted points: %s, properties: %s", points, properties);
  }

  private static class TestTask extends AsyncTask<Void, Void, FeatureCollection> {
    private MapboxMap map;
    private int multiplier;

    TestTask(MapboxMap map, int multiplier) {
      this.map = map;
      this.multiplier = multiplier;
    }

    @Override
    protected void onPreExecute() {
    }

    @Override
    protected FeatureCollection doInBackground(Void... voids) {
      List<Feature> features = new ArrayList<>();

      // Create a bunch of points
      JsonObject properties = properties(100);
      properties.addProperty("type", "point");
      int maxX = 10 * multiplier;
      for (int x = 1; x < maxX; x++) {
        int maxY = 10 * multiplier;
        for (int y = 1; y < maxY; y++) {
          features.add(
            Feature.fromGeometry(
              Point.fromLngLat(((x - maxX / 2) / (maxX * 1.0)), ((y - maxY / 2) / (maxY * 1.0))),
              properties,
              String.format("point-%s-%s", x, y)
            )
          );
        }
      }

      // Create a bunch of lineStrings
      properties = properties(100);
      properties.addProperty("type", "line");
      maxX = 10 * multiplier;
      for (int x = 1; x < maxX; x++) {
        int maxY = 10 * multiplier;
        List<Point> points = new ArrayList<>();
        for (int y = 1; y < maxY; y++) {
          points.add(Point.fromLngLat(((x - maxX / 2) / (maxX * 1.0)), ((y - maxY / 2) / (maxY * 1.0))));
        }
        features.add(Feature.fromGeometry(
          LineString.fromLngLats(points),
          properties));
      }

      // Create a bunch of fills
      properties = properties(100);
      properties.addProperty("type", "fill");
      maxX = 10 * multiplier;
      for (int x = 1; x < maxX; x++) {
        int maxY = 10 * multiplier;
        for (int y = 1; y < maxY; y++) {
          List<Point> points = new ArrayList<>();
          double center_x = ((x - maxX / 2) / (maxX * 1.0));
          double center_y = ((y - maxY / 2) / (maxY * 1.0));
          double offset = 0.02;
          points.add(Point.fromLngLat(center_x - offset, center_y - offset));
          points.add(Point.fromLngLat(center_x + offset, center_y - offset));
          points.add(Point.fromLngLat(center_x + offset, center_y + offset));
          points.add(Point.fromLngLat(center_x - offset, center_y + offset));
          points.add(Point.fromLngLat(center_x - offset, center_y - offset));

          features.add(Feature.fromGeometry(
            Polygon.fromLngLats(Collections.singletonList(points)),
            properties));
        }
      }

      // Add one long line string
      properties = properties(100000);
      properties.addProperty("type", "line");
      maxX = 200 * multiplier;
      List<Point> points = new ArrayList<>();
      for (int x = 1; x < maxX; x++) {
        int maxY = 200 * multiplier;
        for (int y = 1; y < maxY; y++) {
          points.add(Point.fromLngLat(((x - maxX / 2) / (maxX * 1.0)), ((y - maxY / 2) / (maxY * 1.0))));
        }
      }
      features.add(Feature.fromGeometry(
        LineString.fromLngLats(points),
        properties));

      return FeatureCollection.fromFeatures(features);
    }

    private JsonObject properties(int num) {
      JsonObject properties = new JsonObject();

      for (int i = 0; i < num / 2; i++) {
        properties.addProperty("double-prop-" + i, 1.828282828d);
      }
      for (int i = 0; i < num / 2; i++) {
        properties.addProperty("string-prop-" + i, "sdfsdf sdfsd fff fasdasd" + i);
      }

      return properties;
    }

    @Override
    protected void onPostExecute(FeatureCollection features) {
      // Add stuff
      GeoJsonSource source = new GeoJsonSource(GEOMETRY_SOURCE, features);
      map.addSource(source);
      map.addLayer(new CircleLayer("circles", source.getId())
        .withFilter(eq("type", "point"))
        .withProperties(circleColor(Color.RED)));
      map.addLayer(new LineLayer("lines", source.getId())
        .withFilter(eq("type", "line"))
        .withProperties(lineColor(Color.GREEN)));
      map.addLayerBelow(new FillLayer("fill", source.getId())
          .withFilter(eq("type", "fill"))
          .withProperties(fillColor(Color.BLUE), fillOpacity(.5f)),
        "circles");
    }
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
