package com.mapbox.mapboxsdk.annotations;

import android.graphics.Bitmap;

import com.mapbox.mapboxsdk.maps.MapView;

/**
 * Icon is the visual representation of a {@link Marker} on a {@link MapView}.
 *
 * @see Marker
 */
public class Icon {
    private Bitmap mBitmap;
    private String mId;

    Icon(String id, Bitmap bitmap) {
        mId = id;
        mBitmap = bitmap;
    }

    public String getId() {
        return mId;
    }

    public Bitmap getBitmap() {
        return mBitmap;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        Icon icon = (Icon) o;

        if (!mBitmap.equals(icon.mBitmap)) return false;
        return mId.equals(icon.mId);

    }

    @Override
    public int hashCode() {
        int result = 0;
        if (mBitmap != null) {
            result = mBitmap.hashCode();
        }
        if (mId != null) {
            result = 31 * result + mId.hashCode();
        }
        return result;
    }
}
