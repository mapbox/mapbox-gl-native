package com.mapbox.mapboxsdk.views;

import android.os.Bundle;

/**
 * A {@link InvalidAccessTokenException} is thrown by {@link MapView} when there is either no access
 * token set before {@link MapView#onCreate(Bundle)} or an invalid access token is set in {@link MapView#setAccessToken(String)}
 *
 * @see MapView#onCreate(Bundle)
 * @see MapView#setAccessToken(String)
 */
public class InvalidAccessTokenException extends RuntimeException {

    InvalidAccessTokenException() {
        super("Using MapView requires setting a valid access token. See the README.md");
    }

}
