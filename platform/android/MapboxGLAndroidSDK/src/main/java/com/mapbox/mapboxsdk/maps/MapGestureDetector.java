package com.mapbox.mapboxsdk.maps;

import android.content.Context;
import android.graphics.PointF;
import android.location.Location;
import android.os.Handler;
import android.support.annotation.Nullable;
import android.support.v4.view.GestureDetectorCompat;
import android.support.v4.view.ScaleGestureDetectorCompat;
import android.view.InputDevice;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.VelocityTracker;
import android.view.ViewConfiguration;

import com.almeros.android.multitouch.gesturedetectors.RotateGestureDetector;
import com.almeros.android.multitouch.gesturedetectors.ShoveGestureDetector;
import com.almeros.android.multitouch.gesturedetectors.TwoFingerGestureDetector;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.services.android.telemetry.MapboxEvent;
import com.mapbox.services.android.telemetry.MapboxTelemetry;
import com.mapbox.services.android.telemetry.utils.MathUtils;
import com.mapbox.services.android.telemetry.utils.TelemetryUtils;

import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveStartedListener.REASON_API_GESTURE;

/**
 * Manages gestures events on a MapView.
 * <p>
 * Relies on gesture detection code in almeros.android.multitouch.gesturedetectors.
 * </p>
 */
final class MapGestureDetector {

  private final Transform transform;
  private final Projection projection;
  private final UiSettings uiSettings;
  private final TrackingSettings trackingSettings;
  private final AnnotationManager annotationManager;
  private final CameraChangeDispatcher cameraChangeDispatcher;

  private final GestureDetectorCompat gestureDetector;
  private final ScaleGestureDetector scaleGestureDetector;
  private final RotateGestureDetector rotateGestureDetector;
  private final ShoveGestureDetector shoveGestureDetector;

  private MapboxMap.OnMapClickListener onMapClickListener;
  private MapboxMap.OnMapLongClickListener onMapLongClickListener;
  private MapboxMap.OnFlingListener onFlingListener;
  private MapboxMap.OnScrollListener onScrollListener;

  private PointF focalPoint;

  private boolean twoTap;
  private boolean quickZoom;
  private boolean tiltGestureOccurred;
  private boolean scrollGestureOccurred;

  private boolean scaleGestureOccurred;
  private boolean recentScaleGestureOccurred;
  private long scaleBeginTime;

  private VelocityTracker velocityTracker = null;
  private boolean wasZoomingIn = false;
  private final Handler handler = new Handler();

  MapGestureDetector(Context context, Transform transform, Projection projection, UiSettings uiSettings,
                     TrackingSettings trackingSettings, AnnotationManager annotationManager,
                     CameraChangeDispatcher cameraChangeDispatcher) {
    this.annotationManager = annotationManager;
    this.transform = transform;
    this.projection = projection;
    this.uiSettings = uiSettings;
    this.trackingSettings = trackingSettings;
    this.cameraChangeDispatcher = cameraChangeDispatcher;

    // Touch gesture detectors
    gestureDetector = new GestureDetectorCompat(context, new GestureListener());
    gestureDetector.setIsLongpressEnabled(true);
    scaleGestureDetector = new ScaleGestureDetector(context, new ScaleGestureListener());
    ScaleGestureDetectorCompat.setQuickScaleEnabled(scaleGestureDetector, true);
    rotateGestureDetector = new RotateGestureDetector(context, new RotateGestureListener());
    shoveGestureDetector = new ShoveGestureDetector(context, new ShoveGestureListener());
  }

  /**
   * Set the gesture focal point.
   * <p>
   * this is the center point used for calculate transformations from gestures, value is
   * overridden if end user provides his own through {@link UiSettings#setFocalPoint(PointF)}.
   * </p>
   *
   * @param focalPoint the center point for gestures
   */
  void setFocalPoint(PointF focalPoint) {
    if (focalPoint == null) {
      // resetting focal point,
      if (uiSettings.getFocalPoint() != null) {
        // using user provided one to reset
        focalPoint = uiSettings.getFocalPoint();
      }
    }
    this.focalPoint = focalPoint;
  }

