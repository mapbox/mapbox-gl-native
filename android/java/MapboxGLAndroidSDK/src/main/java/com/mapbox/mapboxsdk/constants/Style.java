package com.mapbox.mapboxsdk.constants;

import android.support.annotation.StringDef;

import com.mapbox.mapboxsdk.views.MapView;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
/**
* Style provides URLs to several professional styles designed by Mapbox.
* <p/>
* These styles are all ready to go in your app. To load one, pass it into {@link MapView#setStyleUrl(String)}
*
* @see MapView#setStyleUrl(String)
*/
public class Style {

    /**
     * Indicates the parameter accepts one of the values from {@link Style}.
     */
    @StringDef({MAPBOX_STREETS, EMERALD, LIGHT, DARK, SATELLITE})
    @Retention(RetentionPolicy.SOURCE)
    public @interface StyleUrl {
    }

    // IMPORTANT: If you change any of these you also need to edit them in strings.xml

    /**
     * Mapbox Streets: Our signature style.
     */
    public static final String MAPBOX_STREETS = "asset://styles/streets-v8.json";
    /**
     * Emerald: Great for transportation and outdoor terrain.
     */
    public static final String EMERALD = "asset://styles/emerald-v8.json";
    /**
     * Light: Light-colored style that is great for data overlay.
     */
    public static final String LIGHT = "asset://styles/light-v8.json";
    /**
     * Dark: Dark-colored style that is great for data overlay.
     */
    public static final String DARK = "asset://styles/dark-v8.json";
    /**
     * Satellite: The best-looking, most accurate, and most up-to-date satellite imagery available anywhere.
     */
    public static final String SATELLITE = "asset://styles/satellite-v8.json";

}
