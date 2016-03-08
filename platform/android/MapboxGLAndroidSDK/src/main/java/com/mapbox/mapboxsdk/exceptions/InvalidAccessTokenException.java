package com.mapbox.mapboxsdk.exceptions;

import android.os.Bundle;
import com.mapbox.mapboxsdk.maps.MapView;

/**
 * A {@code InvalidAccessTokenException} is thrown by {@link MapView} when there is either no access
 * token set before {@link MapView#onCreate(Bundle)} or an invalid access token is set in {@link MapView#setAccessToken(String)}
 *
 * @see MapView#onCreate(Bundle)
 * @see MapView#setAccessToken(String)
 */
public class InvalidAccessTokenException extends RuntimeException {

    public InvalidAccessTokenException() {
        super("\nUsing MapView requires setting a valid access token. Please see https://www.mapbox.com/help/create-api-access-token/ to learn how to create one.\n     Lastly, follow the steps in this guide https://www.mapbox.com/help/first-steps-android-sdk/#access-tokens in order to include the access token in your app.");
    }

}