  /**
   * Get the current active gesture focal point.
   * <p>
   * This could be either the user provided focal point in {@link UiSettings#setFocalPoint(PointF)} or the focal point
   * defined as a result of {@link TrackingSettings#setMyLocationEnabled(boolean)}.
   * </p>
   *
   * @return the current active gesture focal point.
   */
  @Nullable
  PointF getFocalPoint() {
    return focalPoint;
  }

  /**
   * Given coordinates from a gesture, use the current projection to translate it into
   * a Location object.
   *
   * @param x coordinate
   * @param y coordinate
   * @return location
   */
  private Location getLocationFromGesture(float x, float y) {
    LatLng latLng = projection.fromScreenLocation(new PointF(x, y));
    return TelemetryUtils.buildLocation(latLng.getLongitude(), latLng.getLatitude());
  }

  /**
   * Called when user touches the screen, all positions are absolute.
   * <p>
   * Forwards event to the related gesture detectors.
   * </p>
   *
   * @param event the MotionEvent
   * @return True if touch event is handled
   */
  boolean onTouchEvent(MotionEvent event) {
    // framework can return null motion events in edge cases #9432
    if (event == null) {
      return false;
    }

    // Check and ignore non touch or left clicks
    if ((event.getButtonState() != 0) && (event.getButtonState() != MotionEvent.BUTTON_PRIMARY)) {
      return false;
    }

    // Check two finger gestures first
    scaleGestureDetector.onTouchEvent(event);
    rotateGestureDetector.onTouchEvent(event);
    shoveGestureDetector.onTouchEvent(event);

    // Handle two finger tap
    switch (event.getActionMasked()) {
      case MotionEvent.ACTION_DOWN:
        if (velocityTracker == null) {
          velocityTracker = VelocityTracker.obtain();
        } else {
          velocityTracker.clear();
        }
        velocityTracker.addMovement(event);
        // First pointer down, reset scaleGestureOccurred, used to avoid triggering a fling after a scale gesture #7666
        recentScaleGestureOccurred = false;
        transform.setGestureInProgress(true);
        break;

      case MotionEvent.ACTION_POINTER_DOWN:
        // Second pointer down
        twoTap = event.getPointerCount() == 2
          && uiSettings.isZoomGesturesEnabled();
        if (twoTap) {
          // Confirmed 2nd Finger Down
          MapboxTelemetry.getInstance().pushEvent(MapboxEventWrapper.buildMapClickEvent(
            getLocationFromGesture(event.getX(), event.getY()),
            MapboxEvent.GESTURE_TWO_FINGER_SINGLETAP, transform));
        }
        break;

      case MotionEvent.ACTION_POINTER_UP:
        // Second pointer up
        break;

      case MotionEvent.ACTION_UP:
        // First pointer up
        long tapInterval = event.getEventTime() - event.getDownTime();
        boolean isTap = tapInterval <= ViewConfiguration.getTapTimeout();
        boolean inProgress = rotateGestureDetector.isInProgress()
          || scaleGestureDetector.isInProgress()
          || shoveGestureDetector.isInProgress()
          || scaleGestureOccurred;

        if (twoTap && isTap && !inProgress) {
          if (focalPoint != null) {
            transform.zoom(false, focalPoint);
          } else {
            PointF focalPoint = TwoFingerGestureDetector.determineFocalPoint(event);
            transform.zoom(false, focalPoint);
          }
          twoTap = false;
          return true;
        }

        // Scroll / Pan Has Stopped
        if (scrollGestureOccurred) {
          MapboxTelemetry.getInstance().pushEvent(MapboxEventWrapper.buildMapDragEndEvent(
            getLocationFromGesture(event.getX(), event.getY()), transform));
          scrollGestureOccurred = false;
          cameraChangeDispatcher.onCameraIdle();
        }

        twoTap = false;
        transform.setGestureInProgress(false);
        if (velocityTracker != null) {
          velocityTracker.recycle();
        }
        velocityTracker = null;
        break;

      case MotionEvent.ACTION_CANCEL:
        twoTap = false;
        transform.setGestureInProgress(false);
        if (velocityTracker != null) {
          velocityTracker.recycle();
        }
        velocityTracker = null;
        break;
      case MotionEvent.ACTION_MOVE:
        velocityTracker.addMovement(event);
        velocityTracker.computeCurrentVelocity(1000);
        break;
    }

    return gestureDetector.onTouchEvent(event);
  }

