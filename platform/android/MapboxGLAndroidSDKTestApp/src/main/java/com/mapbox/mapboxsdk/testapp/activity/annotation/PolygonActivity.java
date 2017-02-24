package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.PolygonOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;
import java.util.List;

import static com.mapbox.mapboxsdk.testapp.activity.annotation.PolygonActivity.Config.BLUE_COLOR;
import static com.mapbox.mapboxsdk.testapp.activity.annotation.PolygonActivity.Config.BROKEN_SHAPE_POINTS;
import static com.mapbox.mapboxsdk.testapp.activity.annotation.PolygonActivity.Config.FULL_ALPHA;
import static com.mapbox.mapboxsdk.testapp.activity.annotation.PolygonActivity.Config.NO_ALPHA;
import static com.mapbox.mapboxsdk.testapp.activity.annotation.PolygonActivity.Config.PARTIAL_ALPHA;
import static com.mapbox.mapboxsdk.testapp.activity.annotation.PolygonActivity.Config.RED_COLOR;
import static com.mapbox.mapboxsdk.testapp.activity.annotation.PolygonActivity.Config.STAR_SHAPE_POINTS;
import static com.mapbox.mapboxsdk.testapp.activity.annotation.PolygonActivity.Config.STAR_SHAPE_HOLES;

/**
 * Activity to test the Polygon annotation API & programmatically creating a MapView.
 * <p>
 * Showcases changing Polygon features as visibility, alpha, color and points.
 * </p>
 */
public class PolygonActivity extends AppCompatActivity implements OnMapReadyCallback {

  private MapView mapView;
  private MapboxMap mapboxMap;

  private Polygon polygon;
  private boolean fullAlpha = true;
  private boolean visible = true;
  private boolean color = true;
  private boolean allPoints;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    // configure inital map state
    MapboxMapOptions options = new MapboxMapOptions()
      .attributionTintColor(RED_COLOR)
      // deprecated feature!
      .textureMode(true)
      .compassFadesWhenFacingNorth(false)
      .styleUrl(Style.MAPBOX_STREETS)
      .camera(new CameraPosition.Builder()
        .target(new LatLng(45.520486, -122.673541))
        .zoom(12)
        .tilt(40)
        .build());

    // create map
    mapView = new MapView(this, options);
    mapView.setId(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);

    setContentView(mapView);
  }

  @Override
  public void onMapReady(MapboxMap map) {
    mapboxMap = map;
    polygon = mapboxMap.addPolygon(new PolygonOptions()
      .addAll(STAR_SHAPE_POINTS)
      .fillColor(BLUE_COLOR));
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
      case R.id.action_id_alpha:
        fullAlpha = !fullAlpha;
        polygon.setAlpha(fullAlpha ? FULL_ALPHA : PARTIAL_ALPHA);
        return true;
      case R.id.action_id_visible:
        visible = !visible;
        polygon.setAlpha(visible ? (fullAlpha ? FULL_ALPHA : PARTIAL_ALPHA) : NO_ALPHA);
        return true;
      case R.id.action_id_points:
        allPoints = !allPoints;
        polygon.setPoints(allPoints ? STAR_SHAPE_POINTS : BROKEN_SHAPE_POINTS);
        polygon.setHoles(STAR_SHAPE_HOLES);
        return true;
      case R.id.action_id_color:
        color = !color;
        polygon.setFillColor(color ? BLUE_COLOR : RED_COLOR);
        return true;
      default:
        return super.onOptionsItemSelected(item);
    }
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_polygon, menu);
    return true;
  }

  static final class Config {
    static final int BLUE_COLOR = Color.parseColor("#3bb2d0");
    static final int RED_COLOR = Color.parseColor("#AF0000");

    static final float FULL_ALPHA = 1.0f;
    static final float PARTIAL_ALPHA = 0.5f;
    static final float NO_ALPHA = 0.0f;

    static final List<LatLng> STAR_SHAPE_POINTS = new ArrayList<LatLng>() {
      {
        add(new LatLng(45.522585, -122.685699));
        add(new LatLng(45.534611, -122.708873));
        add(new LatLng(45.530883, -122.678833));
        add(new LatLng(45.547115, -122.667503));
        add(new LatLng(45.530643, -122.660121));
        add(new LatLng(45.533529, -122.636260));
        add(new LatLng(45.521743, -122.659091));
        add(new LatLng(45.510677, -122.648792));
        add(new LatLng(45.515008, -122.664070));
        add(new LatLng(45.502496, -122.669048));
        add(new LatLng(45.515369, -122.678489));
        add(new LatLng(45.506346, -122.702007));
        add(new LatLng(45.522585, -122.685699));
      }
    };

    static final List<? extends List<LatLng>> STAR_SHAPE_HOLES = new ArrayList<ArrayList<LatLng>>() {
      {
        add(new ArrayList<LatLng>() {
          {
            add(new LatLng(45.522584, -122.672653));
            add(new LatLng(45.532584, -122.672653));
            add(new LatLng(45.532584, -122.682653));
            add(new LatLng(45.522584, -122.682653));
            add(new LatLng(45.522584, -122.672653));
          }
        });
        add(new ArrayList<LatLng>() {
          {
            add(new LatLng(45.521584, -122.671653));
            add(new LatLng(45.516584, -122.671653));
            add(new LatLng(45.516584, -122.676653));
            add(new LatLng(45.521584, -122.676653));
            add(new LatLng(45.521584, -122.671653));
          }
        });
        add(new ArrayList<LatLng>() {
          {
            add(new LatLng(45.529584, -122.661653));
            add(new LatLng(45.524584, -122.661653));
            add(new LatLng(45.524584, -122.656653));
            add(new LatLng(45.529584, -122.656653));
            add(new LatLng(45.529584, -122.661653));
          }
        });
      }
    };

    static final List<LatLng> BROKEN_SHAPE_POINTS =
      STAR_SHAPE_POINTS.subList(0, STAR_SHAPE_POINTS.size() - 3);
  }
}
