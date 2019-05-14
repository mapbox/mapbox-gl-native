package com.mapbox.mapboxsdk.testapp.activity.camera;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomSheetBehavior;
import android.support.v4.content.res.ResourcesCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.view.LockableBottomSheetBehavior;
import com.mapbox.mapboxsdk.utils.BitmapUtils;

import java.util.ArrayList;
import java.util.List;

import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.iconImage;

/**
 * Test activity showcasing using the LatLngBounds camera API.
 */
public class LatLngBoundsActivity extends AppCompatActivity implements View.OnClickListener {

  private static final List<LatLng> LOCATIONS = new ArrayList<LatLng>() {
    {
      add(new LatLng(37.806866, -122.422502));
      add(new LatLng(37.812905, -122.477605));
      add(new LatLng(37.826944, -122.423188));
      add(new LatLng(37.752676, -122.447736));
      add(new LatLng(37.769305, -122.479322));
      add(new LatLng(37.749834, -122.417867));
      add(new LatLng(37.756149, -122.405679));
      add(new LatLng(37.751403, -122.387397));
      add(new LatLng(37.793064, -122.391517));
      add(new LatLng(37.769122, -122.427394));
    }
  };
  private static final LatLngBounds BOUNDS = new LatLngBounds.Builder().includes(LOCATIONS).build();
  private static final int ANIMATION_DURATION_LONG = 450;
  private static final int ANIMATION_DURATION_SHORT = 250;
  private static final int BOUNDS_PADDING_DIVIDER_SMALL = 3;
  private static final int BOUNDS_PADDING_DIVIDER_LARGE = 9;
  private static final String SYMBOL_SOURCE_ID = "source=id";
  private static final String SYMBOL_LAYER_ID = "layer=id";
  private static final String ICON_ID = "marker";

  private MapView mapView;
  private MapboxMap mapboxMap;
  private View bottomSheet;
  private LockableBottomSheetBehavior bottomSheetBehavior;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_latlngbounds);
    initBottomSheet();
    initMapView(savedInstanceState);
  }

  private void initMapView(Bundle savedInstanceState) {
    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);

    mapView.addOnStyleImageMissingListener(id -> {
      if (mapboxMap != null && id.equals(ICON_ID)) {
        mapboxMap.getStyle(style -> {
          Bitmap marker = BitmapUtils.getBitmapFromDrawable(
            ResourcesCompat.getDrawable(getResources(), R.drawable.mapbox_marker_icon, getTheme())
          );
          assert marker != null;
          style.addImage(id, marker);
        });
      }
    });
    mapView.getMapAsync(map -> {
      mapboxMap = map;
      disableGestures();
      moveToBounds(bottomSheet.getMeasuredHeight(), BOUNDS_PADDING_DIVIDER_SMALL, ANIMATION_DURATION_SHORT);
      loadStyle();
    });
  }

  private void loadStyle() {
    mapboxMap.setStyle(new Style.Builder().fromUrl(Style.MAPBOX_STREETS), style -> {
      addMarkers();
      initFab();
    });
  }

  private void disableGestures() {
    mapboxMap.getUiSettings().setTiltGesturesEnabled(false);
    mapboxMap.getUiSettings().setRotateGesturesEnabled(false);
  }

  private void addMarkers() {
    final List<Feature> features = new ArrayList<>();
    for (LatLng location : LOCATIONS) {
      features.add(Feature.fromGeometry(Point.fromLngLat(location.getLongitude(), location.getLatitude())));
    }

    mapboxMap.getStyle(style -> {
      style.addSource(new GeoJsonSource(SYMBOL_SOURCE_ID, FeatureCollection.fromFeatures(features)));
      style.addLayer(new SymbolLayer(SYMBOL_LAYER_ID, SYMBOL_SOURCE_ID).withProperties(iconImage("marker")));
    });
  }

  private void initFab() {
    findViewById(R.id.fab).setOnClickListener(this);
  }

  @Override
  public void onClick(View v) {
    bottomSheetBehavior.setState(BottomSheetBehavior.STATE_HIDDEN);
    v.animate().alpha(0.0f).setDuration(ANIMATION_DURATION_SHORT);
  }

  private void initBottomSheet() {
    bottomSheet = findViewById(R.id.bottom_sheet);
    bottomSheetBehavior = (LockableBottomSheetBehavior) BottomSheetBehavior.from(bottomSheet);
    bottomSheetBehavior.setLocked(true);
    bottomSheetBehavior.setBottomSheetCallback(new BottomSheetBehavior.BottomSheetCallback() {
      @Override
      public void onStateChanged(@NonNull View bottomSheet, int newState) {
        if (newState == BottomSheetBehavior.STATE_SETTLING && mapboxMap != null) {
          moveToBounds(0, BOUNDS_PADDING_DIVIDER_LARGE, ANIMATION_DURATION_LONG);
        }
      }

      @Override
      public void onSlide(@NonNull View bottomSheet, float slideOffset) {

      }
    });
  }

  private void moveToBounds(int verticalOffset, int boundsPaddingDivider, int duration) {
    int paddingHorizontal = mapView.getMeasuredWidth() / boundsPaddingDivider;
    int paddingVertical = (mapView.getMeasuredHeight() - verticalOffset) / boundsPaddingDivider;
    mapboxMap.animateCamera(CameraUpdateFactory.newLatLngBounds(
      BOUNDS,
      paddingHorizontal,
      paddingVertical,
      paddingHorizontal,
      paddingVertical + verticalOffset),
      duration
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
