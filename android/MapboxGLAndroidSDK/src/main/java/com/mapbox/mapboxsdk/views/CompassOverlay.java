package com.mapbox.mapboxsdk.views;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.RectF;
import android.support.annotation.IntDef;
import android.view.MotionEvent;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.geometry.BoundingBox;
import com.mapbox.mapboxsdk.geometry.LatLng;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

final class CompassOverlay implements Overlay {

    //region CLASS VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    @IntDef(flag=true, value={
            COMPASS_GRAVITY_TOP,
            COMPASS_GRAVITY_BOTTOM,
            COMPASS_GRAVITY_LEFT,
            COMPASS_GRAVITY_RIGHT})
    @Retention(RetentionPolicy.SOURCE)
    public @interface CompassGravity {}
    public static final int COMPASS_GRAVITY_TOP = 0x0001;
    public static final int COMPASS_GRAVITY_BOTTOM = COMPASS_GRAVITY_TOP << 1;
    public static final int COMPASS_GRAVITY_LEFT = COMPASS_GRAVITY_TOP << 2;
    public static final int COMPASS_GRAVITY_RIGHT = COMPASS_GRAVITY_TOP << 3;

    //endregion

    //region CLASS METHODS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //endregion

    //region FIELDS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    private MapView mMapView;
    private final Bitmap mCompassBitmap;
    private final int mCompassWidth;
    private final int mCompassHeight;
    private Rect mPixelBounds = null;
    private Rect mMargins = new Rect();
    private RectF mCompassScreenRect = new RectF();
    private final Matrix mRotationMatrix = new Matrix();
    @CompassGravity private int mGravity;
    private boolean mEnabled = false;

    //endregion

    //region INJECTED DEPENDENCIES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //endregion

    //region INJECTED VIEWS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //endregion

    //region CONSTRUCTOR ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    public CompassOverlay(Context context) {
        // View configuration
        mCompassBitmap = BitmapFactory.decodeResource(context.getResources(), R.drawable.compass);
        mCompassWidth = mCompassBitmap.getWidth();
        mCompassHeight = mCompassBitmap.getHeight();

        // Default margins
        int margin  = context.getResources().getDimensionPixelSize(R.dimen.infowindow_margin);
        mMargins.set(margin, margin, margin, margin);

        // Default gravity
        mGravity = COMPASS_GRAVITY_TOP | COMPASS_GRAVITY_RIGHT;
    }

    //endregion

    //region LIFE CYCLE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //endregion

    //region ACCESSORS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    public void setEnabled(boolean enabled) {
        mEnabled = enabled;
    }

    public void setGravity(@CompassGravity int gravity) {
        mGravity = gravity;
        calculateScreenPosition();
    }

    public void setMargins(int left, int top, int right, int bottom) {
        mMargins.set(left, top, right, bottom);
        calculateScreenPosition();
    }

    //endregion

    //region PRIVATE / PACKAGE LOCAL METHODS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    @CompassGravity int getGravity() {
        return mGravity;
    }

    Rect getMargins() {
        return mMargins;
    }

    private void calculateScreenPosition() {
        if (mPixelBounds != null) {
            // left top right bottom
            if ((mGravity & COMPASS_GRAVITY_LEFT) != 0) {
                if ((mGravity & COMPASS_GRAVITY_TOP) != 0) {
                    // left -> top
                    mCompassScreenRect.set(mMargins.left, mMargins.top,
                            mMargins.left + mCompassWidth, mMargins.top + mCompassHeight);
                } else {
                    // left -> bottom
                    int y = mPixelBounds.height() - mCompassHeight - mMargins.bottom;
                    mCompassScreenRect.set(mMargins.left, y,
                            mMargins.left + mCompassWidth, y + mCompassHeight);
                }
            } else if ((mGravity & COMPASS_GRAVITY_RIGHT) != 0) {
                int x = mPixelBounds.width() - mCompassWidth - mMargins.right;
                if ((mGravity & COMPASS_GRAVITY_TOP) != 0) {
                    //right -> top
                    mCompassScreenRect.set(x, mMargins.top, x + mCompassWidth, mMargins.top + mCompassHeight);
                } else {
                    //right -> bottom
                    int y = mPixelBounds.height() - mCompassHeight - mMargins.bottom;
                    mCompassScreenRect.set(x, y, x + mCompassWidth, y + mCompassHeight);
                }
            }
        }
    }

    //endregion

    //region PUBLIC METHODS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //endregion

    //region Overlay ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    @Override
    public void onOverlayDraw(MapView mapView, Canvas canvas, BoundingBox wgsBounds,
                              LatLng wgsCenter, float bearing, float zoom) {

        if (mPixelBounds != null) {
            mRotationMatrix.setRotate(bearing, mCompassWidth / 2, mCompassHeight / 2);
            mRotationMatrix.postTranslate(mCompassScreenRect.left, mCompassScreenRect.top);
            canvas.drawBitmap(mCompassBitmap, mRotationMatrix, null);
        }
    }

    @Override
    public void onOverlayTouchEvent(MotionEvent event) { /* ignored */ }

    @Override
    public void onOverlayAttached(MapView mapView) {
        mMapView = mapView;
    }

    @Override
    public void onOverlayDetached() {
        mMapView = null;
    }

    @Override
    public void onOverlaySingleTapConfirmed(LatLng pressPosition) {
        if (mEnabled) {
            PointF tapPoint = mMapView.toScreenLocation(pressPosition);

            if (mCompassScreenRect.contains(tapPoint.x, tapPoint.y)) {
                mMapView.resetNorth();
            }
        }
    }

    @Override
    public void onOverlayLongPress(LatLng pressPosition) { /* ignored */ }

    @Override
    public boolean isOverlayDrawEnabled() {
        return mEnabled;
    }

    @Override
    public void onMapViewPixelBoundsChanged(Rect mapPixelBounds) {
        mPixelBounds = mapPixelBounds;
        calculateScreenPosition();
    }

    //endregion

    //region LISTENERS  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //endregion

    //region EVENTS  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //endregion

    //region INNER CLASSES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //endregion

}
