package com.mapbox.mapboxsdk.constants;

import android.support.annotation.IntDef;

import com.mapbox.mapboxsdk.views.MapView;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * MyLocationTracking exposes different types of locational tracking modes.
 *
 * @see MapView#setMyLocationTrackingMode(int)
 * @see com.mapbox.mapboxsdk.views.UserLocationView#setMyLocationTrackingMode(int)
 */
public class MyLocationTracking {

    /**
     * Indicates the parameter accepts one of the values from {@link MyLocationTracking}.
     */
    @IntDef({TRACKING_NONE, TRACKING_FOLLOW})
    @Retention(RetentionPolicy.SOURCE)
    public @interface Mode {
    }

    /**
     * Location tracking is disabled.
     */
    public static final int TRACKING_NONE = 0x00000000;

    /**
     * Tracking user, {@link MapView} will reposition to center of {@link com.mapbox.mapboxsdk.views.UserLocationView}
     */
    public static final int TRACKING_FOLLOW = 0x00000004;

}