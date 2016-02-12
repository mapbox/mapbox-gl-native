package com.mapbox.mapboxsdk.maps;

import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.utils.ApiAccess;

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
public class SupportMapFragment extends Fragment {

    private MapView mMap;

    public static SupportMapFragment newInstance() {
        return new SupportMapFragment();
    }

    public static SupportMapFragment newInstance(MapboxMapOptions mapboxMapOptions) {
        final SupportMapFragment mapFragment = new SupportMapFragment();
        Bundle bundle = new Bundle();
        bundle.putParcelable(MapboxConstants.FRAG_ARG_MAPBOXMAPOPTIONS, mapboxMapOptions);
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
    public void getMapAsync(@NonNull final OnMapReadyCallback onMapReadyCallback) {
        new Handler().post(new Runnable() {
            @Override
            public void run() {
                mMap.getMapAsync(onMapReadyCallback);
            }
        });
    }
}
