package com.mapbox.mapboxsdk;

import android.content.Context;
import com.mapbox.mapboxsdk.telemetry.MapboxEventManager;

public class MapboxAccountManager {

    private static MapboxAccountManager mapboxAccountManager = null;

    private final String accessToken;
    private final Context applicationContext;

    /**
     * MapboxAccountManager should NOT be instantiated directly.
     * Use @see MapboxAccountManager#getInstance() instead.
     * @param applicationContext
     * @param accessToken Mapbox Access Token
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
     * @param context Context used to get Application Context
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
        return mapboxAccountManager;
    }

    /**
     * Access Token for this application
     * @return Mapbox Access Token
     */
    public String getAccessToken() {
        return accessToken;
    }
}
