package com.mapbox.mapboxsdk.annotations;

import android.graphics.Bitmap;

public final class Icon {
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
        int result = mBitmap.hashCode();
        result = 31 * result + mId.hashCode();
        return result;
    }
}
