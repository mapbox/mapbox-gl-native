package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import android.widget.Toast;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.utils.MapFragmentUtils;

/**
 * Test activity showcasing having 2 maps on top of each other.
 * <p>
 * The small map is using the `mapbox_enableZMediaOverlay="true"` configuration
 * </p>
 */
public class DoubleMapActivity extends AppCompatActivity {

  private static final String TAG_FRAGMENT = "map";
  private static final LatLng MACHU_PICCHU = new LatLng(-13.1650709, -72.5447154);
  private static final double ZOOM_IN = 12;
  private static final double ZOOM_OUT = 4;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_fragment);

    if (savedInstanceState == null) {
      MapboxMapOptions options = new MapboxMapOptions();
      options.styleUrl(Style.DARK);
      options.camera(new CameraPosition.Builder()
        .target(MACHU_PICCHU)
        .zoom(ZOOM_IN)
        .build()
      );

      DoubleMapFragment doubleMapFragment = new DoubleMapFragment();
      doubleMapFragment.setArguments(MapFragmentUtils.createFragmentArgs(options));

      FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
      transaction.add(R.id.fragment_container, doubleMapFragment, TAG_FRAGMENT);
      transaction.commit();
    }
  }

  /**
   * Custom fragment containing 2 MapViews.
   */
  public static class DoubleMapFragment extends Fragment {

    private MapView mapView;
    private MapView mapViewMini;

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
      return inflater.inflate(R.layout.fragment_double_map, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
      super.onViewCreated(view, savedInstanceState);

      // MapView large
      mapView = new MapView(view.getContext(), MapFragmentUtils.resolveArgs(view.getContext(), getArguments()));
      mapView.onCreate(savedInstanceState);
      ((ViewGroup) view.findViewById(R.id.container)).addView(mapView, 0);

      // MapView mini
      mapViewMini = view.findViewById(R.id.mini_map);
      mapViewMini.onCreate(savedInstanceState);
      mapViewMini.setStyleUrl(Style.LIGHT);
      mapViewMini.getMapAsync(mapboxMap -> {
        mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(
          new CameraPosition.Builder().target(MACHU_PICCHU)
            .zoom(ZOOM_OUT)
            .build()
          )
        );

        UiSettings uiSettings = mapboxMap.getUiSettings();
        uiSettings.setAllGesturesEnabled(false);
        uiSettings.setCompassEnabled(false);
        uiSettings.setAttributionEnabled(false);
        uiSettings.setLogoEnabled(false);

        mapboxMap.setOnMapClickListener(point -> {
          // test if we can open 2 activities after each other
          Toast.makeText(mapViewMini.getContext(), "Creating a new Activity instance",Toast.LENGTH_SHORT).show();
          startActivity(new Intent(mapViewMini.getContext(), DoubleMapActivity.class));
        });
      });
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
    public void onDestroyView() {
      super.onDestroyView();
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
