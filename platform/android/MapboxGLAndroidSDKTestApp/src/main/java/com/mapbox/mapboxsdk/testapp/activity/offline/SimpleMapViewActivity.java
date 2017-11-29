package com.mapbox.mapboxsdk.testapp.activity.offline;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * The most basic example of adding a map to an activity.
 */
public class SimpleMapViewActivity extends AppCompatActivity {

  private static final String TAG = "TEST-OFFLINE";

  public static final String BOUNDS_ARG = "BOUNDS";
  public static final String STYLE_ARG = "STYLE";
  public static final String MINZOOM_ARG = "MINZOOM";
  public static final String MAXZOOM_ARG = "MAXZOOM";

  private MapView mapView;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    Bundle bundle = getIntent().getExtras();
    final String styleUrl = getStyleUrl(bundle, Style.MAPBOX_STREETS);
    final double minZoom = getZoom(bundle, MINZOOM_ARG, 0);
    final double maxZoom = getZoom(bundle, MAXZOOM_ARG, 15);
    final CameraPosition cameraPosition = getCameraPostion(bundle, minZoom, maxZoom);


    Log.d(TAG, " >>>> StyleURl: =" + styleUrl);
    Log.d(TAG, " >>>> Camera pos: =" + cameraPosition);
    Log.d(TAG, " >>>> Min ZOOM: =" + minZoom);
    Log.d(TAG, " >>>> Max ZOOM: =" + maxZoom);

    // configure inital map state
//
//    // create map
//    mapView = new MapView(this, options);

    mapView = new MapView(this);
    mapView.setId(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    setContentView(mapView);

    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(MapboxMap mapboxMap) {

        // correct style
        mapboxMap.setStyle(styleUrl);

        // position map on top of offline region
        mapboxMap.setCameraPosition(cameraPosition);

        // restrict camera movement
        mapboxMap.setMinZoomPreference(minZoom);
        mapboxMap.setMaxZoomPreference(maxZoom);
        // mapboxMap.setLatLngBoundsForCameraTarget(definition.getBounds());
      }
    });

  }


  private CameraPosition getCameraPostion(Bundle bundle, double minZoom, double maxZoom) {
    LatLng cameraPos = null;
    if (bundle != null && bundle.containsKey(BOUNDS_ARG)) {
      LatLngBounds latLngBounds = (LatLngBounds) bundle.getParcelable(BOUNDS_ARG);
      if (latLngBounds != null) {
        cameraPos = latLngBounds.getCenter();
      }
    }
    if (cameraPos == null) {
      cameraPos = new LatLng(45.520486, -122.673541);
    }

    return new CameraPosition.Builder()
      .target(cameraPos)
      .zoom(maxZoom > minZoom ? maxZoom - minZoom / 2 : maxZoom)
      .build();
  }

  private String getStyleUrl(Bundle bundle, String defaultValue) {
    if (bundle != null) {
      return bundle.getString(STYLE_ARG, defaultValue);
    }

    return defaultValue;
  }

  private double getZoom(Bundle bundle, String arg, double defaultValue) {
    if (bundle != null) {
      return bundle.getDouble(arg, defaultValue);
    }

    return defaultValue;
  }

  private MapboxMapOptions getMapBoxOptions(Bundle bundle) {

    double minZoom = getZoom(bundle, MINZOOM_ARG, 0);
    double maxZoom = getZoom(bundle, MAXZOOM_ARG, 15);
    return new MapboxMapOptions()
      .styleUrl(getStyleUrl(bundle, Style.MAPBOX_STREETS))
      .camera(getCameraPostion(bundle, minZoom, maxZoom));
  }



  // Add the mapView lifecycle to the activity's lifecycle methods
  @Override
  public void onResume() {
    super.onResume();
    mapView.onResume();
  }

  @Override
  protected void onStart() {
    super.onStart();
    mapView.onStart();
  }

  @Override
  protected void onStop() {
    super.onStop();
    mapView.onStop();
  }

  @Override
  public void onPause() {
    super.onPause();
    mapView.onPause();
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