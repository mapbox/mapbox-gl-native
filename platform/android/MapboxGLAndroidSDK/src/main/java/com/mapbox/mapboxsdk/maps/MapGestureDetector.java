package com.mapbox.mapboxsdk.maps;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.ValueAnimator;
import android.content.Context;
import android.graphics.PointF;
import android.os.Handler;
import android.support.annotation.Nullable;
import android.view.InputDevice;
import android.view.MotionEvent;
import android.view.animation.DecelerateInterpolator;

import com.mapbox.android.gestures.AndroidGesturesManager;
import com.mapbox.android.gestures.MoveGestureDetector;
import com.mapbox.android.gestures.MultiFingerTapGestureDetector;
import com.mapbox.android.gestures.RotateGestureDetector;
import com.mapbox.android.gestures.ShoveGestureDetector;
import com.mapbox.android.gestures.StandardGestureDetector;
import com.mapbox.android.gestures.StandardScaleGestureDetector;
import com.mapbox.android.telemetry.Event;
import com.mapbox.android.telemetry.MapEventFactory;
import com.mapbox.android.telemetry.MapState;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.MathUtils;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.CopyOnWriteArrayList;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveStartedListener.REASON_API_ANIMATION;
import static com.mapbox.mapboxsdk.maps.MapboxMap.OnCameraMoveStartedListener.REASON_API_GESTURE;

/**
 * Manages gestures events on a MapView.
 */
final class MapGestureDetector {

  private final Transform transform;
  private final Projection projection;
  private final UiSettings uiSettings;
  private final AnnotationManager annotationManager;
  private final CameraChangeDispatcher cameraChangeDispatcher;

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

  private final CopyOnWriteArrayList<MapboxMap.OnMoveListener> onMoveListenerList
    = new CopyOnWriteArrayList<>();

  private final CopyOnWriteArrayList<MapboxMap.OnRotateListener> onRotateListenerList
    = new CopyOnWriteArrayList<>();

  private final CopyOnWriteArrayList<MapboxMap.OnScaleListener> onScaleListenerList
    = new CopyOnWriteArrayList<>();

  private final CopyOnWriteArrayList<MapboxMap.OnShoveListener> onShoveListenerList
    = new CopyOnWriteArrayList<>();

  private PointF focalPoint;

  private AndroidGesturesManager gesturesManager;
  private boolean executeDoubleTap;

  private Animator scaleAnimator;
  private Animator rotateAnimator;
  private final List<Animator> scheduledAnimators = new ArrayList<>();
  private Handler mainHandler = new Handler();

