package com.mapbox.mapboxsdk.maps;

import android.graphics.PointF;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.ViewConfiguration;

/**
 * Manages key events on a MapView.
 * <p>
 * <ul>
 * <li> Uses {@link Transform} to change the map state</li>
 * <li> Uses {@link UiSettings} to verify validity of user restricted movement.</li>
 * </ul>
 * <p>
 */
final class MapKeyListener {

  private final String TAG = "MapKeyListener";
  private final Transform transform;
  private final UiSettings uiSettings;
  private final MapGestureDetector mapGestureDetector;
  private int bearingIndex = 1;

  @Nullable
  private TrackballLongPressTimeOut currentTrackballLongPressTimeOut;

  MapKeyListener(Transform transform, UiSettings uiSettings, MapGestureDetector mapGestureDetector) {
    this.transform = transform;
    this.uiSettings = uiSettings;
    this.mapGestureDetector = mapGestureDetector;
  }

  /**
   * Called when the user presses a key, also called for repeated keys held down.
   *
   * @param keyCode the id of the pressed key
   * @param event   the related key event
   * @return true if the event is handled
   */
  boolean onKeyDown(int keyCode, @NonNull KeyEvent event) {
    // If the user has held the scroll key down for a while then accelerate
    // the map camera movement speeds
    double scrollDist = event.getRepeatCount() >= 5 ? 90.0 : 50.0;
    double pitchDist = event.getRepeatCount() >= 5 ? 5.00 : 1.0;
    double rotateDist = event.getRepeatCount() >= 5 ? 20.0 : 1.0;

    // Check which key was pressed via hardware/real key code
    switch (keyCode) {
      // Tell the system to track these keys for long presses on
      // onKeyLongPress is fired
      case KeyEvent.KEYCODE_ENTER:
      case KeyEvent.KEYCODE_DPAD_CENTER:
        event.startTracking();
        return true;

      case KeyEvent.KEYCODE_DPAD_LEFT:
        if (!uiSettings.isScrollGesturesEnabled()) {
          return false;
        }

        // Cancel any animation
        transform.cancelTransitions();

        // Check if the shift key is being held down
        if (event.isShiftPressed()) {
          if (!uiSettings.isRotateGesturesEnabled()) {
            return false;
          }
          transform.setBearing(bearingIndex * rotateDist);
          Log.d(TAG, "transform.getBearing() = " + transform.getBearing());
          bearingIndex++;
        } else {
          // Move map camera left
          transform.moveBy(scrollDist, 0.0, 0 /*no animation*/);
        }
        return true;

      case KeyEvent.KEYCODE_DPAD_RIGHT:
        if (!uiSettings.isScrollGesturesEnabled()) {
          return false;
        }

        // Cancel any animation
        transform.cancelTransitions();

        // Check if the shift key is being held down
        if (event.isShiftPressed()) {
          if (!uiSettings.isRotateGesturesEnabled()) {
            return false;
          }
          transform.setBearing(180 - (bearingIndex * rotateDist));
          Log.d(TAG, "transform.getBearing() = " + transform.getBearing());
          bearingIndex++;
        } else {
          // Move map camera right
          transform.moveBy(-scrollDist, 0.0, 0 /*no animation*/);
        }

        return true;

      case KeyEvent.KEYCODE_DPAD_UP:
        if (!uiSettings.isScrollGesturesEnabled()) {
          return false;
        }

        // Cancel any animation
        transform.cancelTransitions();

        // Check if the shift key is being held down
        if (event.isShiftPressed()) {
          if (!uiSettings.isTiltGesturesEnabled()) {
            return false;
          }

          // Increase map camera pitch value
          transform.setTilt(transform.getTilt() + pitchDist);

        } else {
          // Move map camera target up
          transform.moveBy(0.0, scrollDist, 0 /*no animation*/);
        }

        return true;

      case KeyEvent.KEYCODE_DPAD_DOWN:
        if (!uiSettings.isScrollGesturesEnabled()) {
          return false;
        }

        // Cancel any animation
        transform.cancelTransitions();

        // Check if the shift key is being held down
        if (event.isShiftPressed()) {
          if (!uiSettings.isTiltGesturesEnabled()) {
            return false;
          }

          // Decrease map camera pitch value
          transform.setTilt(transform.getTilt() - pitchDist);

        } else {
          // Move map target down
          transform.moveBy(0.0, -scrollDist, 0 /*no animation*/);
        }

        return true;

      case KeyEvent.KEYCODE_EQUALS:

        // We're interested in the equals sign because most keyboards have the
        // + and = symbols on the same key. KeyEvent.KEYCODE_EQUALS fires when
        // this key is tapped.

        // Cancel any animation
        transform.cancelTransitions();

        if (!uiSettings.isZoomGesturesEnabled()) {
          return false;
        }
        // Zoom the map camera closer to the map (i.e. increase the map camera zoom level number)
        zoomMapCameraIn();

        return true;

      case KeyEvent.KEYCODE_MINUS:

        // Cancel any animation
        transform.cancelTransitions();

        if (!uiSettings.isZoomGesturesEnabled()) {
          return false;
        }

        // Pull the map camera out away from the map (i.e. decrease the map camera zoom level number)
        zoomMapCameraOut();

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
        zoomMapCameraOut();
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
        zoomMapCameraIn();
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
        if (!uiSettings.isScrollGesturesEnabled()) {
          return false;
        }

        // Cancel any animation
        transform.cancelTransitions();

        // Scroll the map
        zoomMapCameraIn();
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
          zoomMapCameraIn();
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
        zoomMapCameraOut();

        // Ensure the up action is not run
        currentTrackballLongPressTimeOut = null;
      }
    }
  }

  /**
   * Moves the map camera towards from the map plane.
   */
  private void zoomMapCameraIn() {
    PointF focalPoint = new PointF(uiSettings.getWidth() / 2, uiSettings.getHeight() / 2);
    mapGestureDetector.zoomInAnimated(focalPoint, true);
  }

  /**
   * Moves the map camera away from the map plane
   */
  private void zoomMapCameraOut() {
    PointF focalPoint = new PointF(uiSettings.getWidth() / 2, uiSettings.getHeight() / 2);
    mapGestureDetector.zoomOutAnimated(focalPoint, true);
  }
}
