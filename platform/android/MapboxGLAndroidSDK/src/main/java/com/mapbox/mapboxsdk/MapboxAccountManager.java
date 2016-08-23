package com.mapbox.mapboxsdk;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.text.TextUtils;

import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.exceptions.InvalidAccessTokenException;
import com.mapbox.mapboxsdk.exceptions.MapboxAccountManagerNotStartedException;
import com.mapbox.mapboxsdk.telemetry.MapboxEventManager;

public class MapboxAccountManager {

    private static MapboxAccountManager mapboxAccountManager = null;

    private final String accessToken;
    private final Context applicationContext;

    /**
     * MapboxAccountManager should NOT be instantiated directly.
     * Use @see MapboxAccountManager#getInstance() instead.
     *
     * @param applicationContext Context used to get ApplicationContext
     * @param accessToken        Mapbox Access Token
     */
    private MapboxAccountManager(Context applicationContext, String accessToken) {
        super();
        this.applicationContext = applicationContext.getApplicationContext();
        this.accessToken = accessToken;
    }

    /**
     * Primary entry point to Mapbox for implementing developers.
     * Must be configured in either Application.onCreate() or Launch Activity.onCreate()
     *
     * @param context     Context used to get Application Context
     * @param accessToken Mapbox Access Token.  You can get one on the Mapbox Web site.
     * @return MapboxAccountManager instance for app
     */
    public static MapboxAccountManager start(Context context, String accessToken) {
        if (mapboxAccountManager == null) {
            mapboxAccountManager = new MapboxAccountManager(context, accessToken);
        }
        MapboxEventManager eventManager = MapboxEventManager.getMapboxEventManager();
        eventManager.initialize(mapboxAccountManager.applicationContext, mapboxAccountManager.accessToken);
        return mapboxAccountManager;
    }

    /**
     * Internal Use Only
     * Get an instance of MapboxAccountManager configured with the app's Access Token
     *
     * @return MapboxAccountManager instance for app.  May be NULL if not configured yet.
     */
    public static MapboxAccountManager getInstance() {
        if (mapboxAccountManager == null) {
            throw new MapboxAccountManagerNotStartedException();
        }

        return mapboxAccountManager;
    }

    /**
     * Access Token for this application.
     *
     * @return Mapbox Access Token
     */
    public String getAccessToken() {
        return accessToken;
    }

    /**
     * Runtime validation of Access Token.
     *
     * @param accessToken Access Token to check
     * @throws InvalidAccessTokenException the exception thrown
     */
    public static void validateAccessToken(String accessToken) throws InvalidAccessTokenException {
        if (TextUtils.isEmpty(accessToken) || (!accessToken.toLowerCase(MapboxConstants.MAPBOX_LOCALE).startsWith("pk.") && !accessToken.toLowerCase(MapboxConstants.MAPBOX_LOCALE).startsWith("sk."))) {
            throw new InvalidAccessTokenException();
        }
    }

    /**
     * Determines whether we have an Internet connection available.
     *
     * @return true if there is an Internet connection, false otherwise
     */
    public boolean isConnected() {
        if (applicationContext == null) {
            // Assume connection if MapboxAccountManager contains an empty Context
            return true;
        }

        // Check actual connectivity via ConnectivityManager. Code adapted from:
        // https://developer.android.com/training/monitoring-device-state/connectivity-monitoring.html#DetermineConnection
        ConnectivityManager cm = (ConnectivityManager) applicationContext.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo activeNetwork = cm.getActiveNetworkInfo();
        return (activeNetwork != null && activeNetwork.isConnectedOrConnecting());
    }
}
