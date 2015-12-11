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
    public static final String MAPBOX_STREETS = "mapbox://styles/mapbox/streets-v8";
    /**
     * Emerald: A versatile style, with emphasis on road networks and public transit.
     */
    public static final String EMERALD = "mapbox://styles/mapbox/emerald-v8";
    /**
     * Light: Subtle light backdrop for data visualizations.
     */
    public static final String LIGHT = "mapbox://styles/mapbox/light-v8";
    /**
     * Dark: Subtle dark backdrop for data visualizations.
     */
    public static final String DARK = "mapbox://styles/mapbox/dark-v8";
    /**
     * Satellite: A beautiful global satellite and aerial imagery layer.
     */
    public static final String SATELLITE = "mapbox://styles/mapbox/satellite-v8";

    /**
     * Satellite Streets: Global satellite and aerial imagery with unobtrusive labels.
     */
    public static final String SATELLITE_STREETS = "mapbox://styles/mapbox/satellite-hybrid-v8";

}
