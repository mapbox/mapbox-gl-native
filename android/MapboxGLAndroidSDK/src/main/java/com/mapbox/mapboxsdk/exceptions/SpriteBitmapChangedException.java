package com.mapbox.mapboxsdk.exceptions;

import android.graphics.Bitmap;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.Sprite;
import com.mapbox.mapboxsdk.views.MapView;

/**
 * A {@code SpriteBitmapChangedException} is thrown by {@link MapView} when a {@link Marker} is added
 * that has a {@link Sprite} with a {@link Bitmap} that has been modified.
 * <p/>
 * You cannot modify a {@code Sprite} after it has been added to the map in a {@code Marker}
 *
 * @see MapView
 * @see Sprite
 * @see Marker
 */
public class SpriteBitmapChangedException extends RuntimeException {

    public SpriteBitmapChangedException() {
        super("The added Marker has a Sprite with a Bitmap that has been modified. You cannot modufy" +
                "a Sprite after it has been added in a Marker.");
    }

}
