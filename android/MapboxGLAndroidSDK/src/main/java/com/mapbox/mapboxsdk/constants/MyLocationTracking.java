package com.mapbox.mapboxsdk.constants;

import android.support.annotation.IntDef;

import com.mapbox.mapboxsdk.views.MapView;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * MyLocationTracking exposes different types location tracking modes.
 *
 * @see MapView#setMyLocationTrackingMode(int)
 * @see com.mapbox.mapboxsdk.views.UserLocationView#setMyLocationTrackingMode(int)
 */
public class MyLocationTracking {

    /**
     * Indicates the parameter accepts one of the values from {@link MyLocationTracking}.
     */
    @IntDef({TRACKING_NONE, TRACKING_FOLLOW, /**TRACKING_FOLLOW_BEARING_GPS, TRACKING_FOLLOW_BEARING_COMPASS, TRACKING_FOLLOW_BEARING**/})
    @Retention(RetentionPolicy.SOURCE)
    public @interface Mode {
    }

    /**
     * Not tracking user
     */
    public static final int TRACKING_NONE = 0x00000000;

    /**
     * Tracking the location of the user
     */
    public static final int TRACKING_FOLLOW = 0x00000004;

//    /**
//     * Tracking the location of the user with bearing from GPS
//     */
//    public static final int TRACKING_FOLLOW_BEARING_GPS = 0x00000008;
//
//    /**
//     * Tracking the location of the user with bearing from compass
//     */
//    public static final int TRACKING_FOLLOW_BEARING_COMPASS = 0x00000012;
//
//    /**
//     * Tracking the location fot the user with bearing from GPS and compass
//     */
//    public static final int TRACKING_FOLLOW_BEARING = 0x00000016;

}