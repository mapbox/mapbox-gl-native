package com.mapbox.mapboxsdk.maps;

import android.graphics.PointF;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.NonNull;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.ViewConfiguration;

/**
 * Manages key events on a MapView.
 * <p>
 * <ul>
 * <li> Uses {@link Transform} to change the map state</li>
 * <li> Uses {@link TrackingSettings} to verify validity of the current tracking mode.</li>
 * <li> Uses {@link UiSettings} to verify validity of user restricted movement.</li>
 * </ul>
 * <p>
 */
final class MapKeyListener {

  private final TrackingSettings trackingSettings;
  private final Transform transform;
  private final UiSettings uiSettings;

  private TrackballLongPressTimeOut currentTrackballLongPressTimeOut;

  MapKeyListener(@NonNull Transform transform, @NonNull TrackingSettings trackingSettings,
                 @NonNull UiSettings uiSettings) {
    this.transform = transform;
    this.trackingSettings = trackingSettings;
    this.uiSettings = uiSettings;
  }

  /**
   * Called when the user presses a key, alse called for repeated keys held down.
   *
   * @param keyCode the id of the pressed key
   * @param event   the related key event
   * @return true if the wevent is handled
   */
  boolean onKeyDown(int keyCode, @NonNull KeyEvent event) {
    // If the user has held the scroll key down for a while then accelerate
    // the scroll speed
    double scrollDist = event.getRepeatCount() >= 5 ? 50.0 : 10.0;

    // Check which key was pressed via hardware/real key code
    switch (keyCode) {
      // Tell the system to track these keys for long presses on
      // onKeyLongPress is fired
      case KeyEvent.KEYCODE_ENTER:
      case KeyEvent.KEYCODE_DPAD_CENTER:
        event.startTracking();
        return true;

      case KeyEvent.KEYCODE_DPAD_LEFT:
        if (!trackingSettings.isScrollGestureCurrentlyEnabled()) {
          return false;
        }

        // Cancel any animation
        transform.cancelTransitions();

        // Move left
        transform.moveBy(scrollDist, 0.0, 0 /*no animation*/);
        return true;

      case KeyEvent.KEYCODE_DPAD_RIGHT:
        if (!trackingSettings.isScrollGestureCurrentlyEnabled()) {
          return false;
        }

        // Cancel any animation
        transform.cancelTransitions();

        // Move right
        transform.moveBy(-scrollDist, 0.0, 0 /*no animation*/);
        return true;

      case KeyEvent.KEYCODE_DPAD_UP:
        if (!trackingSettings.isScrollGestureCurrentlyEnabled()) {
          return false;
        }

        // Cancel any animation
        transform.cancelTransitions();

        // Move up
        transform.moveBy(0.0, scrollDist, 0 /*no animation*/);
        return true;

      case KeyEvent.KEYCODE_DPAD_DOWN:
        if (!trackingSettings.isScrollGestureCurrentlyEnabled()) {
          return false;
        }

        // Cancel any animation
        transform.cancelTransitions();

        // Move down
        transform.moveBy(0.0, -scrollDist, 0 /*no animation*/);
        return true;

      default:
        // We are not interested in this key
        return false;
    }
  }

  /**
   * Called when the user long presses a key that is being tracked.
   *
   * @param keyCode the id of the long pressed key
   * @param event   the related key event
   * @return true if event is handled
   */
  boolean onKeyLongPress(int keyCode, KeyEvent event) {
    // Check which key was pressed via hardware/real key code
    switch (keyCode) {
      // Tell the system to track these keys for long presses on
      // onKeyLongPress is fired
      case KeyEvent.KEYCODE_ENTER:
      case KeyEvent.KEYCODE_DPAD_CENTER:
        if (!uiSettings.isZoomGesturesEnabled()) {
          return false;
        }

        // Zoom out
        PointF focalPoint = new PointF(uiSettings.getWidth() / 2, uiSettings.getHeight() / 2);
        transform.zoom(false, focalPoint);
        return true;

      default:
        // We are not interested in this key
        return false;
    }
  }