  /**
   * Called for events that don't fit the other handlers.
   * <p>
   * Examples of such events are mouse scroll events, mouse moves, joystick & trackpad.
   * </p>
   *
   * @param event The MotionEvent occured
   * @return True is the event is handled
   */
  boolean onGenericMotionEvent(MotionEvent event) {
    // Mouse events
    // if (event.isFromSource(InputDevice.SOURCE_CLASS_POINTER)) { // this is not available before API 18
    if ((event.getSource() & InputDevice.SOURCE_CLASS_POINTER) == InputDevice.SOURCE_CLASS_POINTER) {
      // Choose the action
      switch (event.getActionMasked()) {
        // Mouse scrolls
        case MotionEvent.ACTION_SCROLL:
          if (!uiSettings.isZoomGesturesEnabled()) {
            return false;
          }

          // Cancel any animation
          transform.cancelTransitions();

          // Get the vertical scroll amount, one click = 1
          float scrollDist = event.getAxisValue(MotionEvent.AXIS_VSCROLL);

          // Scale the map by the appropriate power of two factor
          transform.zoomBy(scrollDist, event.getX(), event.getY());

          return true;

        default:
          // We are not interested in this event
          return false;
      }
    }

    // We are not interested in this event
    return false;
  }


  /**
   * Responsible for handling one finger gestures.
   */
  private class GestureListener extends android.view.GestureDetector.SimpleOnGestureListener {

    @Override
    public boolean onDown(MotionEvent event) {
      return true;
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent e) {
      if (!uiSettings.isZoomGesturesEnabled() || !uiSettings.isDoubleTapGesturesEnabled()) {
        return false;
      }

      switch (e.getAction()) {
        case MotionEvent.ACTION_DOWN:
          break;
        case MotionEvent.ACTION_MOVE:
          break;
        case MotionEvent.ACTION_UP:
          if (quickZoom) {
            cameraChangeDispatcher.onCameraIdle();
            quickZoom = false;
            break;
          }

          // notify camera change listener
          cameraChangeDispatcher.onCameraMoveStarted(REASON_API_GESTURE);

          // Single finger double tap
          if (focalPoint != null) {
            // User provided focal point
            transform.zoom(true, focalPoint);
          } else {
            // Zoom in on gesture
            transform.zoom(true, new PointF(e.getX(), e.getY()));
          }
          break;
      }

      MapboxTelemetry.getInstance().pushEvent(MapboxEventWrapper.buildMapClickEvent(
        getLocationFromGesture(e.getX(), e.getY()),
        MapboxEvent.GESTURE_DOUBLETAP, transform));

      return true;
    }

    @Override
    public boolean onSingleTapUp(MotionEvent motionEvent) {
      // Cancel any animation
      transform.cancelTransitions();
      return true;
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent motionEvent) {
      PointF tapPoint = new PointF(motionEvent.getX(), motionEvent.getY());
      boolean tapHandled = annotationManager.onTap(tapPoint);

      if (!tapHandled) {
        if (uiSettings.isDeselectMarkersOnTap()) {
          // deselect any selected marker
          annotationManager.deselectMarkers();
        }

        // notify app of map click
        if (onMapClickListener != null) {
          onMapClickListener.onMapClick(projection.fromScreenLocation(tapPoint));
        }
      }

      MapboxTelemetry.getInstance().pushEvent(MapboxEventWrapper.buildMapClickEvent(
        getLocationFromGesture(motionEvent.getX(), motionEvent.getY()),
        MapboxEvent.GESTURE_SINGLETAP, transform));

      return true;
    }

    @Override
    public void onLongPress(MotionEvent motionEvent) {
      if (onMapLongClickListener != null && !quickZoom) {
        onMapLongClickListener.onMapLongClick(
          projection.fromScreenLocation(new PointF(motionEvent.getX(), motionEvent.getY())));
      }
    }

    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
      if ((!trackingSettings.isScrollGestureCurrentlyEnabled()) || recentScaleGestureOccurred) {
        // don't allow a fling is scroll is disabled
        // and ignore when a scale gesture has occurred
        return false;
      }

