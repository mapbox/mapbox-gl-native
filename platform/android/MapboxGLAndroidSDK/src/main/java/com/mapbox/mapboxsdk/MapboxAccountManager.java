package com.mapbox.mapboxsdk;

public class MapboxAccountManager {

    private static MapboxAccountManager mapboxAccountManager = null;

    private final String accessToken;

    /**
     * MapboxAccountManager should NOT be instantiated directly.
     * Use @see MapboxAccountManager#getInstance(String) instead.
     * @param accessToken
     */
    private MapboxAccountManager(String accessToken) {
        super();
        this.accessToken = accessToken;
    }

    /**
     * Get an instance of MapboxAccountManager configured with the app's Access Token
     * @param accessToken Mapbox Access Token.  You can get one on the Mapbox Web site.
     * @return MapboxAccountManager instance for app
     */
    public static MapboxAccountManager getInstance(String accessToken) {
        if (mapboxAccountManager == null) {
            mapboxAccountManager = new MapboxAccountManager(accessToken);
        }
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
