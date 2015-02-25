package com.mapbox.mapboxgl.lib.util;

import com.mapbox.mapboxgl.lib.constants.MapboxConstants;
import com.squareup.okhttp.Callback;
import com.squareup.okhttp.OkHttpClient;
import com.squareup.okhttp.Request;

public class JavaFileSource {

    private static JavaFileSource instance = null;

    // Single reference to OkHttp for performance gains
    private OkHttpClient client;

    /**
     * Private Constructor to support Singleton pattern
     */
    private JavaFileSource() {
        super();
        client = new OkHttpClient();
    }

    /**
     * Get the singleton instance of JavaFileSource
     * @return Reference to the Singleton Instance of JavaFileSource
     */
    public static JavaFileSource getInstance() {
        if (instance == null) {
            instance = new JavaFileSource();
        }
        return instance;
    }

    /**
     * Make an HTTP Request
     * @param resourceUrl URL to resource
     * @param callback Callback class
     */
    public void request(final String resourceUrl, final Callback callback) {
        Request request = new Request.Builder().url(resourceUrl).tag(resourceUrl.toLowerCase(MapboxConstants.MAPBOX_LOCALE)).build();
        client.newCall(request).enqueue(callback);
    }

    /**
     * Attempt to cancel HTTP Request made
     * @param resourceUrl URL of request to cancel
     */
    public void cancel(final String resourceUrl) {
        client.cancel(resourceUrl.toLowerCase(MapboxConstants.MAPBOX_LOCALE));
    }
}
