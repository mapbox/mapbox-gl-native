package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.content.Context;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.design.widget.BottomSheetBehavior;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.utils.MapFragmentUtils;

/**
 * Test activity showcasing using a bottomView with a MapView and stacking map fragments below.
 */
public class BottomSheetActivity extends AppCompatActivity {

  private static final String TAG_MAIN_FRAGMENT = "com.mapbox.mapboxsdk.fragment.tag.main";
  private static final String TAG_BOTTOM_FRAGMENT = "com.mapbox.mapboxsdk.fragment.tag.bottom";

  private boolean bottomSheetFragmentAdded;

  @Override
  protected void onCreate(@Nullable Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_bottom_sheet);

    ActionBar actionBar = getSupportActionBar();
    if (actionBar != null) {
      actionBar.setDisplayHomeAsUpEnabled(true);
    }

    findViewById(R.id.fabFragment).setOnClickListener(v -> addMapFragment());

    findViewById(R.id.fabBottomSheet).setOnClickListener(v -> toggleBottomSheetMapFragment());

    BottomSheetBehavior bottomSheetBehavior = BottomSheetBehavior.from(findViewById(R.id.bottom_sheet));
    bottomSheetBehavior.setPeekHeight((int) (64 * getResources().getDisplayMetrics().density));
    bottomSheetBehavior.setState(BottomSheetBehavior.STATE_COLLAPSED);
    toggleBottomSheetMapFragment();
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    if (item.getItemId() == android.R.id.home) {
      onBackPressed();
    }
    return super.onOptionsItemSelected(item);
  }

  @Override
  public void onBackPressed() {
    FragmentManager fragmentManager = getSupportFragmentManager();

    if (fragmentManager.getBackStackEntryCount() > 0) {
      fragmentManager.popBackStack();
    } else {
      super.onBackPressed();
    }
  }

  private void addMapFragment() {
    FragmentManager fragmentManager = getSupportFragmentManager();
    int fragmentCount = fragmentManager.getBackStackEntryCount();

    FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
    MainMapFragment mainMapFragment = MainMapFragment.newInstance(fragmentCount);
    if (fragmentCount == 0) {
      fragmentTransaction.add(R.id.fragment_container, mainMapFragment, TAG_MAIN_FRAGMENT);
    } else {
      fragmentTransaction.replace(R.id.fragment_container, mainMapFragment, TAG_MAIN_FRAGMENT);
    }
    fragmentTransaction.addToBackStack(String.valueOf(mainMapFragment.hashCode()));
    fragmentTransaction.commit();
    Toast.makeText(this, "Amount of main map fragments: " + (fragmentCount + 1), Toast.LENGTH_SHORT).show();
  }

  private void toggleBottomSheetMapFragment() {
    if (!bottomSheetFragmentAdded) {
      addBottomSheetMapFragment();
    } else {
      removeBottomSheetFragment();
    }
    bottomSheetFragmentAdded = !bottomSheetFragmentAdded;
  }

  private void addBottomSheetMapFragment() {
    FragmentTransaction fragmentTransaction = getSupportFragmentManager().beginTransaction();
    fragmentTransaction.add(R.id.fragment_container_bottom, BottomSheetFragment.newInstance(), TAG_BOTTOM_FRAGMENT);
    fragmentTransaction.commit();
  }

  private void removeBottomSheetFragment() {
    Fragment fragment = getSupportFragmentManager().findFragmentByTag(TAG_BOTTOM_FRAGMENT);
    if (fragment != null) {
      getSupportFragmentManager().beginTransaction().remove(fragment).commit();
    }
  }

  public static class MainMapFragment extends Fragment implements OnMapReadyCallback {

    private static final String[] STYLES = new String[] {
      Style.MAPBOX_STREETS,
      Style.SATELLITE_STREETS,
      Style.LIGHT,
      Style.DARK,
      Style.SATELLITE,
      Style.OUTDOORS
    };

    private MapView map;

    public static MainMapFragment newInstance(int mapCounter) {
      MainMapFragment mapFragment = new MainMapFragment();
      MapboxMapOptions mapboxMapOptions = new MapboxMapOptions();
      mapboxMapOptions.styleUrl(STYLES[Math.min(Math.max(mapCounter, 0), STYLES.length - 1)]);
      mapFragment.setArguments(MapFragmentUtils.createFragmentArgs(mapboxMapOptions));
      return mapFragment;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
      super.onCreateView(inflater, container, savedInstanceState);
      Context context = inflater.getContext();
      return map = new MapView(context, MapFragmentUtils.resolveArgs(context, getArguments()));
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
      super.onViewCreated(view, savedInstanceState);
      map.onCreate(savedInstanceState);
      map.getMapAsync(this);
    }

    @Override
    public void onMapReady(MapboxMap mapboxMap) {
      Location location = mapboxMap.getMyLocation();
      if (location != null) {
        mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(location), 15));
      }
    }

    @Override
    public void onStart() {
      super.onStart();
      map.onStart();
    }

    @Override
    public void onResume() {
      super.onResume();
      map.onResume();
    }

    @Override
    public void onPause() {
      super.onPause();
      map.onPause();
    }

    @Override
    public void onStop() {
      super.onStop();
      map.onStop();
    }

    @Override
    public void onSaveInstanceState(@NonNull Bundle outState) {
      super.onSaveInstanceState(outState);
      map.onSaveInstanceState(outState);
    }

    @Override
    public void onLowMemory() {
      super.onLowMemory();
      map.onLowMemory();
    }

    @Override
    public void onDestroyView() {
      super.onDestroyView();
      map.onDestroy();
    }
  }

  public static class BottomSheetFragment extends Fragment implements OnMapReadyCallback {

    private MapView map;

    public static BottomSheetFragment newInstance() {
      BottomSheetFragment mapFragment = new BottomSheetFragment();
      MapboxMapOptions mapboxMapOptions = new MapboxMapOptions();
      mapboxMapOptions.locationEnabled(true);
      mapboxMapOptions.renderSurfaceOnTop(true);
      mapboxMapOptions.styleUrl(Style.LIGHT);
      mapFragment.setArguments(MapFragmentUtils.createFragmentArgs(mapboxMapOptions));
      return mapFragment;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
      super.onCreateView(inflater, container, savedInstanceState);
      Context context = inflater.getContext();
      return map = new MapView(context, MapFragmentUtils.resolveArgs(context, getArguments()));
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
      super.onViewCreated(view, savedInstanceState);
      map.onCreate(savedInstanceState);
      map.getMapAsync(this);
    }

    @Override
    public void onMapReady(MapboxMap mapboxMap) {
      Location location = mapboxMap.getMyLocation();
      if (location != null) {
        mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(location), 15));
      }
    }

    @Override
    public void onStart() {
      super.onStart();
      map.onStart();
    }

    @Override
    public void onResume() {
      super.onResume();
      map.onResume();
    }

    @Override
    public void onPause() {
      super.onPause();
      map.onPause();
    }

    @Override
    public void onStop() {
      super.onStop();
      map.onStop();
    }

    @Override
    public void onSaveInstanceState(@NonNull Bundle outState) {
      super.onSaveInstanceState(outState);
      map.onSaveInstanceState(outState);
    }

    @Override
    public void onLowMemory() {
      super.onLowMemory();
      map.onLowMemory();
    }

    @Override
    public void onDestroyView() {
      super.onDestroyView();
      map.onDestroy();
    }
  }
}