  MapGestureDetector(Context context, Transform transform, Projection projection, UiSettings uiSettings,
                     AnnotationManager annotationManager, CameraChangeDispatcher cameraChangeDispatcher) {
    this.annotationManager = annotationManager;
    this.transform = transform;
    this.projection = projection;
    this.uiSettings = uiSettings;
    this.cameraChangeDispatcher = cameraChangeDispatcher;

    // Touch gesture detectors
    if (context != null) {
      gesturesManager = new AndroidGesturesManager(context);

      Set<Integer> set1 = new HashSet<>();
      set1.add(AndroidGesturesManager.GESTURE_TYPE_SHOVE);
      set1.add(AndroidGesturesManager.GESTURE_TYPE_SCALE);

      Set<Integer> set2 = new HashSet<>();
      set2.add(AndroidGesturesManager.GESTURE_TYPE_SHOVE);
      set2.add(AndroidGesturesManager.GESTURE_TYPE_ROTATE);

      Set<Integer> set3 = new HashSet<>();
      set3.add(AndroidGesturesManager.GESTURE_TYPE_SCALE);
      set3.add(AndroidGesturesManager.GESTURE_TYPE_LONG_PRESS);

      gesturesManager.setMutuallyExclusiveGestures(set1, set2, set3);

      gesturesManager.setStandardGestureListener(new StandardGestureListener());
      gesturesManager.setMoveGestureListener(new MoveGestureListener());
      gesturesManager.setStandardScaleGestureListener(new ScaleGestureListener(
        context.getResources().getDimension(R.dimen.mapbox_minimum_scale_velocity)
      ));
      gesturesManager.setRotateGestureListener(new RotateGestureListener(
        context.getResources().getDimension(R.dimen.mapbox_minimum_scale_span_when_rotating),
        context.getResources().getDimension(R.dimen.mapbox_minimum_angular_velocity)
      ));
      gesturesManager.setShoveGestureListener(new ShoveGestureListener());
      gesturesManager.setMultiFingerTapGestureListener(new TapGestureListener());
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
   * This could be either the user provided focal point in {@link UiSettings#setFocalPoint(PointF)} or <code>null</code>.
   * If it's <code>null</code>, gestures will use focal pointed returned by the detector.
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
   * @param motionEvent the MotionEvent
   * @return True if touch event is handled
   */
  boolean onTouchEvent(MotionEvent motionEvent) {
    // Framework can return null motion events in edge cases #9432
    if (motionEvent == null) {
      return false;
    }

    // Check and ignore non touch or left clicks
    if ((motionEvent.getButtonState() != 0) && (motionEvent.getButtonState() != MotionEvent.BUTTON_PRIMARY)) {
      return false;
    }

    boolean result = gesturesManager.onTouchEvent(motionEvent);

    switch (motionEvent.getActionMasked()) {
      case MotionEvent.ACTION_DOWN:
        cancelAnimators();
        transform.setGestureInProgress(true);
        break;
      case MotionEvent.ACTION_UP:
        transform.setGestureInProgress(false);

        if (executeDoubleTap(motionEvent)) {
          return true;
        }

        mainHandler.removeCallbacksAndMessages(null);
        for (Animator animator : scheduledAnimators) {
          animator.start();
        }
        scheduledAnimators.clear();
        break;

      case MotionEvent.ACTION_CANCEL:
        transform.setGestureInProgress(false);
        break;
    }

    return result;
  }

  private boolean executeDoubleTap(MotionEvent motionEvent) {
    if (!uiSettings.isZoomGesturesEnabled() || !executeDoubleTap) {
      return false;
    }

    transform.cancelTransitions();
    cameraChangeDispatcher.onCameraMoveStarted(REASON_API_GESTURE);

    // Single finger double tap
    if (focalPoint != null) {
      // User provided focal point
      transform.zoomIn(focalPoint);
    } else {
      // Zoom in on gesture
      transform.zoomIn(new PointF(motionEvent.getX(), motionEvent.getY()));
    }

    if (isZoomValid(transform)) {
      MapEventFactory mapEventFactory = new MapEventFactory();
      LatLng latLng = projection.fromScreenLocation(new PointF(motionEvent.getX(), motionEvent.getY()));
      MapState doubleTap = new MapState(latLng.getLatitude(), latLng.getLongitude(), transform.getZoom());
      doubleTap.setGesture(Events.DOUBLE_TAP);
      Events.obtainTelemetry().push(mapEventFactory.createMapGestureEvent(Event.Type.MAP_CLICK, doubleTap));
    }

    executeDoubleTap = false;
    return true;
  }

  private void cancelAnimators() {
    if (scaleAnimator != null) {
      scaleAnimator.cancel();
    }
    if (rotateAnimator != null) {
      rotateAnimator.cancel();
    }

    mainHandler.removeCallbacksAndMessages(null);
    scheduledAnimators.clear();
  }

  private Runnable cancelAnimatorsRunnable = new Runnable() {
    @Override
    public void run() {
      cancelAnimators();
    }
  };

  private void scheduleAnimator(Animator animator) {
    scheduledAnimators.add(animator);
    mainHandler.removeCallbacksAndMessages(null);
    mainHandler.postDelayed(cancelAnimatorsRunnable, 150);
  }

  /**
   * Called for events that don't fit the other handlers.
   * <p>
   * Examples of such events are mouse scroll events, mouse moves, joystick & trackpad.
   * </p>
   *
   * @param event The MotionEvent occurred
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
          transform.zoomBy(scrollDist, new PointF(event.getX(), event.getY()));

          return true;

        default:
          // We are not interested in this event
          return false;
      }
    }

    // We are not interested in this event
    return false;
  }

  private final class StandardGestureListener extends StandardGestureDetector.SimpleStandardOnGestureListener {
    @Override
    public boolean onDown(MotionEvent motionEvent) {
      return true;
    }

    @Override
    public boolean onSingleTapUp(MotionEvent motionEvent) {
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
    public boolean onDoubleTap(MotionEvent motionEvent) {
      if (!uiSettings.isZoomGesturesEnabled() || !uiSettings.isDoubleTapGesturesEnabled()) {
        return false;
      }

      executeDoubleTap = true;

      return true;
    }

    @Override
    public void onLongPress(MotionEvent motionEvent) {
      PointF longClickPoint = new PointF(motionEvent.getX(), motionEvent.getY());
      notifyOnMapLongClickListeners(longClickPoint);
    }

    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
      if ((!uiSettings.isScrollGesturesEnabled())) {
        // don't allow a fling is scroll is disabled
        return false;
      }

      float screenDensity = uiSettings.getPixelRatio();

      // calculate velocity vector for xy dimensions, independent from screen size
      double velocityXY = Math.hypot(velocityX / screenDensity, velocityY / screenDensity);
      if (velocityXY < MapboxConstants.VELOCITY_THRESHOLD_IGNORE_FLING) {
        // ignore short flings, these can occur when other gestures just have finished executing
        return false;
      }

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
  }

  private final class MoveGestureListener extends MoveGestureDetector.SimpleOnMoveGestureListener {
    @Override
    public boolean onMoveBegin(MoveGestureDetector detector) {
      if (!uiSettings.isScrollGesturesEnabled()) {
        return false;
      }

      transform.cancelTransitions();
      cameraChangeDispatcher.onCameraMoveStarted(REASON_API_GESTURE);

      if (isZoomValid(transform)) {
        MapEventFactory mapEventFactory = new MapEventFactory();
        LatLng latLng = projection.fromScreenLocation(detector.getFocalPoint());
        MapState pan = new MapState(latLng.getLatitude(), latLng.getLongitude(), transform.getZoom());
        pan.setGesture(Events.PAN);
        Events.obtainTelemetry().push(mapEventFactory.createMapGestureEvent(Event.Type.MAP_CLICK, pan));
      }

      notifyOnMoveBeginListeners(detector);

      return true;
    }

    @Override
    public boolean onMove(MoveGestureDetector detector, float distanceX, float distanceY) {
      cameraChangeDispatcher.onCameraMoveStarted(CameraChangeDispatcher.REASON_API_GESTURE);

      // Scroll the map
      transform.moveBy(-distanceX, -distanceY, 0 /*no duration*/);

      notifyOnScrollListeners();
      notifyOnMoveListeners(detector);
      return true;
    }

    @Override
    public void onMoveEnd(MoveGestureDetector detector, float velocityX, float velocityY) {
      cameraChangeDispatcher.onCameraIdle();
      notifyOnMoveEndListeners(detector);
    }
  }

