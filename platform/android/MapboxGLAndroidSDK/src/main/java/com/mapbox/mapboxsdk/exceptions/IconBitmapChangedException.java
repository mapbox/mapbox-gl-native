package com.mapbox.mapboxsdk.exceptions;

import android.graphics.Bitmap;

import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.views.MapView;

/**
 * <p>
 * A {@code IconBitmapChangedException} is thrown by {@link MapView} when a {@link Marker} is added
 * that has a {@link Icon} with a {@link Bitmap} that has been modified.
 * </p>
 * You cannot modify a {@code Icon} after it has been added to the map in a {@code Marker}
 *
 * @see MapView
 * @see Icon
 * @see Marker
 */
public class IconBitmapChangedException extends RuntimeException {

    public IconBitmapChangedException() {
        super("The added Marker has a Icon with a Bitmap that has been modified. You cannot modufy" +
                "a Icon after it has been added in a Marker.");
    }

}
