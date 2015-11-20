package com.mapbox.mapboxsdk;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.exceptions.InvalidAccessTokenException;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;

/**
 * An activity that displays a Map.
 * <p/>
 * MapActivity hosts a {@link com.mapbox.mapboxsdk.views.MapView}, typically either an array or a Cursor
 * holding query results. Binding, screen layout, and row layout are discussed
 * in the following sections.
 * <p/>
 */
public class MapActivity extends AppCompatActivity {

    protected MapView mMapView;
    private Bundle mSavedInstanceState;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mSavedInstanceState = savedInstanceState;
    }

    /**
     * Updates the screen state when the content changes.
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

        String token = ApiAccess.getToken(this);
        if (token == null) {
            throw new InvalidAccessTokenException(
                    "Your AndroidManifest.xml must have a meta-data tag with android:name=\" " +
                            "com.mapbox.AccessToken\" and value your mapbox access token");
        }

        mMapView.setAccessToken(getAccessToken());
        mMapView.setStyleUrl(getStyle());
        mMapView.onCreate(mSavedInstanceState);
        // could add empty view here
        // could add listeners here
        // requestfocus handler
    }

    @Override
    protected void onStart() {
        super.onStart();
        mMapView.onStart();
    }

    @Override
    public void onResume() {
        super.onResume();
        mMapView.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        mMapView.onPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
        mMapView.onStop();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mMapView.onSaveInstanceState(outState);
    }

    /**
     * Ensures the {@link com.mapbox.mapboxsdk.views.MapView} has been created before Activity
     * restores all of the view states.
     */
    @Override
    protected void onRestoreInstanceState(Bundle state) {
        ensureMapView();
        super.onRestoreInstanceState(state);
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMapView.onDestroy();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mMapView.onLowMemory();
    }

    private void ensureMapView() {
        if (mMapView != null) {
            return;
        }
        setContentView(R.layout.activity_mapview);
    }

    protected String getStyle() {
        return Style.MAPBOX_STREETS;
    }

    protected String getAccessToken() {
        return ApiAccess.getToken(this);
    }

    protected MapView getMapView() {
        ensureMapView();
        return mMapView;
    }

}