  private final class ScaleGestureListener extends StandardScaleGestureDetector.SimpleStandardOnScaleGestureListener {

    private final float minimumVelocity;

    private PointF scaleFocalPoint;
    private boolean quickZoom;

    ScaleGestureListener(float minimumVelocity) {
      this.minimumVelocity = minimumVelocity;
    }

    @Override
    public boolean onScaleBegin(StandardScaleGestureDetector detector) {
      if (!uiSettings.isZoomGesturesEnabled()) {
        return false;
      }

      executeDoubleTap = false;

      transform.cancelTransitions();
      cameraChangeDispatcher.onCameraMoveStarted(REASON_API_GESTURE);

      quickZoom = detector.getPointersCount() == 1;
      if (quickZoom) {
        gesturesManager.getMoveGestureDetector().setEnabled(false);
      }

      if (isZoomValid(transform)) {
        MapEventFactory mapEventFactory = new MapEventFactory();
        LatLng latLng = projection.fromScreenLocation(detector.getFocalPoint());
        MapState pinch = new MapState(latLng.getLatitude(), latLng.getLongitude(), transform.getZoom());
        pinch.setGesture(Events.PINCH);
        Events.obtainTelemetry().push(mapEventFactory.createMapGestureEvent(Event.Type.MAP_CLICK, pinch));
      }

      gesturesManager.getRotateGestureDetector().setAngleThreshold(
        gesturesManager.getRotateGestureDetector().getDefaultAngleThreshold()
          + MapboxConstants.ROTATION_THRESHOLD_INCREASE_WHEN_SCALING
      );

      notifyOnScaleBeginListeners(detector);

      return true;
    }

