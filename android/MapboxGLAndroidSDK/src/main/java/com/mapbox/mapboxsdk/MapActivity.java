package com.mapbox.mapboxsdk;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;

import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.exceptions.InvalidAccessTokenException;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;

/**
 * An activity that displays a MapBox Map.
 * </p>
 * MapActivity hosts a {@link com.mapbox.mapboxsdk.views.MapView}
 * </p>
 */
public class MapActivity extends AppCompatActivity {

    /**
     * The MapView to be displayed
     */
    protected MapView mMapView;

    /**
     * Bundle to hold the saved state information
     */
    private Bundle mSavedInstanceState;

    /**
     * Called when the MapActivity is created
     *
     * @param savedInstanceState the saved state information
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mSavedInstanceState = savedInstanceState;
    }

    /**
     * Called when the content of the Activity changes as a result of setContentView.
     */
    @Override
    public void onContentChanged() {
        super.onContentChanged();
        mMapView = (MapView) findViewById(R.id.map);
        if (mMapView == null) {
            throw new RuntimeException(
                    "Your content must have a MapView whose id attribute is " +
                            "'R.id.map'");
        }

        String token = getAccessToken();
        if (TextUtils.isEmpty(token)) {
            throw new InvalidAccessTokenException(
                    "Your AndroidManifest.xml must have a meta-data tag with android:name=\" " +
                            "com.mapbox.AccessToken\" and value your mapbox access token");
        }

        mMapView.setAccessToken(token);
        mMapView.setStyleUrl(getDefaultStyle());
        mMapView.onCreate(mSavedInstanceState);

        //
        // add default listeners here
        //
    }

    /**
     * Called when the activity becomes visible.
     */
    @Override
    protected void onStart() {
        super.onStart();
        mMapView.onStart();
    }

    /**
     * Called when the activity is ready to be interacted with.
     */
    @Override
    public void onResume() {
        super.onResume();
        mMapView.onResume();
    }

    /**
     * Called when the activity stops listening to interactions.
     */
    @Override
    public void onPause() {
        super.onPause();
        mMapView.onPause();
    }

    /**
     * Called when the activity is no longer in foreground.
     */
    @Override
    protected void onStop() {
        super.onStop();
        mMapView.onStop();
    }

    /**
     * Called when the activity is going to save state information.
     *
     * @param outState The state information to be saved
     */
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mMapView.onSaveInstanceState(outState);
    }

    /**
     * Called when the Activity is going to restore all the view states.
     * </p>
     * Ensures that the {@link com.mapbox.mapboxsdk.views.MapView} has been created.
     */
    @Override
    protected void onRestoreInstanceState(Bundle state) {
        ensureMapView();
        super.onRestoreInstanceState(state);
    }

    /**
     * Called when the Activity is going to be destroyed.
     */
    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMapView.onDestroy();
    }

    /**
     * Called when the system is low on memory and requires the application to clear memory.
     */
    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mMapView.onLowMemory();
    }

    /**
     * Called to ensure a MapView has been created.
     */
    private void ensureMapView() {
        if (mMapView != null) {
            return;
        }
        setContentView(R.layout.activity_mapview);
    }

    /**
     * Returns the default style, can be overridden to provide a different startup style.
     * </p>
     * The default implementation uses {@link Style#MAPBOX_STREETS} as a style.
     *
     * @return The default style to be used.
     * @see Style for other default styles.
     */
    @NonNull
    protected String getDefaultStyle() {
        return Style.MAPBOX_STREETS;
    }

    /**
     * Returns the default access token, can be overridden to provide an access token.
     * </p>
     * The default implementation uses the {@link ApiAccess} class to load an AccessToken
     *
     * @return The default access token to be used.
     */
    @Nullable
    protected String getAccessToken() {
        return ApiAccess.getToken(this);
    }

    /**
     * Returns The displayed activity's {@link MapView}.
     *
     * @return The displayed MapView
     */
    @NonNull
    protected MapView getMapView() {
        ensureMapView();
        return mMapView;
    }

}
