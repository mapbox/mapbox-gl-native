package com.mapbox.mapboxsdk.exceptions;

import com.mapbox.mapboxsdk.annotations.Sprite;
import com.mapbox.mapboxsdk.annotations.SpriteFactory;

/**
 * A {@code TooManySpritesException} is thrown by {@link SpriteFactory} when it
 * cannot create a {@link Sprite} because there are already too many.
 * <p/>
 * You should try to reuse Sprite objects whenever possible.
 *
 * @see SpriteFactory
 */
public class TooManySpritesException extends RuntimeException {

    public TooManySpritesException() {
        super("Cannot create a Sprite because there are already too many. Try reusing Sprites.");
    }

}
