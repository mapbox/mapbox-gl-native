package com.mapbox.mapboxsdk.constants;

import android.support.annotation.StringDef;
import com.mapbox.mapboxsdk.maps.MapView;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

/**
 * <p>
 * Style provides URLs to several professional styles designed by Mapbox.
 * </p>
 * These styles are all ready to go in your app. To load one, pass it into {@link MapView#setStyleUrl(String)}
 *
 * @see MapView#setStyleUrl(String)
 */
public class Style {

    /**
     * Mapbox Streets: A complete basemap, perfect for incorporating your own data.
     */
    private static final String MAPBOX_STREETS_BASE = "mapbox://styles/mapbox/streets-v%d";
    /**
     * Outdoors: A general-purpose style tailored to outdoor activities.
     */
    private static final String OUTDOORS_BASE = "mapbox://styles/mapbox/outdoors-v%d";
    /**
     * Light: Subtle light backdrop for data visualizations.
     */
    private static final String LIGHT_BASE = "mapbox://styles/mapbox/light-v%d";
    /**
     * Dark: Subtle dark backdrop for data visualizations.
     */
    private static final String DARK_BASE = "mapbox://styles/mapbox/dark-v%d";
    /**
     * Satellite: A beautiful global satellite and aerial imagery layer.
     */
    private static final String SATELLITE_BASE = "mapbox://styles/mapbox/satellite-v%d";
    /**
     * Satellite Streets: Global satellite and aerial imagery with unobtrusive labels.
     */
    private static final String SATELLITE_STREETS_BASE = "mapbox://styles/mapbox/satellite-streets-v%d";

    /**
     * Satellite Streets: Global satellite and aerial imagery with unobtrusive labels (Version 8).
     */
    private static final String SATELLITE_STREETS_V8 = "mapbox://styles/mapbox/satellite-hybrid-v8";

    /**
     * Get versioned url of Mapbox streets style.
     * <p>
     * <ul>
     * <li>Current default version is 9.</li>
     * </ul
     * </p>
     * <p>
     * More information on the Mapbox styles API can be found on https://www.mapbox.com/api-documentation/#styles
     * </p>
     *
     * @param version the version of the style.
     * @return uri to load style from
     */
    public static String getMapboxStreetsUrl(int version) {
        return String.format(MapboxConstants.MAPBOX_LOCALE, MAPBOX_STREETS_BASE, version);
    }

    /**
     * Get versioned url of Outdoors streets style.
     * <p>
     * <ul>
     * <li>Current version is 9.</li>
     * </ul>
     * </p>
     * <p>
     * More information on the Mapbox styles API can be found on https://www.mapbox.com/api-documentation/#styles
     * </p>
     *
     * @param version the version of the style.
     * @return uri to load style from
     */
    public static String getOutdoorsStyleUrl(int version) {
        return String.format(MapboxConstants.MAPBOX_LOCALE, OUTDOORS_BASE, version);
    }

    /**
     * Get versioned url of Light style.
     * <p>
     * <ul>
     * <li>Current default version is 9.</li>
     * </ul>
     * </p>
     * <p>
     * More information on the Mapbox styles API can be found on https://www.mapbox.com/api-documentation/#styles
     * </p>
     *
     * @param version the version of the style.
     * @return uri to load style from
     */
    public static String getLightStyleUrl(int version) {
        return String.format(MapboxConstants.MAPBOX_LOCALE, LIGHT_BASE, version);
    }

    /**
     * Get versioned url of Dark style.
     * <p>
     * <ul>
     * <li>Current default version is 9.</li>
     * </ul>
     * </p>
     * <p>
     * More information on the Mapbox styles API can be found on https://www.mapbox.com/api-documentation/#styles
     * </p>
     *
     * @param version the version of the style.
     * @return uri to load style from
     */
    public static String getDarkStyleUrl(int version) {
        return String.format(MapboxConstants.MAPBOX_LOCALE, DARK_BASE, version);
    }

    /**
     * Get versioned url of Satellite style.
     * <p>
     * <ul>
     * <li>Current version is 9.</li>
     * </ul>
     * </p>
     * <p>
     * More information on the Mapbox styles API can be found on https://www.mapbox.com/api-documentation/#styles
     * </p>
     *
     * @param version the version of the style.
     * @return uri to load style from
     */
    public static String getSatelliteStyleUrl(int version) {
        return String.format(MapboxConstants.MAPBOX_LOCALE, SATELLITE_BASE, version);
    }

    /**
     * Get versioned url of Satellite streets style.
     * <p>
     * <ul>
     * <li>Current version is 9.</li>
     * </ul>
     * </p>
     * <p>
     * More information on the Mapbox styles API can be found on https://www.mapbox.com/api-documentation/#styles
     * </p>
     *
     * @param version the version of the style.
     * @return uri to load style from
     */
    public static String getSatelliteStreetsStyleUrl(int version) {
        if (version == 8) {
            return SATELLITE_STREETS_V8;
        }
        return String.format(MapboxConstants.MAPBOX_LOCALE, SATELLITE_STREETS_BASE, version);
    }

    /**
     * Indicates the parameter accepts one of the values from {@link Style}.
     *
     * @deprecated use dedicated versioned methods in {@link Style} instead.
     */
    @StringDef({MAPBOX_STREETS, EMERALD, LIGHT, DARK, SATELLITE, SATELLITE_STREETS})
    @Retention(RetentionPolicy.SOURCE)
    @Deprecated
    public @interface StyleUrl {
    }

    // IMPORTANT: If you change any of these you also need to edit them in strings.xml

    /**
     * Mapbox Streets: A complete basemap, perfect for incorporating your own data.
     *
     * @deprecated use {@link #getMapboxStreetsUrl(int)} instead.
     */
    @Deprecated
    public static final String MAPBOX_STREETS = "mapbox://styles/mapbox/streets-v9";

    /**
     * Emerald: A versatile style, with emphasis on road networks and public transit.
     *
     * @deprecated this style has been deprecated and will be removed in future versions.
     */
    @Deprecated
    public static final String EMERALD = "mapbox://styles/mapbox/emerald-v8";

    /**
     * Light: Subtle light backdrop for data visualizations.
     *
     * @deprecated use {@link #getLightStyleUrl(int)} instead.
     */
    @Deprecated
    public static final String LIGHT = "mapbox://styles/mapbox/light-v9";

    /**
     * Dark: Subtle dark backdrop for data visualizations.
     *
     * @deprecated use {@link #getDarkStyleUrl(int)} (int)} instead.
     */
    @Deprecated
    public static final String DARK = "mapbox://styles/mapbox/dark-v9";

    /**
     * Satellite: A beautiful global satellite and aerial imagery layer.
     *
     * @deprecated use {@link #getSatelliteStyleUrl(int)} instead.
     */
    @Deprecated
    public static final String SATELLITE = "mapbox://styles/mapbox/satellite-v9";

    /**
     * Satellite Streets: Global satellite and aerial imagery with unobtrusive labels.
     *
     * @deprecated use {@link #getSatelliteStreetsStyleUrl(int)} (int)} instead.
     */
    @Deprecated
    public static final String SATELLITE_STREETS = "mapbox://styles/mapbox/satellite-streets-v9";
}