      float screenDensity = uiSettings.getPixelRatio();

      // calculate velocity vector for xy dimensions, independent from screen size
      double velocityXY = Math.hypot(velocityX / screenDensity, velocityY / screenDensity);
      if (velocityXY < MapboxConstants.VELOCITY_THRESHOLD_IGNORE_FLING) {
        // ignore short flings, these can occur when other gestures just have finished executing
        return false;
      }

      trackingSettings.resetTrackingModesIfRequired(true, false, false);

      // cancel any animation
      transform.cancelTransitions();

      cameraChangeDispatcher.onCameraMoveStarted(REASON_API_GESTURE);

      // tilt results in a bigger translation, limiting input for #5281
      double tilt = transform.getTilt();
      double tiltFactor = 1.5 + ((tilt != 0) ? (tilt / 10) : 0);
      double offsetX = velocityX / tiltFactor / screenDensity;
      double offsetY = velocityY / tiltFactor / screenDensity;

      // calculate animation time based on displacement
      long animationTime = (long) (velocityXY / 7 / tiltFactor + MapboxConstants.ANIMATION_DURATION_FLING_BASE);

      // update transformation
      transform.moveBy(offsetX, offsetY, animationTime);

      if (onFlingListener != null) {
        onFlingListener.onFling();
      }
      return true;
    }

    // Called for drags
    @Override
    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
      if (!trackingSettings.isScrollGestureCurrentlyEnabled()) {
        return false;
      }

      if (tiltGestureOccurred) {
        return false;
      }

      if (!scrollGestureOccurred) {
        scrollGestureOccurred = true;

        // Cancel any animation
        if (!scaleGestureOccurred) {
          transform.cancelTransitions();
          cameraChangeDispatcher.onCameraMoveStarted(REASON_API_GESTURE);
        }

        MapboxTelemetry.getInstance().pushEvent(MapboxEventWrapper.buildMapClickEvent(
          getLocationFromGesture(e1.getX(), e1.getY()),
          MapboxEvent.GESTURE_PAN_START, transform));
      }

      // reset tracking if needed
      trackingSettings.resetTrackingModesIfRequired(true, false, false);

      // Scroll the map
      transform.moveBy(-distanceX, -distanceY, 0 /*no duration*/);

