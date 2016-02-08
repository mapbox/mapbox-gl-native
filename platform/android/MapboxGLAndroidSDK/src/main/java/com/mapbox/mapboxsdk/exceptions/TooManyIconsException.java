package com.mapbox.mapboxsdk.exceptions;

import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;

/**
 * <p>
 * A {@code TooManyIconsException} is thrown by {@link IconFactory} when it
 * cannot create a {@link Icon} because there are already too many.
 * </p>
 * You should try to reuse Icon objects whenever possible.
 *
 * @see IconFactory
 */
public class TooManyIconsException extends RuntimeException {

    public TooManyIconsException() {
        super("Cannot create a Icon because there are already too many. Try reusing Icons.");
    }

}
