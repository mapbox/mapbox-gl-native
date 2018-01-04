package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.os.Bundle;
import android.support.v4.util.LongSparseArray;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;

import timber.log.Timber;

/**
 * Test activity showcasing how to listen to map change events.
 */
public class MapChangeActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_simple);

    final LongSparseArray<String> mapChangeMap = buildMapChangeStringValueSparseArray();
    mapView = (MapView) findViewById(R.id.mapView);
    mapView.addOnMapChangedListener(change -> Timber.e("OnMapChange: %s, %s", change, mapChangeMap.get(change)));

    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;
      mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(
        new LatLng(55.754020, 37.620948), 12), 9000);
    });
  }

  private LongSparseArray<String> buildMapChangeStringValueSparseArray() {
    LongSparseArray<String> mapChangeArray = new LongSparseArray<>();
    mapChangeArray.put(MapView.REGION_WILL_CHANGE, "Region will change");
    mapChangeArray.put(MapView.REGION_WILL_CHANGE_ANIMATED, "Region will change animated");
    mapChangeArray.put(MapView.REGION_IS_CHANGING, "Region is changing");
    mapChangeArray.put(MapView.REGION_DID_CHANGE, "Region did change");
    mapChangeArray.put(MapView.REGION_DID_CHANGE_ANIMATED, "Region did change animated");
    mapChangeArray.put(MapView.WILL_START_LOADING_MAP, "Will start loading map");
    mapChangeArray.put(MapView.DID_FINISH_LOADING_MAP, "Did finish loading map");
    mapChangeArray.put(MapView.DID_FAIL_LOADING_MAP, "Did fail loading map");
    mapChangeArray.put(MapView.WILL_START_RENDERING_FRAME, "Will start rendering frame");
    mapChangeArray.put(MapView.DID_FINISH_RENDERING_FRAME, "Did finish rendering frame");
    mapChangeArray.put(MapView.DID_FINISH_RENDERING_FRAME_FULLY_RENDERED, "Did finish rendering frame fully rendered");
    mapChangeArray.put(MapView.WILL_START_RENDERING_MAP, "Will start rendering map");
    mapChangeArray.put(MapView.DID_FINISH_RENDERING_MAP, "Did finish rendering map");
    mapChangeArray.put(MapView.DID_FINISH_RENDERING_MAP_FULLY_RENDERED, "Did finish rendering map fully rendered");
    mapChangeArray.put(MapView.DID_FINISH_LOADING_STYLE, "Did finish loading style");
    mapChangeArray.put(MapView.SOURCE_DID_CHANGE, "Source did change");
    return mapChangeArray;
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
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }
}