  /**
   * Called when the user releases a key.
   *
   * @param keyCode the id of the released key
   * @param event   the related key event
   * @return true if the event is handled
   */
  boolean onKeyUp(int keyCode, KeyEvent event) {
    // Check if the key action was canceled (used for virtual keyboards)
    if (event.isCanceled()) {
      return false;
    }

    // Check which key was pressed via hardware/real key code
    // Note if keyboard does not have physical key (ie primary non-shifted
    // key) then it will not appear here
    // Must use the key character map as physical to character is not
    // fixed/guaranteed
    switch (keyCode) {
      case KeyEvent.KEYCODE_ENTER:
      case KeyEvent.KEYCODE_DPAD_CENTER:
        if (!uiSettings.isZoomGesturesEnabled()) {
          return false;
        }

        // Zoom in
        PointF focalPoint = new PointF(uiSettings.getWidth() / 2, uiSettings.getHeight() / 2);
        transform.zoom(true, focalPoint);
        return true;
    }

    // We are not interested in this key
    return false;
  }

  /**
   * Called for trackball events, all motions are relative in device specific units.
   *
   * @param event the related motion event
   * @return true if the event is handled
   */
  boolean onTrackballEvent(MotionEvent event) {
    // Choose the action
    switch (event.getActionMasked()) {
      // The trackball was rotated
      case MotionEvent.ACTION_MOVE:
        if (!trackingSettings.isScrollGestureCurrentlyEnabled()) {
          return false;
        }

        // Cancel any animation
        transform.cancelTransitions();

        // Scroll the map
        transform.moveBy(-10.0 * event.getX(), -10.0 * event.getY(), 0 /*no animation*/);
        return true;

      // Trackball was pushed in so start tracking and tell system we are
      // interested
      // We will then get the up action
      case MotionEvent.ACTION_DOWN:
        // Set up a delayed callback to check if trackball is still
        // After waiting the system long press time out
        if (currentTrackballLongPressTimeOut != null) {
          currentTrackballLongPressTimeOut.cancel();
          currentTrackballLongPressTimeOut = null;
        }
        currentTrackballLongPressTimeOut = new TrackballLongPressTimeOut();
        new Handler(Looper.getMainLooper()).postDelayed(currentTrackballLongPressTimeOut,
          ViewConfiguration.getLongPressTimeout());
        return true;

      // Trackball was released
      case MotionEvent.ACTION_UP:
        if (!uiSettings.isZoomGesturesEnabled()) {
          return false;
        }

        // Only handle if we have not already long pressed
        if (currentTrackballLongPressTimeOut != null) {
          // Zoom in
          PointF focalPoint = new PointF(uiSettings.getWidth() / 2, uiSettings.getHeight() / 2);
          transform.zoom(true, focalPoint);
        }
        return true;

      // Trackball was cancelled
      case MotionEvent.ACTION_CANCEL:
        if (currentTrackballLongPressTimeOut != null) {
          currentTrackballLongPressTimeOut.cancel();
          currentTrackballLongPressTimeOut = null;
        }
        return true;

      default:
        // We are not interested in this event
        return false;
    }
  }

  /**
   * This class implements the trackball long press time out callback
   */
  private class TrackballLongPressTimeOut implements Runnable {

    // Track if we have been cancelled
    private boolean cancelled;

    TrackballLongPressTimeOut() {
      cancelled = false;
    }

    // Cancel the timeout
    public void cancel() {
      cancelled = true;
    }

    // Called when long press time out expires
    @Override
    public void run() {
      // Check if the trackball is still pressed
      if (!cancelled) {
        // Zoom out
        PointF pointF = new PointF(uiSettings.getWidth() / 2, uiSettings.getHeight() / 2);
        transform.zoom(false, pointF);

        // Ensure the up action is not run
        currentTrackballLongPressTimeOut = null;
      }
    }
  }
}
