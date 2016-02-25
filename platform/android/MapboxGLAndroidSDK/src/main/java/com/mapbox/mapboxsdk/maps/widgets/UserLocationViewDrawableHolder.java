package com.mapbox.mapboxsdk.maps.widgets;

import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;


/**
 * <p>Holder for the UserLocationView drawables.</p>
 * <p>Holds the drawable and its shadow if set, and manage their initialization.</p>
 */
class UserLocationViewDrawableHolder {
    Drawable mDrawable;
    Drawable mShadow;
    final RectF mBounds = new RectF();
    final RectF mShadowBounds = new RectF();

    public UserLocationViewDrawableHolder() {
    }

    public UserLocationViewDrawableHolder(Drawable drawable, Drawable shadow) {
        setDrawable(drawable);
        setShadow(shadow);
    }

    void setDrawable(@NonNull Drawable drawable) {
        mDrawable = drawable;
        setDrawable(mDrawable, mBounds);
    }

    void setShadow(@Nullable Drawable drawable) {
        mShadow = drawable;
        setDrawable(mShadow, mShadowBounds);
    }

    boolean hasShadow() {
        return mShadow != null;
    }

    private static void setDrawable(final @Nullable Drawable drawable, final @NonNull RectF bounds) {
        if (drawable != null) {
            int halfWidth = drawable.getIntrinsicWidth() / 2;
            int halfHeight = drawable.getIntrinsicHeight() / 2;
            bounds.set(
                    -halfWidth,
                    -halfHeight,
                    halfWidth,
                    halfHeight);
            drawable.setBounds(new Rect(
                    -halfWidth,
                    -halfHeight,
                    halfWidth,
                    halfHeight));
        } else {
            bounds.setEmpty();
        }
    }
}
