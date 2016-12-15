package com.almeros.android.multitouch.gesturedetectors;

import android.content.Context;
import android.graphics.PointF;
import android.view.MotionEvent;

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
public class MoveGestureDetector extends BaseGestureDetector {

  /**
   * Listener which must be implemented which is used by MoveGestureDetector
   * to perform callbacks to any implementing class which is registered to a
   * MoveGestureDetector via the constructor.
   *
   * @see MoveGestureDetector.SimpleOnMoveGestureListener
   */
  public interface OnMoveGestureListener {
    public boolean onMove(MoveGestureDetector detector);

    public boolean onMoveBegin(MoveGestureDetector detector);

    public void onMoveEnd(MoveGestureDetector detector);
  }

  /**
   * Helper class which may be extended and where the methods may be
   * implemented. This way it is not necessary to implement all methods of
   * OnMoveGestureListener.
   */
  public static class SimpleOnMoveGestureListener implements
    OnMoveGestureListener {
    public boolean onMove(MoveGestureDetector detector) {
      return false;
    }

    public boolean onMoveBegin(MoveGestureDetector detector) {
      return true;
    }

    public void onMoveEnd(MoveGestureDetector detector) {
      // Do nothing, overridden implementation may be used
    }
  }

  private static final PointF FOCUS_DELTA_ZERO = new PointF();

  private final OnMoveGestureListener listener;

  private PointF focusExternal = new PointF();
  private PointF focusDeltaExternal = new PointF();

  public MoveGestureDetector(Context context, OnMoveGestureListener listener) {
    super(context);
    this.listener = listener;
  }

  @Override
  protected void handleStartProgressEvent(int actionCode, MotionEvent event) {
    switch (actionCode) {
      case MotionEvent.ACTION_DOWN:
        resetState(); // In case we missed an UP/CANCEL event

        prevEvent = MotionEvent.obtain(event);
        timeDelta = 0;

        updateStateByEvent(event);
        break;

      case MotionEvent.ACTION_MOVE:
        gestureInProgress = listener.onMoveBegin(this);
        break;
    }
  }

  @Override
  protected void handleInProgressEvent(int actionCode, MotionEvent event) {
    switch (actionCode) {
      case MotionEvent.ACTION_UP:
      case MotionEvent.ACTION_CANCEL:
        listener.onMoveEnd(this);
        resetState();
        break;

      case MotionEvent.ACTION_MOVE:
        updateStateByEvent(event);

        // Only accept the event if our relative pressure is within
        // a certain limit. This can help filter shaky data as a
        // finger is lifted.
        if (currPressure / prevPressure > PRESSURE_THRESHOLD) {
          final boolean updatePrevious = listener.onMove(this);
          if (updatePrevious) {
            prevEvent.recycle();
            prevEvent = MotionEvent.obtain(event);
          }
        }
        break;
    }
  }

  protected void updateStateByEvent(MotionEvent curr) {
    super.updateStateByEvent(curr);

    final MotionEvent prev = prevEvent;

    // Focus intenal
    PointF currFocusInternal = determineFocalPoint(curr);
    PointF prevFocusInternal = determineFocalPoint(prev);

    // Focus external
    // - Prevent skipping of focus delta when a finger is added or removed
    boolean skipNextMoveEvent = prev.getPointerCount() != curr
      .getPointerCount();
    focusDeltaExternal = skipNextMoveEvent ? FOCUS_DELTA_ZERO
      : new PointF(currFocusInternal.x - prevFocusInternal.x,
      currFocusInternal.y - prevFocusInternal.y);

    // - Don't directly use mFocusInternal (or skipping will occur). Add
    // unskipped delta values to focusExternal instead.
    focusExternal.x += focusDeltaExternal.x;
    focusExternal.y += focusDeltaExternal.y;
  }

  /**
   * Determine (multi)finger focal point (a.k.a. center point between all
   * fingers)
   *
   * @param motionEvent a {@link MotionEvent} object.
   * @return PointF focal point
   */
  private PointF determineFocalPoint(MotionEvent motionEvent) {
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
    return focusExternal.x;
  }

  public float getFocusY() {
    return focusExternal.y;
  }

  public PointF getFocusDelta() {
    return focusDeltaExternal;
  }

}
