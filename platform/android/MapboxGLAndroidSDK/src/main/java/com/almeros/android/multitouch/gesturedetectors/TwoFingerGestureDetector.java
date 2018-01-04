package com.almeros.android.multitouch.gesturedetectors;

import android.content.Context;
import android.graphics.PointF;
import android.util.DisplayMetrics;
import android.view.MotionEvent;
import android.view.ViewConfiguration;

/**
 * @author Almer Thie (code.almeros.com) Copyright (c) 2013, Almer Thie
 *         (code.almeros.com)
 *         <p>
 *         All rights reserved.
 *         <p>
 *         Redistribution and use in source and binary forms, with or without
 *         modification, are permitted provided that the following conditions
 *         are met:
 *         <p>
 *         Redistributions of source code must retain the above copyright
 *         notice, this list of conditions and the following disclaimer.
 *         Redistributions in binary form must reproduce the above copyright
 *         notice, this list of conditions and the following disclaimer in the
 *         documentation and/or other materials provided with the distribution.
 *         <p>
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

  private final float edgeSlop;

  protected float prevFingerDiffX;
  protected float prevFingerDiffY;
  protected float currFingerDiffX;
  protected float currFingerDiffY;

  private float currLen;
  private float prevLen;

  private PointF focus;

  public TwoFingerGestureDetector(Context context) {
    super(context);

    ViewConfiguration config = ViewConfiguration.get(context);

    edgeSlop = config.getScaledEdgeSlop();
  }

  @Override
  protected abstract void handleStartProgressEvent(int actionCode,
                                                   MotionEvent event);

  @Override
  protected abstract void handleInProgressEvent(int actionCode,
                                                MotionEvent event);

  protected void updateStateByEvent(MotionEvent curr) {
    super.updateStateByEvent(curr);

    final MotionEvent prev = prevEvent;

    currLen = -1;
    prevLen = -1;

    // Previous
    final float px0 = prev.getX(0);
    final float py0 = prev.getY(0);
    final float px1 = prev.getX(1);
    final float py1 = prev.getY(1);
    final float pvx = px1 - px0;
    final float pvy = py1 - py0;
    prevFingerDiffX = pvx;
    prevFingerDiffY = pvy;

    // Current
    final float cx0 = curr.getX(0);
    final float cy0 = curr.getY(0);
    final float cx1 = curr.getX(1);
    final float cy1 = curr.getY(1);
    final float cvx = cx1 - cx0;
    final float cvy = cy1 - cy0;
    currFingerDiffX = cvx;
    currFingerDiffY = cvy;
    focus = determineFocalPoint(curr);
  }

  /**
   * Return the current distance between the two pointers forming the gesture
   * in progress.
   *
   * @return Distance between pointers in pixels.
   */
  public float getCurrentSpan() {
    if (currLen == -1) {
      final float cvx = currFingerDiffX;
      final float cvy = currFingerDiffY;
      currLen = (float) Math.sqrt(cvx * cvx + cvy * cvy);
    }
    return currLen;
  }

  /**
   * Return the previous distance between the two pointers forming the gesture
   * in progress.
   *
   * @return Previous distance between pointers in pixels.
   */
  public float getPreviousSpan() {
    if (prevLen == -1) {
      final float pvx = prevFingerDiffX;
      final float pvy = prevFingerDiffY;
      prevLen = (float) Math.sqrt(pvx * pvx + pvy * pvy);
    }
    return prevLen;
  }

  /**
   * MotionEvent has no getRawX(int) method; simulate it pending future API
   * approval.
   *
   * @param event        Motion Event
   * @param pointerIndex Pointer Index
   * @return Raw x value or 0
   */
  protected static float getRawX(MotionEvent event, int pointerIndex) {
    float offset = event.getRawX() - event.getX();
    if (pointerIndex < event.getPointerCount()) {
      return event.getX(pointerIndex) + offset;
    }
    return 0.0f;
  }

  /**
   * MotionEvent has no getRawY(int) method; simulate it pending future API
   * approval.
   *
   * @param event        Motion Event
   * @param pointerIndex Pointer Index
   * @return Raw y value or 0
   */
  protected static float getRawY(MotionEvent event, int pointerIndex) {
    float offset = event.getRawY() - event.getY();
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
    DisplayMetrics metrics = context.getResources().getDisplayMetrics();
    float rightSlopEdge = metrics.widthPixels - edgeSlop;
    float bottomSlopEdge = metrics.heightPixels - edgeSlop;

    final float edgeSlop = this.edgeSlop;

    final float x0 = event.getRawX();
    final float y0 = event.getRawY();
    final float x1 = getRawX(event, 1);
    final float y1 = getRawY(event, 1);

    boolean p0sloppy = x0 < edgeSlop || y0 < edgeSlop || x0 > rightSlopEdge
      || y0 > bottomSlopEdge;
    boolean p1sloppy = x1 < edgeSlop || y1 < edgeSlop || x1 > rightSlopEdge
      || y1 > bottomSlopEdge;

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
   * @param motionEvent Motion Event
   * @return PointF focal point
   */
  public static PointF determineFocalPoint(MotionEvent motionEvent) {
    // Number of fingers on screen
    final int pCount = motionEvent.getPointerCount();
    float x = 0.0f;
    float y = 0.0f;

    for (int i = 0; i < pCount; i++) {
      x += motionEvent.getX(i);
      y += motionEvent.getY(i);
    }

    return new PointF(x / pCount, y / pCount);
  }

  public float getFocusX() {
    return focus.x;
  }

  public float getFocusY() {
    return focus.y;
  }

}