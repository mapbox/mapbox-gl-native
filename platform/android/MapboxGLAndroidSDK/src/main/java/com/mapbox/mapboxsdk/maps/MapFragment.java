package com.mapbox.mapboxsdk.maps;

import android.app.Fragment;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.constants.MapboxConstants;

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

    private MapView mMap;
    private OnMapReadyCallback mOnMapReadyCallback;

    public static MapFragment newInstance(@Nullable MapboxMapOptions mapboxMapOptions) {
        MapFragment mapFragment = new MapFragment();
        Bundle bundle = new Bundle();
        bundle.putParcelable(MapboxConstants.FRAG_ARG_MAPBOXMAPOPTIONS, mapboxMapOptions);
        mapFragment.setArguments(bundle);
        return mapFragment;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        super.onCreateView(inflater, container, savedInstanceState);
        MapboxMapOptions options = getArguments().getParcelable(MapboxConstants.FRAG_ARG_MAPBOXMAPOPTIONS);
        return mMap = new MapView(inflater.getContext(), options);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        mMap.onCreate(savedInstanceState);
    }

    @Override
    public void onStart() {
        super.onStart();
        mMap.onStart();
        mMap.getMapAsync(mOnMapReadyCallback);
    }

    @Override
    public void onResume() {
        super.onResume();
        mMap.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        mMap.onPause();
    }

    @Override
    public void onSaveInstanceState(@NonNull Bundle outState) {
        super.onSaveInstanceState(outState);
        mMap.onSaveInstanceState(outState);
    }

    @Override
    public void onStop() {
        super.onStop();
        mMap.onStop();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mMap.onLowMemory();
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        mMap.onDestroy();
        mMap = null;
    }

    public void getMapAsync(@NonNull final OnMapReadyCallback onMapReadyCallback) {
        mOnMapReadyCallback = onMapReadyCallback;
    }
}
