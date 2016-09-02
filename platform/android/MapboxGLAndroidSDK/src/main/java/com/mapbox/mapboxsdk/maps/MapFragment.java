package com.mapbox.mapboxsdk.maps;

import android.app.Fragment;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.MapboxAccountManager;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.exceptions.InvalidAccessTokenException;

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

    /**
     * Creates a MapFragment instance
     *
     * @param mapboxMapOptions The configuration options to be used.
     * @return MapFragment created.
     */
    public static MapFragment newInstance(@Nullable MapboxMapOptions mapboxMapOptions) {
        MapFragment mapFragment = new MapFragment();
        Bundle bundle = new Bundle();
        bundle.putParcelable(MapboxConstants.FRAG_ARG_MAPBOXMAPOPTIONS, mapboxMapOptions);
        mapFragment.setArguments(bundle);
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
        MapboxMapOptions options = null;

        // Get bundle
        Bundle bundle = getArguments();
        if (bundle != null && bundle.containsKey(MapboxConstants.FRAG_ARG_MAPBOXMAPOPTIONS)) {
            options = bundle.getParcelable(MapboxConstants.FRAG_ARG_MAPBOXMAPOPTIONS);
        }

        // Assign an AccessToken if needed
        if (options == null || options.getAccessToken() == null) {
            String token = null;
            if (MapboxAccountManager.getInstance() != null) {
                token = MapboxAccountManager.getInstance().getAccessToken();
            } else {
                token = getToken(inflater.getContext());
            }
            if (TextUtils.isEmpty(token)) {
                throw new InvalidAccessTokenException();
            }
            if (options == null) {
                options = new MapboxMapOptions().accessToken(token);
            } else {
                options.accessToken(token);
            }
        }

        Drawable foregroundDrawable = options.getMyLocationForegroundDrawable();
        Drawable foregroundBearingDrawable = options.getMyLocationForegroundBearingDrawable();
        if (foregroundDrawable == null || foregroundBearingDrawable == null) {
            if (foregroundDrawable == null) {
                foregroundDrawable = ContextCompat.getDrawable(context, R.drawable.ic_mylocationview_normal);
            }
            if (foregroundBearingDrawable == null) {
                foregroundBearingDrawable = ContextCompat.getDrawable(context, R.drawable.ic_mylocationview_bearing);
            }
            options.myLocationForegroundDrawables(foregroundDrawable, foregroundBearingDrawable);
        }

        if (options.getMyLocationBackgroundDrawable() == null) {
            options.myLocationBackgroundDrawable(ContextCompat.getDrawable(context, R.drawable.ic_mylocationview_background));
        }

        return mMap = new MapView(inflater.getContext(), options);
    }

    /**
     * <p>
     * Returns the Mapbox access token set in the app resources.
     * </p>
     * It will first search the application manifest for a {@link MapboxConstants#KEY_META_DATA_MANIFEST}
     * meta-data value. If not found it will then attempt to load the access token from the
     * {@code res/raw/token.txt} development file.
     *
     * @param context The {@link Context} of the {@link android.app.Activity} or {@link android.app.Fragment}.
     * @return The Mapbox access token or null if not found.
     * @see MapboxConstants#KEY_META_DATA_MANIFEST
     *
     * @deprecated As of release 4.1.0, replaced by {@link com.mapbox.mapboxsdk.MapboxAccountManager#start(Context, String)}
     */
    @Deprecated
    private String getToken(@NonNull Context context) {
        try {
            // read out AndroidManifest
            PackageManager packageManager = context.getPackageManager();
            ApplicationInfo appInfo = packageManager.getApplicationInfo(context.getPackageName(), PackageManager.GET_META_DATA);
            String token = appInfo.metaData.getString(MapboxConstants.KEY_META_DATA_MANIFEST);
            if (token == null || token.isEmpty()) {
                throw new IllegalArgumentException();
            }
            return token;
        } catch (Exception e) {
            // use fallback on string resource, used for development
            int tokenResId = context.getResources().getIdentifier("mapbox_access_token", "string", context.getPackageName());
            return tokenResId != 0 ? context.getString(tokenResId) : null;
        }
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
        mMap.onCreate(savedInstanceState);
    }

    /**
     * Called when the fragment is visible for the users.
     */
    @Override
    public void onStart() {
        super.onStart();
        mMap.getMapAsync(mOnMapReadyCallback);
    }

    /**
     * Called when the fragment is ready to be interacted with.
     */
    @Override
    public void onResume() {
        super.onResume();
        mMap.onResume();
    }

    /**
     * Called when the fragment is pausing.
     */
    @Override
    public void onPause() {
        super.onPause();
        mMap.onPause();
    }

    /**
     * Called when the fragment state needs to be saved.
     *
     * @param outState The saved state
     */
    @Override
    public void onSaveInstanceState(@NonNull Bundle outState) {
        super.onSaveInstanceState(outState);
        mMap.onSaveInstanceState(outState);
    }

    /**
     * Called when the fragment is no longer visible for the user.
     */
    @Override
    public void onStop() {
        super.onStop();
    }

    /**
     * Called when the fragment receives onLowMemory call from the hosting Activity.
     */
    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mMap.onLowMemory();
    }

    /**
     * Called when the fragment is view hiearchy is being destroyed.
     */
    @Override
    public void onDestroyView() {
        super.onDestroyView();
        mMap.onDestroy();
    }

    /**
     * Sets a callback object which will be triggered when the MapboxMap instance is ready to be used.
     *
     * @param onMapReadyCallback The callback to be invoked.
     */
    public void getMapAsync(@NonNull final OnMapReadyCallback onMapReadyCallback) {
        mOnMapReadyCallback = onMapReadyCallback;
    }
}
