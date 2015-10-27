package com.mapbox.mapboxsdk.constants;

import android.support.annotation.IntDef;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * MyBearingTracking exposes different types bearing tracking modes.
 *
 * @see
 * @see
 */
public class MyBearingTracking {

    /**
     * Indicates the parameter accepts one of the values from {@link MyBearingTracking}.
     */
    @IntDef({NONE, COMPASS, GPS, /**COMBINED**/})
    @Retention(RetentionPolicy.SOURCE)
    public @interface Mode {
    }

    public static final int NONE = 0x00000000;
    public static final int COMPASS = 0x00000004;
    public static final int GPS = 0x00000008;
//    public static final int COMBINED = 0x00000012;

}
