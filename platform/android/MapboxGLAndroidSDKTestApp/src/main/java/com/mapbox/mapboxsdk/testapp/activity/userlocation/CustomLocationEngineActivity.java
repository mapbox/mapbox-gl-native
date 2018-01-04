package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.view.Menu;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing using a custom location engine.
 */
public class CustomLocationEngineActivity extends BaseLocationActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;
  private FloatingActionButton locationToggleFab;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_custom_location_engine);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;
      mapboxMap.setLocationSource(MockLocationEngine.getInstance());
    });

    locationToggleFab = (FloatingActionButton) findViewById(R.id.fabLocationToggle);
    locationToggleFab.setOnClickListener(view -> {
      if (mapboxMap != null) {
        enableLocation(!mapboxMap.isMyLocationEnabled());
      }
    });
  }

  @Override
  protected void enableLocation(boolean enabled) {
    mapboxMap.setMyLocationEnabled(enabled);
    if (enabled) {
      locationToggleFab.setImageResource(R.drawable.ic_location_disabled);
    } else {
      locationToggleFab.setImageResource(R.drawable.ic_my_location);
    }
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_location_engine, menu);
    return super.onCreateOptionsMenu(menu);
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    if (mapboxMap != null) {
      int itemId = item.getItemId();
      if (itemId == R.id.action_id_location_source_lost) {
        mapboxMap.setLocationSource(Mapbox.getLocationEngine());
        return true;
      } else if (itemId == R.id.action_id_location_source_mock) {
        mapboxMap.setLocationSource(MockLocationEngine.getInstance());
        return true;
      } else if (itemId == R.id.action_id_location_source_null) {
        mapboxMap.setLocationSource(null);
        return true;
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
