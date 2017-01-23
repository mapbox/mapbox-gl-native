package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.TrackingSettings;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.testapp.R;

public class DoubleMapActivity extends AppCompatActivity {

  private static final String TAG_FRAGMENT = "map";

  // used for ui tests
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_fragment);

    if (savedInstanceState == null) {
      FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
      transaction.add(R.id.fragment_container, new DoubleMapFragment(), TAG_FRAGMENT);
      transaction.commit();
    }
  }

  public void setMapboxMap(MapboxMap map) {
    // we need to set mapboxmap on the parent activity,
    // for auto-generated ui tests

    mapboxMap = map;
    mapboxMap.setStyleUrl(Style.DARK);
    mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(18));
    try {
      TrackingSettings settings = mapboxMap.getTrackingSettings();
      settings.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW);
    } catch (SecurityException securityException) {
      // permission is handled in MainActivity
      finish();
    }
  }

  public static class DoubleMapFragment extends Fragment {

    private DoubleMapActivity activity;
    private MapView mapView;
    private MapView mapViewMini;

    @Override
    public void onAttach(Context context) {
      super.onAttach(context);
      activity = (DoubleMapActivity) context;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
      return inflater.inflate(R.layout.fragment_double_map, container, false);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
      super.onViewCreated(view, savedInstanceState);

      // MapView large
      mapView = (MapView) view.findViewById(R.id.mapView);
      mapView.onCreate(savedInstanceState);
      mapView.getMapAsync(new OnMapReadyCallback() {
        @Override
        public void onMapReady(@NonNull MapboxMap mapboxMap) {
          if (activity != null) {
            activity.setMapboxMap(mapboxMap);
          }
        }
      });

      // MapView mini
      mapViewMini = (MapView) view.findViewById(R.id.mini_map);
      mapViewMini.onCreate(savedInstanceState);
      mapViewMini.getMapAsync(new OnMapReadyCallback() {
        @Override
        public void onMapReady(@NonNull MapboxMap mapboxMap) {
          mapboxMap.setStyleUrl(Style.LIGHT);
          mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(4));

          UiSettings uiSettings = mapboxMap.getUiSettings();
          uiSettings.setAllGesturesEnabled(false);
          uiSettings.setCompassEnabled(false);
          uiSettings.setAttributionEnabled(false);
          uiSettings.setLogoEnabled(false);

          try {
            TrackingSettings settings = mapboxMap.getTrackingSettings();
            settings.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW);
          } catch (SecurityException securityException) {
            // permission is handled in MainActivity
            getActivity().finish();
          }

          mapboxMap.setOnMapClickListener(new MapboxMap.OnMapClickListener() {
            @Override
            public void onMapClick(@NonNull LatLng point) {
              // test if we can open 2 activities after each other
              startActivity(new Intent(mapViewMini.getContext(), DoubleMapActivity.class));
            }
          });
        }
      });

      SurfaceView surfaceViewMini = (SurfaceView) mapViewMini.findViewById(R.id.surfaceView);
      surfaceViewMini.setZOrderMediaOverlay(true);
    }

    @Override
    public void onResume() {
      super.onResume();
      mapView.onResume();
      mapViewMini.onResume();
    }

    @Override
    public void onStart() {
      super.onStart();
      mapView.onStart();
      mapViewMini.onStart();
    }

    @Override
    public void onPause() {
      super.onPause();
      mapView.onPause();
      mapViewMini.onPause();
    }

    @Override
    public void onStop() {
      super.onStop();
      mapView.onStop();
      mapViewMini.onStop();
    }

    @Override
    public void onDestroy() {
      super.onDestroy();
      mapView.onDestroy();
      mapViewMini.onDestroy();
    }

    @Override
    public void onLowMemory() {
      super.onLowMemory();
      mapView.onLowMemory();
      mapViewMini.onLowMemory();
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
      super.onSaveInstanceState(outState);
      mapView.onSaveInstanceState(outState);
      mapViewMini.onSaveInstanceState(outState);
    }
  }
}