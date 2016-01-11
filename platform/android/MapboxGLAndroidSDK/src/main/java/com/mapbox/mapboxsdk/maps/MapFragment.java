package com.mapbox.mapboxsdk.maps;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.maps.MapView;

/**
 * Fragment wrapper around a {@link MapView}.
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
public class MapFragment extends Fragment {

    //
    // Static members
    //

    // Tag used for logging
    private static final String TAG = "MapFragment";

    // Argument used for configuration
    private static final String ARGS_MAPBOXMAP_OPTIONS = "MapboxMapOptions";

    //
    // Instance members
    //

    // The map
    private MapView mMap;
    private OnMapReadyCallback mMapReadyCallback;

    public static MapFragment newInstance(){
        return new MapFragment();
    }

    public static MapFragment newInstance(MapboxMapOptions mapboxMapOptions) {
        final MapFragment mapFragment = new MapFragment();
        Bundle bundle = new Bundle();
        bundle.putParcelable(ARGS_MAPBOXMAP_OPTIONS, mapboxMapOptions);
        mapFragment.setArguments(bundle);
        return mapFragment;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        super.onCreateView(inflater, container, savedInstanceState);
        mMap = (MapView) inflater.inflate(R.layout.fragment_mapview, container, false);
        mMap.setAccessToken(ApiAccess.getToken(inflater.getContext()));
        mMap.onCreate(savedInstanceState);
        return mMap;
    }

    @Override
    public void onStart() {
        super.onStart();
        mMap.onStart();
    }

    @Override
    public void onResume() {
        super.onResume();
        mMap.onResume();
        if(mMapReadyCallback!=null){
            mMapReadyCallback.onMapReady(mMap.getMapboxMap());
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        mMap.onPause();
    }

    @Override
    public void onSaveInstanceState(@NonNull Bundle outState) {
        mMap.onSaveInstanceState(outState);
        super.onSaveInstanceState(outState);
    }

    @Override
    public void onStop() {
        super.onStop();
        mMap.onStop();
    }

    @Override
    public void onLowMemory() {
        mMap.onLowMemory();
        super.onLowMemory();
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        mMap.onDestroy();
        mMap = null;
    }

    @NonNull
    public void getMapAsync(@NonNull OnMapReadyCallback onMapReadyCallback){
        mMapReadyCallback = onMapReadyCallback;
    }
}
