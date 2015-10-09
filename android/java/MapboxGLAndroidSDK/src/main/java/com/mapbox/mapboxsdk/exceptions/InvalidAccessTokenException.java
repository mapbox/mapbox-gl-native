package com.mapbox.mapboxsdk.exceptions;

import android.os.Bundle;
import com.mapbox.mapboxsdk.views.MapView;

/**
 * A {@code InvalidAccessTokenException} is thrown by {@link MapView} when there is either no access
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
