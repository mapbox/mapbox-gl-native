package com.almeros.android.multitouch.gesturedetectors;

import android.content.Context;
import android.graphics.PointF;
import android.util.DisplayMetrics;
import android.view.MotionEvent;
import android.view.ViewConfiguration;

/**
 * @author Almer Thie (code.almeros.com) Copyright (c) 2013, Almer Thie
 *         (code.almeros.com)
 *
 *         All rights reserved.
 *
 *         Redistribution and use in source and binary forms, with or without
 *         modification, are permitted provided that the following conditions
 *         are met:
 *
 *         Redistributions of source code must retain the above copyright
 *         notice, this list of conditions and the following disclaimer.
 *         Redistributions in binary form must reproduce the above copyright
 *         notice, this list of conditions and the following disclaimer in the
 *         documentation and/or other materials provided with the distribution.
 *
 *         THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *         "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *         LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *         A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *         HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *         INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *         BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 *         OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *         AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *         LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 *         WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *         POSSIBILITY OF SUCH DAMAGE.
 */
public abstract class TwoFingerGestureDetector extends BaseGestureDetector {

    private final float mEdgeSlop;

    protected float mPrevFingerDiffX;
    protected float mPrevFingerDiffY;
    protected float mCurrFingerDiffX;
    protected float mCurrFingerDiffY;

    private float mCurrLen;
    private float mPrevLen;

    private PointF mFocus;

    public TwoFingerGestureDetector(Context context) {
        super(context);

        ViewConfiguration config = ViewConfiguration.get(context);
        mEdgeSlop = config.getScaledEdgeSlop();
    }

    @Override
    protected abstract void handleStartProgressEvent(int actionCode,
            MotionEvent event);

    @Override
    protected abstract void handleInProgressEvent(int actionCode,
            MotionEvent event);

    protected void updateStateByEvent(MotionEvent curr) {
        super.updateStateByEvent(curr);

        final MotionEvent prev = mPrevEvent;

        mCurrLen = -1;
        mPrevLen = -1;

        // Previous
        final float px0 = prev.getX(0);
        final float py0 = prev.getY(0);
        final float px1 = prev.getX(1);
        final float py1 = prev.getY(1);
        final float pvx = px1 - px0;
        final float pvy = py1 - py0;
        mPrevFingerDiffX = pvx;
        mPrevFingerDiffY = pvy;

        // Current
        final float cx0 = curr.getX(0);
        final float cy0 = curr.getY(0);
        final float cx1 = curr.getX(1);
        final float cy1 = curr.getY(1);
        final float cvx = cx1 - cx0;
        final float cvy = cy1 - cy0;
        mCurrFingerDiffX = cvx;
        mCurrFingerDiffY = cvy;
        mFocus = determineFocalPoint(curr);
    }

    /**
     * Return the current distance between the two pointers forming the gesture
     * in progress.
     *
     * @return Distance between pointers in pixels.
     */
    public float getCurrentSpan() {
        if (mCurrLen == -1) {
            final float cvx = mCurrFingerDiffX;
            final float cvy = mCurrFingerDiffY;
            mCurrLen = (float) Math.sqrt(cvx * cvx + cvy * cvy);
        }
        return mCurrLen;
    }

    /**
     * Return the previous distance between the two pointers forming the gesture
     * in progress.
     *
     * @return Previous distance between pointers in pixels.
     */
    public float getPreviousSpan() {
        if (mPrevLen == -1) {
            final float pvx = mPrevFingerDiffX;
            final float pvy = mPrevFingerDiffY;
            mPrevLen = (float) Math.sqrt(pvx * pvx + pvy * pvy);
        }
        return mPrevLen;
    }

    /**
     * MotionEvent has no getRawX(int) method; simulate it pending future API
     * approval.
     *
     * @param event Motion Event
     * @param pointerIndex Pointer Index
     * @return Raw x value or 0
     */
    protected static float getRawX(MotionEvent event, int pointerIndex) {
        float offset = event.getX() - event.getRawX();
        if (pointerIndex < event.getPointerCount()) {
            return event.getX(pointerIndex) + offset;
        }
        return 0.0f;
    }

    /**
     * MotionEvent has no getRawY(int) method; simulate it pending future API
     * approval.
     *
     * @param event Motion Event
     * @param pointerIndex Pointer Index
     * @return Raw y value or 0
     */
    protected static float getRawY(MotionEvent event, int pointerIndex) {
        float offset = event.getY() - event.getRawY();
        if (pointerIndex < event.getPointerCount()) {
            return event.getY(pointerIndex) + offset;
        }
        return 0.0f;
    }

    /**
     * Check if we have a sloppy gesture. Sloppy gestures can happen if the edge
     * of the user's hand is touching the screen, for example.
     *
     * @param event Motion Event
     * @return {@code true} if is sloppy gesture, {@code false} if not
     */
    protected boolean isSloppyGesture(MotionEvent event) {
        // As orientation can change, query the metrics in touch down
        DisplayMetrics metrics = mContext.getResources().getDisplayMetrics();
        float mRightSlopEdge = metrics.widthPixels - mEdgeSlop;
        float mBottomSlopEdge = metrics.heightPixels - mEdgeSlop;

        final float edgeSlop = mEdgeSlop;

        final float x0 = event.getRawX();
        final float y0 = event.getRawY();
        final float x1 = getRawX(event, 1);
        final float y1 = getRawY(event, 1);

        boolean p0sloppy = x0 < edgeSlop || y0 < edgeSlop || x0 > mRightSlopEdge
                || y0 > mBottomSlopEdge;
        boolean p1sloppy = x1 < edgeSlop || y1 < edgeSlop || x1 > mRightSlopEdge
                || y1 > mBottomSlopEdge;

        if (p0sloppy && p1sloppy) {
            return true;
        } else if (p0sloppy) {
            return true;
        } else if (p1sloppy) {
            return true;
        }
        return false;
    }

    /**
     * Determine (multi)finger focal point (a.k.a. center point between all
     * fingers)
     *
     * @param e Motion Event
     * @return PointF focal point
     */
    public static PointF determineFocalPoint(MotionEvent e) {
        // Number of fingers on screen
        final int pCount = e.getPointerCount();
        float x = 0.0f;
        float y = 0.0f;

        for (int i = 0; i < pCount; i++) {
            x += e.getX(i);
            y += e.getY(i);
        }

        return new PointF(x / pCount, y / pCount);
    }

    public float getFocusX() {
        return mFocus.x;
    }

    public float getFocusY() {
        return mFocus.y;
    }

}