    @Override
    public boolean onScale(StandardScaleGestureDetector detector) {
      cameraChangeDispatcher.onCameraMoveStarted(CameraChangeDispatcher.REASON_API_GESTURE);

      if (focalPoint != null) {
        // around user provided focal point
        scaleFocalPoint = focalPoint;
      } else if (quickZoom) {
        // around center
        scaleFocalPoint = new PointF(uiSettings.getWidth() / 2, uiSettings.getHeight() / 2);
      } else {
        // around gesture
        scaleFocalPoint = detector.getFocalPoint();
      }

      float scaleFactor = detector.getScaleFactor();
      double zoomBy = getNewZoom(scaleFactor, quickZoom);
      transform.zoomBy(zoomBy, scaleFocalPoint);

      notifyOnScaleListeners(detector);

      return true;
    }

    @Override
    public void onScaleEnd(StandardScaleGestureDetector detector, float velocityX, float velocityY) {
      cameraChangeDispatcher.onCameraIdle();

      if (quickZoom) {
        gesturesManager.getMoveGestureDetector().setEnabled(true);
      }

      gesturesManager.getRotateGestureDetector().setAngleThreshold(
        gesturesManager.getRotateGestureDetector().getDefaultAngleThreshold()
      );

      float velocityXY = Math.abs(velocityX) + Math.abs(velocityY);
      if (velocityXY > minimumVelocity) {
        double zoomAddition = calculateScale(velocityXY, detector.isScalingOut());
        double currentZoom = transform.getRawZoom();
        long animationTime = (long) (Math.abs(zoomAddition) * 1000 / 4); //todo make divider public
        scaleAnimator = createScaleAnimator(currentZoom, zoomAddition, animationTime);
        scheduleAnimator(scaleAnimator);
      }

      notifyOnScaleEndListeners(detector);
    }

    private double calculateScale(double velocityXY, boolean isScalingOut) {
      double zoomAddition = (float) Math.log(velocityXY / 1000 + 1);
      if (isScalingOut) {
        zoomAddition = -zoomAddition;
      }
      return zoomAddition;
    }

    private Animator createScaleAnimator(double currentZoom, double zoomAddition, long animationTime) {
      ValueAnimator animator = ValueAnimator.ofFloat((float) currentZoom, (float) (currentZoom + zoomAddition));
      animator.setDuration(animationTime);
      animator.setInterpolator(new DecelerateInterpolator());
      animator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {

        @Override
        public void onAnimationUpdate(ValueAnimator animation) {
          transform.setZoom((Float) animation.getAnimatedValue(), scaleFocalPoint, 0);
        }
      });

      animator.addListener(new AnimatorListenerAdapter() {

        @Override
        public void onAnimationStart(Animator animation) {
          transform.cancelTransitions();
          cameraChangeDispatcher.onCameraMoveStarted(REASON_API_ANIMATION);
        }

        @Override
        public void onAnimationCancel(Animator animation) {
          transform.cancelTransitions();
        }

        @Override
        public void onAnimationEnd(Animator animation) {
          cameraChangeDispatcher.onCameraIdle();
        }
      });
      return animator;
    }

