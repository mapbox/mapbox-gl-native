package com.mapbox.mapboxsdk.maps;

import android.app.Fragment;
import android.content.Context;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.utils.MapFragmentUtils;

/**
 * Fragment wrapper around a map view.
 * <p>
 * A Map component in an app. This fragment is the simplest way to place a map in an application.
 * It's a wrapper around a view of a map to automatically handle the necessary life cycle needs.
 * Being a fragment, this component can be added to an activity's layout or can dynamically be added
 * using a FragmentManager.
 * </p>
 * <p>
 * To get a reference to the MapView, use {@link #getMapAsync(OnMapReadyCallback)}}
 * </p>
 *
 * @see #getMapAsync(OnMapReadyCallback)
 */
public final class MapFragment extends Fragment {

  private MapView map;
  private OnMapReadyCallback onMapReadyCallback;

  /**
   * Creates a default MapFragment instance
   *
   * @return MapFragment instantiated
   */
  public static MapFragment newInstance() {
    return new MapFragment();
  }

  /**
   * Creates a MapFragment instance
   *
   * @param mapboxMapOptions The configuration options to be used.
   * @return MapFragment instantiated.
   */
  public static MapFragment newInstance(@Nullable MapboxMapOptions mapboxMapOptions) {
    MapFragment mapFragment = new MapFragment();
    mapFragment.setArguments(MapFragmentUtils.createFragmentArgs(mapboxMapOptions));
    return mapFragment;
  }

  /**
   * Creates the fragment view hierarchy.
   *
   * @param inflater           Inflater used to inflate content.
   * @param container          The parent layout for the map fragment.
   * @param savedInstanceState The saved instance state for the map fragment.
   * @return The view created
   */
  @Override
  public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
    super.onCreateView(inflater, container, savedInstanceState);
    Context context = inflater.getContext();
    return map = new MapView(context, MapFragmentUtils.resolveArgs(context, getArguments()));
  }

  /**
   * Called when the fragment view hierarchy is created.
   *
   * @param view               The content view of the fragment
   * @param savedInstanceState THe saved instance state of the framgnt
   */
  @Override
  public void onViewCreated(View view, Bundle savedInstanceState) {
    super.onViewCreated(view, savedInstanceState);
    map.onCreate(savedInstanceState);
  }

  /**
   * Called when the fragment is visible for the users.
   */
  @Override
  public void onStart() {
    super.onStart();
    map.onStart();
    map.getMapAsync(onMapReadyCallback);
  }

  /**
   * Called when the fragment is ready to be interacted with.
   */
  @Override
  public void onResume() {
    super.onResume();
    map.onResume();
  }

  /**
   * Called when the fragment is pausing.
   */
  @Override
  public void onPause() {
    super.onPause();
    map.onPause();
  }

  /**
   * Called when the fragment state needs to be saved.
   *
   * @param outState The saved state
   */
  @Override
  public void onSaveInstanceState(@NonNull Bundle outState) {
    super.onSaveInstanceState(outState);
    map.onSaveInstanceState(outState);
  }

  /**
   * Called when the fragment is no longer visible for the user.
   */
  @Override
  public void onStop() {
    super.onStop();
    map.onStop();
  }

  /**
   * Called when the fragment receives onLowMemory call from the hosting Activity.
   */
  @Override
  public void onLowMemory() {
    super.onLowMemory();
    map.onLowMemory();
  }

  /**
   * Called when the fragment is view hiearchy is being destroyed.
   */
  @Override
  public void onDestroyView() {
    super.onDestroyView();
    map.onDestroy();
  }

  /**
   * Sets a callback object which will be triggered when the MapboxMap instance is ready to be used.
   *
   * @param onMapReadyCallback The callback to be invoked.
   */
  public void getMapAsync(@NonNull final OnMapReadyCallback onMapReadyCallback) {
    this.onMapReadyCallback = onMapReadyCallback;
  }
}
