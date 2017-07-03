package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
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

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.addOnMapChangedListener(new MapView.OnMapChangedListener() {
      @Override
      public void onMapChanged(int change) {
        Timber.e("OnMapChange: %s", resolveMapChange(change));
      }
    });

    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(MapboxMap map) {
        mapboxMap = map;
        mapboxMap.animateCamera(CameraUpdateFactory.newLatLngZoom(
          new LatLng(55.754020, 37.620948), 12), 9000);
      }
    });
  }

  private String resolveMapChange(int change) {
    switch (change) {
      case MapView.REGION_WILL_CHANGE:
        return "Region will change";
      case MapView.REGION_WILL_CHANGE_ANIMATED:
        return "Region will change animated";
      case MapView.REGION_IS_CHANGING:
        return "Region is changing";
      case MapView.REGION_DID_CHANGE:
        return "Region did change";
      case MapView.REGION_DID_CHANGE_ANIMATED:
        return "Region did change animated";
      case MapView.WILL_START_LOADING_MAP:
        return "Will start loading map";
      case MapView.DID_FINISH_LOADING_MAP:
        return "Did finish loading map";
      case MapView.DID_FAIL_LOADING_MAP:
        return "Did fail loading map";
      case MapView.WILL_START_RENDERING_FRAME:
        return "Will start rendering frame";
      case MapView.DID_FINISH_RENDERING_FRAME:
        return "Did finish rendering frame";
      case MapView.DID_FINISH_RENDERING_FRAME_FULLY_RENDERED:
        return "Did finish rendering frame fully rendered";
      case MapView.WILL_START_RENDERING_MAP:
        return "Will start rendering map";
      case MapView.DID_FINISH_RENDERING_MAP:
        return "Did finish rendering map";
      case MapView.DID_FINISH_RENDERING_MAP_FULLY_RENDERED:
        return "Did finish rendering map fully rendered";
      case MapView.DID_FINISH_LOADING_STYLE:
        return "Did finish loading style";
      case MapView.SOURCE_DID_CHANGE:
        return "Source did change";
      default:
        throw new RuntimeException("Unsupported map type with int value: " + change);
    }
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