      if (onScrollListener != null) {
        onScrollListener.onScroll();
      }
      return true;
    }
  }

  /**
   * Responsible for handling two finger gestures and double-tap drag gestures.
   */
  private class ScaleGestureListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {

    private float scaleFactor = 1.0f;

    // Called when two fingers first touch the screen
    @Override
    public boolean onScaleBegin(ScaleGestureDetector detector) {
      if (!uiSettings.isZoomGesturesEnabled()) {
        return false;
      }

      recentScaleGestureOccurred = true;
      scaleBeginTime = detector.getEventTime();
      MapboxTelemetry.getInstance().pushEvent(MapboxEventWrapper.buildMapClickEvent(
        getLocationFromGesture(detector.getFocusX(), detector.getFocusY()),
        MapboxEvent.GESTURE_PINCH_START, transform));
      return true;
    }

    // Called when fingers leave screen
    @Override
    public void onScaleEnd(ScaleGestureDetector detector) {
      double velocityXY = Math.abs(velocityTracker.getYVelocity()) + Math.abs(velocityTracker.getXVelocity());
      if (velocityXY > MapboxConstants.VELOCITY_THRESHOLD_IGNORE_FLING) {
        long animationTime = (long)(Math.log(velocityXY) * 66);
        transform.zoom(wasZoomingIn, new PointF(detector.getFocusX(), detector.getFocusY()), animationTime);
        handler.postDelayed(new Runnable() {
          @Override
          public void run() {
            scaleGestureOccurred = false;
          }
        }, animationTime);
      } else {
        scaleGestureOccurred = false;
        scaleBeginTime = 0;
        scaleFactor = 1.0f;
        cameraChangeDispatcher.onCameraIdle();
      }
    }

    // Called each time a finger moves
    // Called for pinch zooms and quickzooms/quickscales
    @Override
    public boolean onScale(ScaleGestureDetector detector) {
      if (!uiSettings.isZoomGesturesEnabled()) {
        return super.onScale(detector);
      }

      if (tiltGestureOccurred) {
        return false;
      }

      // Ignore short touches in case it is a tap
      // Also ignore small scales
      long time = detector.getEventTime();
      long interval = time - scaleBeginTime;
      if (!scaleGestureOccurred && (interval <= ViewConfiguration.getTapTimeout())) {
        return false;
      }

      // If scale is large enough ignore a tap
      scaleFactor *= detector.getScaleFactor();
      if ((scaleFactor > 1.05f) || (scaleFactor < 0.95f)) {
        // notify camera change listener
        cameraChangeDispatcher.onCameraMoveStarted(REASON_API_GESTURE);
        scaleGestureOccurred = true;
      }

      if (!scaleGestureOccurred) {
        return false;
      }

      // Gesture is a quickzoom if there aren't two fingers
      if (!quickZoom && !twoTap) {
        cameraChangeDispatcher.onCameraMoveStarted(REASON_API_GESTURE);
      }
      quickZoom = !twoTap;

      // make an assumption here; if the zoom center is specified by the gesture, it's NOT going
      // to be in the center of the map. Therefore the zoom will translate the map center, so tracking
      // should be disabled.

      trackingSettings.resetTrackingModesIfRequired(!quickZoom, false, false);
      // Scale the map
      wasZoomingIn = (Math.log(detector.getScaleFactor()) / Math.log(Math.PI / 2)) >= 0;
      if (focalPoint != null) {
        // arround user provided focal point
        transform.zoomBy(Math.log(detector.getScaleFactor()) / Math.log(Math.PI / 2), focalPoint.x, focalPoint.y);
      } else if (quickZoom) {
        cameraChangeDispatcher.onCameraMove();
        // clamp scale factors we feed to core #7514
        float scaleFactor = MathUtils.clamp(detector.getScaleFactor(),
          MapboxConstants.MINIMUM_SCALE_FACTOR_CLAMP,
          MapboxConstants.MAXIMUM_SCALE_FACTOR_CLAMP);
        // around center map
        transform.zoomBy(Math.log(scaleFactor) / Math.log(Math.PI / 2),
          uiSettings.getWidth() / 2, uiSettings.getHeight() / 2);
      } else {
        // around gesture
        transform.zoomBy(Math.log(detector.getScaleFactor()) / Math.log(Math.PI / 2),
          detector.getFocusX(), detector.getFocusY());
      }
      return true;
    }
  }

  /**
   * Responsible for handling rotation gestures.
   */
  private class RotateGestureListener extends RotateGestureDetector.SimpleOnRotateGestureListener {

    private static final long ROTATE_INVOKE_WAIT_TIME = 750;
    private static final float ROTATE_INVOKE_ANGLE = 17.5f;

    private long beginTime = 0;
    private boolean started = false;

    // Called when two fingers first touch the screen
    @Override
    public boolean onRotateBegin(RotateGestureDetector detector) {
      if (!trackingSettings.isRotateGestureCurrentlyEnabled()) {
        return false;
      }

      // notify camera change listener
      cameraChangeDispatcher.onCameraMoveStarted(REASON_API_GESTURE);

      beginTime = detector.getEventTime();
      return true;
    }

    // Called when the fingers leave the screen
    @Override
    public void onRotateEnd(RotateGestureDetector detector) {
      // notify camera change listener
      beginTime = 0;
      started = false;
    }

    // Called each time one of the two fingers moves
    // Called for rotation
    @Override
    public boolean onRotate(RotateGestureDetector detector) {
      if (!trackingSettings.isRotateGestureCurrentlyEnabled() || tiltGestureOccurred) {
        return false;
      }

      // Ignore short touches in case it is a tap
      // Also ignore small rotate
      long time = detector.getEventTime();
      long interval = time - beginTime;
      if (!started && (interval <= ViewConfiguration.getTapTimeout() || isScaleGestureActive(time))) {
        return false;
      }

      // If rotate is large enough ignore a tap
      // Also is zoom already started, don't rotate
      float angle = detector.getRotationDegreesDelta();
      if (Math.abs(angle) >= ROTATE_INVOKE_ANGLE) {
        MapboxTelemetry.getInstance().pushEvent(MapboxEventWrapper.buildMapClickEvent(
          getLocationFromGesture(detector.getFocusX(), detector.getFocusY()),
          MapboxEvent.GESTURE_ROTATION_START, transform));
        started = true;
      }

      if (!started) {
        return false;
      }

      // rotation constitutes translation of anything except the center of
      // rotation, so cancel both location and bearing tracking if required
      trackingSettings.resetTrackingModesIfRequired(true, true, false);

      // Calculate map bearing value
      double bearing = transform.getRawBearing() + angle;

      // Rotate the map
      if (focalPoint != null) {
        // User provided focal point
        transform.setBearing(bearing, focalPoint.x, focalPoint.y);
      } else {
        // around gesture
        transform.setBearing(bearing, detector.getFocusX(), detector.getFocusY());
      }
      return true;
    }

    private boolean isScaleGestureActive(long time) {
      long scaleExecutionTime = time - scaleBeginTime;
      boolean scaleGestureStarted = scaleBeginTime != 0;
      boolean scaleOffsetTimeValid = scaleExecutionTime > ROTATE_INVOKE_WAIT_TIME;
      return (scaleGestureStarted && scaleOffsetTimeValid) || scaleGestureOccurred;
    }
  }

  /**
   * Responsible for handling 2 finger shove gestures.
   */
  private class ShoveGestureListener implements ShoveGestureDetector.OnShoveGestureListener {

    private long beginTime = 0;
    private float totalDelta = 0.0f;

    @Override
    public boolean onShoveBegin(ShoveGestureDetector detector) {
      if (!uiSettings.isTiltGesturesEnabled()) {
        return false;
      }

      // notify camera change listener
      cameraChangeDispatcher.onCameraMoveStarted(REASON_API_GESTURE);
      return true;
    }

    @Override
    public void onShoveEnd(ShoveGestureDetector detector) {
      beginTime = 0;
      totalDelta = 0.0f;
      tiltGestureOccurred = false;
    }

    @Override
    public boolean onShove(ShoveGestureDetector detector) {
      if (!uiSettings.isTiltGesturesEnabled()) {
        return false;
      }

      // Ignore short touches in case it is a tap
      // Also ignore small tilt
      long time = detector.getEventTime();
      long interval = time - beginTime;
      if (!tiltGestureOccurred && (interval <= ViewConfiguration.getTapTimeout())) {
        return false;
      }

      // If tilt is large enough ignore a tap
      // Also if zoom already started, don't tilt
      totalDelta += detector.getShovePixelsDelta();
      if (!tiltGestureOccurred && ((totalDelta > 10.0f) || (totalDelta < -10.0f))) {
        tiltGestureOccurred = true;
        beginTime = detector.getEventTime();
        MapboxTelemetry.getInstance().pushEvent(MapboxEventWrapper.buildMapClickEvent(
          getLocationFromGesture(detector.getFocusX(), detector.getFocusY()),
          MapboxEvent.GESTURE_PITCH_START, transform));
      }

      if (!tiltGestureOccurred) {
        return false;
      }

      // Get tilt value (scale and clamp)
      double pitch = transform.getTilt();
      pitch -= 0.1 * detector.getShovePixelsDelta();
      pitch = Math.max(MapboxConstants.MINIMUM_TILT, Math.min(MapboxConstants.MAXIMUM_TILT, pitch));

      // Tilt the map
      transform.setTilt(pitch);
      return true;
    }
  }

  void setOnMapClickListener(MapboxMap.OnMapClickListener onMapClickListener) {
    this.onMapClickListener = onMapClickListener;
  }

  void setOnMapLongClickListener(MapboxMap.OnMapLongClickListener onMapLongClickListener) {
    this.onMapLongClickListener = onMapLongClickListener;
  }

  void setOnFlingListener(MapboxMap.OnFlingListener onFlingListener) {
    this.onFlingListener = onFlingListener;
  }

  void setOnScrollListener(MapboxMap.OnScrollListener onScrollListener) {
    this.onScrollListener = onScrollListener;
  }
}