    private double getNewZoom(float scaleFactor, boolean quickZoom) {
      double zoomBy = Math.log(scaleFactor) / Math.log(Math.PI / 2);
      if (quickZoom) {
        // clamp scale factors we feed to core #7514
        boolean negative = zoomBy < 0;
        zoomBy = MathUtils.clamp(Math.abs(zoomBy),
          MapboxConstants.MINIMUM_SCALE_FACTOR_CLAMP,
          MapboxConstants.MAXIMUM_SCALE_FACTOR_CLAMP);
        return negative ? -zoomBy : zoomBy;
      }
      return zoomBy;
    }
  }

  private final class RotateGestureListener extends RotateGestureDetector.SimpleOnRotateGestureListener {
    private PointF rotateFocalPoint;
    private final float minimumScaleSpanWhenRotating;
    private final float minimumAngularVelocity;

    RotateGestureListener(float minimumScaleSpanWhenRotating, float minimumAngularVelocity) {
      this.minimumScaleSpanWhenRotating = minimumScaleSpanWhenRotating;
      this.minimumAngularVelocity = minimumAngularVelocity;
    }

    @Override
    public boolean onRotateBegin(RotateGestureDetector detector) {
      if (!uiSettings.isRotateGesturesEnabled()) {
        return false;
      }

      transform.cancelTransitions();
      cameraChangeDispatcher.onCameraMoveStarted(REASON_API_GESTURE);

      if (isZoomValid(transform)) {
        MapEventFactory mapEventFactory = new MapEventFactory();
        LatLng latLng = projection.fromScreenLocation(detector.getFocalPoint());
        MapState rotation = new MapState(latLng.getLatitude(), latLng.getLongitude(), transform.getZoom());
        rotation.setGesture(Events.ROTATION);
        Events.obtainTelemetry().push(mapEventFactory.createMapGestureEvent(Event.Type.MAP_CLICK, rotation));
      }

      gesturesManager.getStandardScaleGestureDetector().setSpanSinceStartThreshold(minimumScaleSpanWhenRotating);
      gesturesManager.getStandardScaleGestureDetector().interrupt();

      notifyOnRotateBeginListeners(detector);

      return true;
    }

    @Override
    public boolean onRotate(RotateGestureDetector detector, float rotationDegreesSinceLast, float rotationDegreesSinceFirst) {
      cameraChangeDispatcher.onCameraMoveStarted(CameraChangeDispatcher.REASON_API_GESTURE);

      if (focalPoint != null) {
        // User provided focal point
        rotateFocalPoint = focalPoint;
      } else {
        // around gesture
        rotateFocalPoint = detector.getFocalPoint();
      }

      // Calculate map bearing value
      double bearing = transform.getRawBearing() + rotationDegreesSinceLast;

      // Rotate the map
      transform.setBearing(bearing, rotateFocalPoint.x, rotateFocalPoint.y);

      notifyOnRotateListeners(detector);

      return true;
    }

    @Override
    public void onRotateEnd(RotateGestureDetector detector, float velocityX, float velocityY, float angularVelocity) {
      cameraChangeDispatcher.onCameraIdle();

      gesturesManager.getStandardScaleGestureDetector().setSpanSinceStartThreshold(
        gesturesManager.getStandardScaleGestureDetector().getDefaultSpanSinceStartThreshold());

      if (Math.abs(angularVelocity) < minimumAngularVelocity) {
        return;
      }

      long animationTime;
      if (angularVelocity != 0) {
        boolean negative = angularVelocity < 0;
        angularVelocity = Math.abs(angularVelocity);
        if (angularVelocity > 0) {
          angularVelocity = angularVelocity * angularVelocity + 1;
        } else {
          angularVelocity = angularVelocity * 2 + 1;
        }
        angularVelocity = MathUtils.clamp(
          angularVelocity, MapboxConstants.MINIMUM_ANGULAR_VELOCITY, MapboxConstants.MAXIMUM_ANGULAR_VELOCITY);

        animationTime = (long) (Math.log(angularVelocity + 1) * 500);

        if (negative) {
          angularVelocity = -angularVelocity;
        }
      } else {
        animationTime = 0;
      }

      rotateAnimator = createRotateAnimator(angularVelocity, animationTime);
      scheduleAnimator(rotateAnimator);

      notifyOnRotateEndListeners(detector);
    }

    private Animator createRotateAnimator(float angularVelocity, long animationTime) {
      ValueAnimator animator = ValueAnimator.ofFloat(angularVelocity, 0f);
      animator.setDuration(animationTime);
      animator.setInterpolator(new DecelerateInterpolator());
      animator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
        @Override
        public void onAnimationUpdate(ValueAnimator animation) {
          transform.setBearing(
            transform.getRawBearing() + (float) animation.getAnimatedValue(),
            rotateFocalPoint.x, rotateFocalPoint.y,
            0L
          );
        }
      });

      animator.addListener(new AnimatorListenerAdapter() {

        @Override
        public void onAnimationStart(Animator animation) {
          transform.cancelTransitions();
          cameraChangeDispatcher.onCameraMoveStarted(REASON_API_ANIMATION);
        }

        @Override
        public void onAnimationCancel(Animator animation) {
          cameraChangeDispatcher.onCameraIdle();
        }

        @Override
        public void onAnimationEnd(Animator animation) {
          cameraChangeDispatcher.onCameraIdle();
        }
      });

      return animator;
    }

  }

  private final class ShoveGestureListener extends ShoveGestureDetector.SimpleOnShoveGestureListener {
    @Override
    public boolean onShoveBegin(ShoveGestureDetector detector) {
      if (!uiSettings.isTiltGesturesEnabled()) {
        return false;
      }

      transform.cancelTransitions();
      cameraChangeDispatcher.onCameraMoveStarted(REASON_API_GESTURE);

      if (isZoomValid(transform)) {
        MapEventFactory mapEventFactory = new MapEventFactory();
        LatLng latLng = projection.fromScreenLocation(detector.getFocalPoint());
        MapState pitch = new MapState(latLng.getLatitude(), latLng.getLongitude(), transform.getZoom());
        pitch.setGesture(Events.PITCH);
        Events.obtainTelemetry().push(mapEventFactory.createMapGestureEvent(Event.Type.MAP_CLICK, pitch));
      }

      gesturesManager.getMoveGestureDetector().setEnabled(false);

      notifyOnShoveBeginListeners(detector);

      return true;
    }

    @Override
    public boolean onShove(ShoveGestureDetector detector, float deltaPixelsSinceLast, float deltaPixelsSinceStart) {
      cameraChangeDispatcher.onCameraMoveStarted(CameraChangeDispatcher.REASON_API_GESTURE);

      // Get tilt value (scale and clamp)
      double pitch = transform.getTilt();
      pitch -= 0.1 * deltaPixelsSinceLast;
      pitch = MathUtils.clamp(pitch, MapboxConstants.MINIMUM_TILT, MapboxConstants.MAXIMUM_TILT);

      // Tilt the map
      transform.setTilt(pitch);

      notifyOnShoveListeners(detector);

      return true;
    }

    @Override
    public void onShoveEnd(ShoveGestureDetector detector, float velocityX, float velocityY) {
      cameraChangeDispatcher.onCameraIdle();

      gesturesManager.getMoveGestureDetector().setEnabled(true);

      notifyOnShoveEndListeners(detector);
    }
  }

  private final class TapGestureListener implements MultiFingerTapGestureDetector.OnMultiFingerTapGestureListener {
    @Override
    public boolean onMultiFingerTap(MultiFingerTapGestureDetector detector, int pointersCount) {
      if (!uiSettings.isZoomGesturesEnabled() || pointersCount != 2) {
        return false;
      }

      transform.cancelTransitions();
      cameraChangeDispatcher.onCameraMoveStarted(REASON_API_GESTURE);

      if (focalPoint != null) {
        transform.zoomOut(focalPoint);
      } else {
        transform.zoomOut(detector.getFocalPoint());
      }

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

  void notifyOnMoveBeginListeners(MoveGestureDetector detector) {
    for (MapboxMap.OnMoveListener listener : onMoveListenerList) {
      listener.onMoveBegin(detector);
    }
  }

  void notifyOnMoveListeners(MoveGestureDetector detector) {
    for (MapboxMap.OnMoveListener listener : onMoveListenerList) {
      listener.onMove(detector);
    }
  }

  void notifyOnMoveEndListeners(MoveGestureDetector detector) {
    for (MapboxMap.OnMoveListener listener : onMoveListenerList) {
      listener.onMoveEnd(detector);
    }
  }

  void notifyOnRotateBeginListeners(RotateGestureDetector detector) {
    for (MapboxMap.OnRotateListener listener : onRotateListenerList) {
      listener.onRotateBegin(detector);
    }
  }

  void notifyOnRotateListeners(RotateGestureDetector detector) {
    for (MapboxMap.OnRotateListener listener : onRotateListenerList) {
      listener.onRotate(detector);
    }
  }

  void notifyOnRotateEndListeners(RotateGestureDetector detector) {
    for (MapboxMap.OnRotateListener listener : onRotateListenerList) {
      listener.onRotateEnd(detector);
    }
  }

  void notifyOnScaleBeginListeners(StandardScaleGestureDetector detector) {
    for (MapboxMap.OnScaleListener listener : onScaleListenerList) {
      listener.onScaleBegin(detector);
    }
  }

  void notifyOnScaleListeners(StandardScaleGestureDetector detector) {
    for (MapboxMap.OnScaleListener listener : onScaleListenerList) {
      listener.onScale(detector);
    }
  }

  void notifyOnScaleEndListeners(StandardScaleGestureDetector detector) {
    for (MapboxMap.OnScaleListener listener : onScaleListenerList) {
      listener.onScaleEnd(detector);
    }
  }

  void notifyOnShoveBeginListeners(ShoveGestureDetector detector) {
    for (MapboxMap.OnShoveListener listener : onShoveListenerList) {
      listener.onShoveBegin(detector);
    }
  }

  void notifyOnShoveListeners(ShoveGestureDetector detector) {
    for (MapboxMap.OnShoveListener listener : onShoveListenerList) {
      listener.onShove(detector);
    }
  }

  void notifyOnShoveEndListeners(ShoveGestureDetector detector) {
    for (MapboxMap.OnShoveListener listener : onShoveListenerList) {
      listener.onShoveEnd(detector);
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

  void addOnMoveListener(MapboxMap.OnMoveListener listener) {
    onMoveListenerList.add(listener);
  }

  void removeOnMoveListener(MapboxMap.OnMoveListener listener) {
    onMoveListenerList.remove(listener);
  }

  void addOnRotateListener(MapboxMap.OnRotateListener listener) {
    onRotateListenerList.add(listener);
  }

  void removeOnRotateListener(MapboxMap.OnRotateListener listener) {
    onRotateListenerList.remove(listener);
  }

  void addOnScaleListener(MapboxMap.OnScaleListener listener) {
    onScaleListenerList.add(listener);
  }

  void removeOnScaleListener(MapboxMap.OnScaleListener listener) {
    onScaleListenerList.remove(listener);
  }

  void addShoveListener(MapboxMap.OnShoveListener listener) {
    onShoveListenerList.add(listener);
  }

  void removeShoveListener(MapboxMap.OnShoveListener listener) {
    onShoveListenerList.remove(listener);
  }

  AndroidGesturesManager getGesturesManager() {
    return gesturesManager;
  }

  void setGesturesManager(AndroidGesturesManager gesturesManager) {
    this.gesturesManager = gesturesManager;
  }

  private boolean isZoomValid(Transform transform) {
    if (transform == null) {
      return false;
    }
    double mapZoom = transform.getZoom();
    return mapZoom >= MapboxConstants.MINIMUM_ZOOM && mapZoom <= MapboxConstants.MAXIMUM_ZOOM;
  }
}