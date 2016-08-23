package com.mapbox.mapboxsdk.annotations;

import android.graphics.Bitmap;

import com.mapbox.mapboxsdk.maps.MapView;

/**
 * Icon is the visual representation of a {@link Marker} on a {@link MapView}.
 *
 * @see Marker
 */
public class Icon {
    private Bitmap bitmap;
    private String id;

    Icon(String id, Bitmap bitmap) {
        this.id = id;
        this.bitmap = bitmap;
    }

    public String getId() {
        return id;
    }

    public Bitmap getBitmap() {
        return bitmap;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        Icon icon = (Icon) o;

        if (!bitmap.equals(icon.bitmap)) return false;
        return id.equals(icon.id);

    }

    @Override
    public int hashCode() {
        int result = 0;
        if (bitmap != null) {
            result = bitmap.hashCode();
        }
        if (id != null) {
            result = 31 * result + id.hashCode();
        }
        return result;
    }
}
