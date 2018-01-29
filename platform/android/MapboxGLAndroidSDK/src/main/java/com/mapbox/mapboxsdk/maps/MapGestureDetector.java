package com.mapbox.mapboxsdk.maps;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.ValueAnimator;
import android.content.Context;
import android.graphics.PointF;
import android.support.annotation.Nullable;
import android.support.v4.view.GestureDetectorCompat;
import android.support.v4.view.ScaleGestureDetectorCompat;
import android.support.v4.view.animation.FastOutSlowInInterpolator;
import android.view.InputDevice;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.VelocityTracker;
import android.view.ViewConfiguration;

import com.almeros.android.multitouch.gesturedetectors.RotateGestureDetector;
import com.almeros.android.multitouch.gesturedetectors.ShoveGestureDetector;
import com.almeros.android.multitouch.gesturedetectors.TwoFingerGestureDetector;
import com.mapbox.android.telemetry.Event;
import com.mapbox.android.telemetry.MapEventFactory;
import com.mapbox.android.telemetry.MapState;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.MathUtils;

import java.util.concurrent.CopyOnWriteArrayList;

import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveStartedListener.REASON_API_ANIMATION;
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

  private GestureDetectorCompat gestureDetector;
  private ScaleGestureDetector scaleGestureDetector;
  private RotateGestureDetector rotateGestureDetector;
  private ShoveGestureDetector shoveGestureDetector;

  // deprecated map touch API
  private MapboxMap.OnMapClickListener onMapClickListener;
  private MapboxMap.OnMapLongClickListener onMapLongClickListener;
  private MapboxMap.OnFlingListener onFlingListener;
  private MapboxMap.OnScrollListener onScrollListener;

  // new map touch API
  private final CopyOnWriteArrayList<MapboxMap.OnMapClickListener> onMapClickListenerList
    = new CopyOnWriteArrayList<>();

  private final CopyOnWriteArrayList<MapboxMap.OnMapLongClickListener> onMapLongClickListenerList
    = new CopyOnWriteArrayList<>();

  private final CopyOnWriteArrayList<MapboxMap.OnFlingListener> onFlingListenerList
    = new CopyOnWriteArrayList<>();

  private final CopyOnWriteArrayList<MapboxMap.OnScrollListener> onScrollListenerList
    = new CopyOnWriteArrayList<>();

  private PointF focalPoint;

  private boolean twoTap;
  private boolean quickZoom;
  private boolean tiltGestureOccurred;
  private boolean scrollGestureOccurred;

  private boolean scaleGestureOccurred;
  private boolean recentScaleGestureOccurred;
  private long scaleBeginTime;

  private boolean scaleAnimating;
  private boolean rotateAnimating;

  private VelocityTracker velocityTracker;
  private boolean wasZoomingIn;
  private boolean wasClockwiseRotating;
  private boolean rotateGestureOccurred;

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
    if (context != null) {
      gestureDetector = new GestureDetectorCompat(context, new GestureListener());
      gestureDetector.setIsLongpressEnabled(true);
      scaleGestureDetector = new ScaleGestureDetector(context, new ScaleGestureListener());
      ScaleGestureDetectorCompat.setQuickScaleEnabled(scaleGestureDetector, true);
      rotateGestureDetector = new RotateGestureDetector(context, new RotateGestureListener());
      shoveGestureDetector = new ShoveGestureDetector(context, new ShoveGestureListener());
    }
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
          if (isZoomValid(transform)) {
            MapEventFactory mapEventFactory = new MapEventFactory();
            LatLng latLng = projection.fromScreenLocation(new PointF(event.getX(), event.getY()));
            MapState twoFingerTap = new MapState(latLng.getLatitude(), latLng.getLongitude(), transform.getZoom());
            twoFingerTap.setGesture(Events.TWO_FINGER_TAP);
            Events.obtainTelemetry().push(mapEventFactory.createMapGestureEvent(Event.Type.MAP_CLICK, twoFingerTap));
          }
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
          || shoveGestureDetector.isInProgress();

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
          if (isZoomValid(transform)) {
            MapEventFactory mapEventFactory = new MapEventFactory();
            LatLng latLng = projection.fromScreenLocation(new PointF(event.getX(), event.getY()));
            MapState dragend = new MapState(latLng.getLatitude(), latLng.getLongitude(), transform.getZoom());
            Events.obtainTelemetry().push(mapEventFactory.createMapGestureEvent(Event.Type.MAP_DRAGEND, dragend));
          }
          scrollGestureOccurred = false;

          if (!scaleAnimating && !rotateAnimating) {
            cameraChangeDispatcher.onCameraIdle();
          }
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
        if (velocityTracker != null) {
          velocityTracker.addMovement(event);
          velocityTracker.computeCurrentVelocity(1000);
        }
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
          if (isZoomValid(transform)) {
            MapEventFactory mapEventFactory = new MapEventFactory();
            LatLng latLng = projection.fromScreenLocation(new PointF(e.getX(), e.getY()));
            MapState doubleTap = new MapState(latLng.getLatitude(), latLng.getLongitude(), transform.getZoom());
            doubleTap.setGesture(Events.DOUBLE_TAP);
            Events.obtainTelemetry().push(mapEventFactory.createMapGestureEvent(Event.Type.MAP_CLICK, doubleTap));
          }
          break;
      }

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

        notifyOnMapClickListeners(tapPoint);
      }

      if (isZoomValid(transform)) {
        MapEventFactory mapEventFactory = new MapEventFactory();
        LatLng latLng = projection.fromScreenLocation(new PointF(motionEvent.getX(), motionEvent.getY()));
        MapState singleTap = new MapState(latLng.getLatitude(), latLng.getLongitude(), transform.getZoom());
        singleTap.setGesture(Events.SINGLE_TAP);
        Events.obtainTelemetry().push(mapEventFactory.createMapGestureEvent(Event.Type.MAP_CLICK, singleTap));
      }

      return true;
    }

    @Override
    public void onLongPress(MotionEvent motionEvent) {
      PointF longClickPoint = new PointF(motionEvent.getX(), motionEvent.getY());

      if (!quickZoom) {
        notifyOnMapLongClickListeners(longClickPoint);
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

      notifyOnFlingListeners();
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

        if (isZoomValid(transform)) {
          MapEventFactory mapEventFactory = new MapEventFactory();
          LatLng latLng = projection.fromScreenLocation(new PointF(e1.getX(), e1.getY()));
          MapState pan = new MapState(latLng.getLatitude(), latLng.getLongitude(), transform.getZoom());
          pan.setGesture(Events.PAN);
          Events.obtainTelemetry().push(mapEventFactory.createMapGestureEvent(Event.Type.MAP_CLICK, pan));
        }
      }

      // reset tracking if needed
      trackingSettings.resetTrackingModesIfRequired(true, false, false);

      // Scroll the map
      transform.moveBy(-distanceX, -distanceY, 0 /*no duration*/);

      notifyOnScrollListeners();
      return true;
    }
  }

  void notifyOnMapClickListeners(PointF tapPoint) {
    // deprecated API
    if (onMapClickListener != null) {
      onMapClickListener.onMapClick(projection.fromScreenLocation(tapPoint));
    }

    // new API
    for (MapboxMap.OnMapClickListener listener : onMapClickListenerList) {
      listener.onMapClick(projection.fromScreenLocation(tapPoint));
    }
  }

  void notifyOnMapLongClickListeners(PointF longClickPoint) {
    // deprecated API
    if (onMapLongClickListener != null) {
      onMapLongClickListener.onMapLongClick(projection.fromScreenLocation(longClickPoint));
    }

    // new API
    for (MapboxMap.OnMapLongClickListener listener : onMapLongClickListenerList) {
      listener.onMapLongClick(projection.fromScreenLocation(longClickPoint));
    }
  }

  void notifyOnFlingListeners() {
    // deprecated API
    if (onFlingListener != null) {
      onFlingListener.onFling();
    }

    // new API
    for (MapboxMap.OnFlingListener listener : onFlingListenerList) {
      listener.onFling();
    }
  }

  void notifyOnScrollListeners() {
    //deprecated API
    if (onScrollListener != null) {
      onScrollListener.onScroll();
    }

    // new API
    for (MapboxMap.OnScrollListener listener : onScrollListenerList) {
      listener.onScroll();
    }
  }

  /**
   * Responsible for handling two finger gestures and double-tap drag gestures.
   */
  private class ScaleGestureListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {

    private static final int ANIMATION_TIME_MULTIPLIER = 77;
    private static final double ZOOM_DISTANCE_DIVIDER = 5;

    private float scaleFactor = 1.0f;
    private PointF scalePointBegin;

    // Called when two fingers first touch the screen
    @Override
    public boolean onScaleBegin(ScaleGestureDetector detector) {
      if (!uiSettings.isZoomGesturesEnabled()) {
        return false;
      }

      recentScaleGestureOccurred = true;
      scalePointBegin = new PointF(detector.getFocusX(), detector.getFocusY());
      scaleBeginTime = detector.getEventTime();
      if (isZoomValid(transform)) {
        MapEventFactory mapEventFactory = new MapEventFactory();
        LatLng latLng = projection.fromScreenLocation(new PointF(detector.getFocusX(), detector.getFocusY()));
        MapState pinch = new MapState(latLng.getLatitude(), latLng.getLongitude(), transform.getZoom());
        pinch.setGesture(Events.PINCH);
        Events.obtainTelemetry().push(mapEventFactory.createMapGestureEvent(Event.Type.MAP_CLICK, pinch));
      }
      return true;
    }

    // Called each time a finger moves
    // Called for pinch zooms and quickzooms/quickscales
    @Override
    public boolean onScale(ScaleGestureDetector detector) {
      if (!uiSettings.isZoomGesturesEnabled()) {
        return super.onScale(detector);
      }

      wasZoomingIn = (Math.log(detector.getScaleFactor()) / Math.log(Math.PI / 2)) > 0;
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
      if ((scaleFactor > 1.1f) || (scaleFactor < 0.9f)) {
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
      if (focalPoint != null) {
        // arround user provided focal point
        transform.zoomBy(Math.log(detector.getScaleFactor()) / Math.log(Math.PI / 2), focalPoint.x, focalPoint.y);
      } else if (quickZoom) {
        cameraChangeDispatcher.onCameraMove();
        // clamp scale factors we feed to core #7514
        float scaleFactor = detector.getScaleFactor();
        // around center map
        double zoomBy = Math.log(scaleFactor) / Math.log(Math.PI / 2);
        boolean negative = zoomBy < 0;
        zoomBy = MathUtils.clamp(Math.abs(zoomBy),
          MapboxConstants.MINIMUM_SCALE_FACTOR_CLAMP,
          MapboxConstants.MAXIMUM_SCALE_FACTOR_CLAMP);
        transform.zoomBy(negative ? -zoomBy : zoomBy, uiSettings.getWidth() / 2, uiSettings.getHeight() / 2);
        recentScaleGestureOccurred = true;
      } else {
        // around gesture
        transform.zoomBy(Math.log(detector.getScaleFactor()) / Math.log(Math.PI / 2),
          scalePointBegin.x, scalePointBegin.y);
      }
      return true;
    }

    // Called when fingers leave screen
    @Override
    public void onScaleEnd(final ScaleGestureDetector detector) {
      if (velocityTracker == null) {
        return;
      }

      if (rotateGestureOccurred || quickZoom) {
        reset();
        return;
      }

      double velocityXY = Math.abs(velocityTracker.getYVelocity()) + Math.abs(velocityTracker.getXVelocity());
      if (velocityXY > MapboxConstants.VELOCITY_THRESHOLD_IGNORE_FLING / 2) {
        scaleAnimating = true;
        double zoomAddition = calculateScale(velocityXY);
        double currentZoom = transform.getRawZoom();
        long animationTime = (long) (Math.log(velocityXY) * ANIMATION_TIME_MULTIPLIER);
        createScaleAnimator(currentZoom, zoomAddition, animationTime).start();
      } else if (!scaleAnimating) {
        reset();
      }
    }

    private void reset() {
      scaleAnimating = false;
      scaleGestureOccurred = false;
      scaleBeginTime = 0;
      scaleFactor = 1.0f;
      cameraChangeDispatcher.onCameraIdle();
    }

    private double calculateScale(double velocityXY) {
      double zoomAddition = (float) (Math.log(velocityXY) / ZOOM_DISTANCE_DIVIDER);
      if (!wasZoomingIn) {
        zoomAddition = -zoomAddition;
      }
      return zoomAddition;
    }

    private Animator createScaleAnimator(double currentZoom, double zoomAddition, long animationTime) {
      ValueAnimator animator = ValueAnimator.ofFloat((float) currentZoom, (float) (currentZoom + zoomAddition));
      animator.setDuration(animationTime);
      animator.setInterpolator(new FastOutSlowInInterpolator());
      animator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {

        @Override
        public void onAnimationUpdate(ValueAnimator animation) {
          transform.setZoom((Float) animation.getAnimatedValue(), scalePointBegin, 0, true);
        }
      });
      animator.addListener(new AnimatorListenerAdapter() {

        @Override
        public void onAnimationStart(Animator animation) {
          cameraChangeDispatcher.onCameraMoveStarted(REASON_API_ANIMATION);
        }

        @Override
        public void onAnimationCancel(Animator animation) {
          reset();
        }

        @Override
        public void onAnimationEnd(Animator animation) {
          reset();
        }
      });
      return animator;
    }
  }

  /**
   * Responsible for handling rotation gestures.
   */
  private class RotateGestureListener extends RotateGestureDetector.SimpleOnRotateGestureListener {

    private static final float ROTATE_INVOKE_ANGLE = 15.30f;
    private static final float ROTATE_LIMITATION_ANGLE = 3.35f;
    private static final float ROTATE_LIMITATION_DURATION = ROTATE_LIMITATION_ANGLE * 1.85f;

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

    // Called each time one of the two fingers moves
    // Called for rotation
    @Override
    public boolean onRotate(RotateGestureDetector detector) {
      if (!trackingSettings.isRotateGestureCurrentlyEnabled() || tiltGestureOccurred) {
        return false;
      }

      // If rotate is large enough ignore a tap
      // Also is zoom already started, don't rotate
      float angle = detector.getRotationDegreesDelta();
      if (Math.abs(angle) >= ROTATE_INVOKE_ANGLE) {
        if (isZoomValid(transform)) {
          MapEventFactory mapEventFactory = new MapEventFactory();
          LatLng latLng = projection.fromScreenLocation(new PointF(detector.getFocusX(), detector.getFocusY()));
          MapState rotation = new MapState(latLng.getLatitude(), latLng.getLongitude(), transform.getZoom());
          rotation.setGesture(Events.ROTATION);
          Events.obtainTelemetry().push(mapEventFactory.createMapGestureEvent(Event.Type.MAP_CLICK, rotation));
        }
        started = true;
      }

      if (!started) {
        return false;
      }

      wasClockwiseRotating = detector.getRotationDegreesDelta() > 0;
      if (scaleBeginTime != 0) {
        rotateGestureOccurred = true;
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

    // Called when the fingers leave the screen
    @Override
    public void onRotateEnd(RotateGestureDetector detector) {
      long interval = detector.getEventTime() - beginTime;
      if ((!started && (interval <= ViewConfiguration.getTapTimeout())) || scaleAnimating || interval > 500) {
        reset();
        return;
      }

      double angularVelocity = calculateVelocityVector(detector);
      if (Math.abs(angularVelocity) > 0.001 && rotateGestureOccurred && !rotateAnimating) {
        animateRotateVelocity();
      } else if (!rotateAnimating) {
        reset();
      }
    }

    private void reset() {
      beginTime = 0;
      started = false;
      rotateAnimating = false;
      rotateGestureOccurred = false;

      if (!twoTap) {
        cameraChangeDispatcher.onCameraIdle();
      }
    }

    private void animateRotateVelocity() {
      rotateAnimating = true;
      double currentRotation = transform.getRawBearing();
      double rotateAdditionDegrees = calculateVelocityInDegrees();
      createAnimator(currentRotation, rotateAdditionDegrees).start();
    }

    private double calculateVelocityVector(RotateGestureDetector detector) {
      return ((detector.getFocusX() * velocityTracker.getYVelocity())
        + (detector.getFocusY() * velocityTracker.getXVelocity()))
        / (Math.pow(detector.getFocusX(), 2) + Math.pow(detector.getFocusY(), 2));
    }

    private double calculateVelocityInDegrees() {
      double angleRadians = Math.atan2(velocityTracker.getXVelocity(), velocityTracker.getYVelocity());
      double angle = angleRadians / (Math.PI / 180);
      if (angle <= 0) {
        angle += 360;
      }

      // limit the angle
      angle = angle / ROTATE_LIMITATION_ANGLE;

      // correct direction
      if (!wasClockwiseRotating) {
        angle = -angle;
      }

      return angle;
    }

    private Animator createAnimator(double currentRotation, double rotateAdditionDegrees) {
      ValueAnimator animator = ValueAnimator.ofFloat(
        (float) currentRotation,
        (float) (currentRotation + rotateAdditionDegrees)
      );
      animator.setDuration((long) (Math.abs(rotateAdditionDegrees) * ROTATE_LIMITATION_DURATION));
      animator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
        @Override
        public void onAnimationUpdate(ValueAnimator animation) {
          transform.setBearing((Float) animation.getAnimatedValue());
        }
      });
      animator.addListener(new AnimatorListenerAdapter() {

        @Override
        public void onAnimationStart(Animator animation) {
          cameraChangeDispatcher.onCameraMoveStarted(REASON_API_ANIMATION);
        }

        @Override
        public void onAnimationCancel(Animator animation) {
          reset();
        }

        @Override
        public void onAnimationEnd(Animator animation) {
          reset();
        }
      });
      return animator;
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
        if (isZoomValid(transform)) {
          MapEventFactory mapEventFactory = new MapEventFactory();
          LatLng latLng = projection.fromScreenLocation(new PointF(detector.getFocusX(), detector.getFocusY()));
          MapState pitch = new MapState(latLng.getLatitude(), latLng.getLongitude(), transform.getZoom());
          pitch.setGesture(Events.PITCH);
          Events.obtainTelemetry().push(mapEventFactory.createMapGestureEvent(Event.Type.MAP_CLICK, pitch));
        }
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

  void addOnMapClickListener(MapboxMap.OnMapClickListener onMapClickListener) {
    onMapClickListenerList.add(onMapClickListener);
  }

  void removeOnMapClickListener(MapboxMap.OnMapClickListener onMapClickListener) {
    onMapClickListenerList.remove(onMapClickListener);
  }

  void addOnMapLongClickListener(MapboxMap.OnMapLongClickListener onMapLongClickListener) {
    onMapLongClickListenerList.add(onMapLongClickListener);
  }

  void removeOnMapLongClickListener(MapboxMap.OnMapLongClickListener onMapLongClickListener) {
    onMapLongClickListenerList.remove(onMapLongClickListener);
  }

  void addOnFlingListener(MapboxMap.OnFlingListener onFlingListener) {
    onFlingListenerList.add(onFlingListener);
  }

  void removeOnFlingListener(MapboxMap.OnFlingListener onFlingListener) {
    onFlingListenerList.remove(onFlingListener);
  }

  void addOnScrollListener(MapboxMap.OnScrollListener onScrollListener) {
    onScrollListenerList.add(onScrollListener);
  }

  void removeOnScrollListener(MapboxMap.OnScrollListener onScrollListener) {
    onScrollListenerList.remove(onScrollListener);
  }

  private boolean isZoomValid(Transform transform) {
    if (transform == null) {
      return false;
    }
    double mapZoom = transform.getZoom();
    if (mapZoom < MapboxConstants.MINIMUM_ZOOM || mapZoom > MapboxConstants.MAXIMUM_ZOOM) {
      return false;
    }
    return true;
  }
}
