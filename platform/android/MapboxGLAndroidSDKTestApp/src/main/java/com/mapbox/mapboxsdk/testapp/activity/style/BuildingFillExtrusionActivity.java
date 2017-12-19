package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.style.functions.Function;
import com.mapbox.mapboxsdk.style.functions.stops.IdentityStops;
import com.mapbox.mapboxsdk.style.layers.FillExtrusionLayer;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.PropertyFactory;
import com.mapbox.mapboxsdk.style.light.Light;
import com.mapbox.mapboxsdk.style.light.Position;
import com.mapbox.mapboxsdk.testapp.R;

import static com.mapbox.mapboxsdk.style.layers.Filter.eq;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillExtrusionBase;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillExtrusionColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillExtrusionHeight;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillExtrusionOpacity;

/**
 * Test activity showing 3D buildings with a FillExtrusion Layer
 */
public class BuildingFillExtrusionActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;
  private Light light;

  private boolean isMapAnchorLight;
  private boolean isLowIntensityLight;
  private boolean isRedColor;
  private boolean isInitPosition;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_building_layer);
    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;
      setupBuildings();
      setupLight();
    });
  }

  private void setupBuildings() {
    FillExtrusionLayer fillExtrusionLayer = new FillExtrusionLayer("3d-buildings", "composite");
    fillExtrusionLayer.setSourceLayer("building");
    fillExtrusionLayer.setFilter(eq("extrude", "true"));
    fillExtrusionLayer.setMinZoom(15);
    fillExtrusionLayer.setProperties(
      fillExtrusionColor(Color.LTGRAY),
      fillExtrusionHeight(Function.property("height", new IdentityStops<Float>())),
      fillExtrusionBase(Function.property("min_height", new IdentityStops<Float>())),
      fillExtrusionOpacity(0.9f)
    );
    mapboxMap.addLayer(fillExtrusionLayer);
  }

  private void setupLight() {
    light = mapboxMap.getLight();

    findViewById(R.id.fabLightPosition).setOnClickListener(v -> {
      isInitPosition = !isInitPosition;
      if (isInitPosition) {
        light.setPosition(new Position(1.5f, 90, 80));
      } else {
        light.setPosition(new Position(1.15f, 210, 30));
      }
    });

    findViewById(R.id.fabLightColor).setOnClickListener(v -> {
      isRedColor = !isRedColor;
      light.setColor(PropertyFactory.colorToRgbaString(isRedColor ? Color.RED : Color.BLUE));
    });
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_building, menu);
    return true;
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    if (light != null) {
      int id = item.getItemId();
      if (id == R.id.menu_action_anchor) {
        isMapAnchorLight = !isMapAnchorLight;
        light.setAnchor(isMapAnchorLight ? Property.ANCHOR_MAP : Property.ANCHOR_VIEWPORT);
      } else if (id == R.id.menu_action_intensity) {
        isLowIntensityLight = !isLowIntensityLight;
        light.setIntensity(isLowIntensityLight ? 0.35f : 1.0f);
      }
    }
    return super.onOptionsItemSelected(item);
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
