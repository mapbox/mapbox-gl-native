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
    @StringDef({MAPBOX_STREETS, EMERALD, LIGHT, DARK, SATELLITE, SATELLITE_STREETS})
    @Retention(RetentionPolicy.SOURCE)
    public @interface StyleUrl {
    }

    // IMPORTANT: If you change any of these you also need to edit them in strings.xml

    /**
     * Mapbox Streets: A complete basemap, perfect for incorporating your own data.
     */
    public static final String MAPBOX_STREETS = "asset://styles/streets-v8.json";
    /**
     * Emerald: A versatile style, with emphasis on road networks and public transit.
     */
    public static final String EMERALD = "asset://styles/emerald-v8.json";
    /**
     * Light: Subtle light backdrop for data visualizations.
     */
    public static final String LIGHT = "asset://styles/light-v8.json";
    /**
     * Dark: Subtle dark backdrop for data visualizations.
     */
    public static final String DARK = "asset://styles/dark-v8.json";
    /**
     * Satellite: A beautiful global satellite and aerial imagery layer.
     */
    public static final String SATELLITE = "asset://styles/satellite-v8.json";

    /**
     * Satellite Streets: Global satellite and aerial imagery with unobtrusive labels.
     */
    public static final String SATELLITE_STREETS = "asset://styles/satellite-hybrid-v8.json";

}
