package com.mapbox.mapboxsdk.maps;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.ValueAnimator;
import android.content.Context;
import android.graphics.PointF;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.InputDevice;
import android.view.MotionEvent;
import android.view.animation.DecelerateInterpolator;

import com.mapbox.android.gestures.AndroidGesturesManager;
import com.mapbox.android.gestures.Constants;
import com.mapbox.android.gestures.MoveGestureDetector;
import com.mapbox.android.gestures.MultiFingerTapGestureDetector;
import com.mapbox.android.gestures.RotateGestureDetector;
import com.mapbox.android.gestures.ShoveGestureDetector;
import com.mapbox.android.gestures.StandardGestureDetector;
import com.mapbox.android.gestures.StandardScaleGestureDetector;
import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.utils.MathUtils;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.CopyOnWriteArrayList;

import static com.mapbox.mapboxsdk.constants.MapboxConstants.ZOOM_RATE;
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

  // new map touch API
  private final CopyOnWriteArrayList<MapboxMap.OnMapClickListener> onMapClickListenerList
    = new CopyOnWriteArrayList<>();

  private final CopyOnWriteArrayList<MapboxMap.OnMapLongClickListener> onMapLongClickListenerList
    = new CopyOnWriteArrayList<>();

  private final CopyOnWriteArrayList<MapboxMap.OnFlingListener> onFlingListenerList
    = new CopyOnWriteArrayList<>();

  private final CopyOnWriteArrayList<MapboxMap.OnMoveListener> onMoveListenerList
    = new CopyOnWriteArrayList<>();

  private final CopyOnWriteArrayList<MapboxMap.OnRotateListener> onRotateListenerList
    = new CopyOnWriteArrayList<>();

  private final CopyOnWriteArrayList<MapboxMap.OnScaleListener> onScaleListenerList
    = new CopyOnWriteArrayList<>();

  private final CopyOnWriteArrayList<MapboxMap.OnShoveListener> onShoveListenerList
    = new CopyOnWriteArrayList<>();

  /**
   * User-set focal point.
   */
  @Nullable
  private PointF constantFocalPoint;

  private AndroidGesturesManager gesturesManager;

  private Animator scaleAnimator;
  private Animator rotateAnimator;
  private final List<Animator> scheduledAnimators = new ArrayList<>();

  /**
   * Cancels scheduled velocity animations if user doesn't lift fingers within
   * {@link MapboxConstants#SCHEDULED_ANIMATION_TIMEOUT}
   */
  @NonNull
  private Handler animationsTimeoutHandler = new Handler();

  private boolean doubleTapRegistered;

  MapGestureDetector(@Nullable Context context, Transform transform, Projection projection, UiSettings uiSettings,
                     AnnotationManager annotationManager, CameraChangeDispatcher cameraChangeDispatcher) {
    this.annotationManager = annotationManager;
    this.transform = transform;
    this.projection = projection;
    this.uiSettings = uiSettings;
    this.cameraChangeDispatcher = cameraChangeDispatcher;

    // Checking for context != null for testing purposes
    if (context != null) {
      // Initialize gestures manager
      AndroidGesturesManager androidGesturesManager = new AndroidGesturesManager(context);
      initializeGesturesManager(androidGesturesManager, true);

      // Initialize gesture listeners
      initializeGestureListeners(context, true);
    }
  }

  private void initializeGestureListeners(@NonNull Context context, boolean attachDefaultListeners) {
    if (attachDefaultListeners) {
      StandardGestureListener standardGestureListener = new StandardGestureListener(
        context.getResources().getDimension(
          com.mapbox.android.gestures.R.dimen.mapbox_defaultScaleSpanSinceStartThreshold));
      MoveGestureListener moveGestureListener = new MoveGestureListener();
      ScaleGestureListener scaleGestureListener = new ScaleGestureListener(
        context.getResources().getDimension(R.dimen.mapbox_minimum_scale_velocity));
      RotateGestureListener rotateGestureListener = new RotateGestureListener(
        context.getResources().getDimension(R.dimen.mapbox_minimum_scale_span_when_rotating),
        context.getResources().getDimension(R.dimen.mapbox_minimum_angular_velocity),
        context.getResources().getDimension(
          com.mapbox.android.gestures.R.dimen.mapbox_defaultScaleSpanSinceStartThreshold));
      ShoveGestureListener shoveGestureListener = new ShoveGestureListener();
      TapGestureListener tapGestureListener = new TapGestureListener();

      gesturesManager.setStandardGestureListener(standardGestureListener);
      gesturesManager.setMoveGestureListener(moveGestureListener);
      gesturesManager.setStandardScaleGestureListener(scaleGestureListener);
      gesturesManager.setRotateGestureListener(rotateGestureListener);
      gesturesManager.setShoveGestureListener(shoveGestureListener);
      gesturesManager.setMultiFingerTapGestureListener(tapGestureListener);
    }
  }

  private void initializeGesturesManager(@NonNull AndroidGesturesManager androidGesturesManager,
                                         boolean setDefaultMutuallyExclusives) {
    if (setDefaultMutuallyExclusives) {
      Set<Integer> shoveScaleSet = new HashSet<>();
      shoveScaleSet.add(AndroidGesturesManager.GESTURE_TYPE_SHOVE);
      shoveScaleSet.add(AndroidGesturesManager.GESTURE_TYPE_SCALE);

      Set<Integer> shoveRotateSet = new HashSet<>();
      shoveRotateSet.add(AndroidGesturesManager.GESTURE_TYPE_SHOVE);
      shoveRotateSet.add(AndroidGesturesManager.GESTURE_TYPE_ROTATE);

      Set<Integer> ScaleLongPressSet = new HashSet<>();
      ScaleLongPressSet.add(AndroidGesturesManager.GESTURE_TYPE_SCALE);
      ScaleLongPressSet.add(AndroidGesturesManager.GESTURE_TYPE_LONG_PRESS);

      androidGesturesManager.setMutuallyExclusiveGestures(shoveScaleSet, shoveRotateSet, ScaleLongPressSet);
    }

    gesturesManager = androidGesturesManager;
  }

  /**
   * Set the gesture focal point.
   * <p>
   * This is the center point used for calculate transformations from gestures, value is
   * overridden if end user provides his own through {@link UiSettings#setFocalPoint(PointF)}.
   * </p>
   *
   * @param focalPoint the center point for gestures
   */
  void setFocalPoint(@Nullable PointF focalPoint) {
    if (focalPoint == null) {
      // resetting focal point,
      if (uiSettings.getFocalPoint() != null) {
        // using user provided one to reset
        focalPoint = uiSettings.getFocalPoint();
      }
    }
    this.constantFocalPoint = focalPoint;
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
  boolean onTouchEvent(@Nullable MotionEvent motionEvent) {
    // Framework can return null motion events in edge cases #9432
    if (motionEvent == null) {
      return false;
    }

    // Check and ignore non touch or left clicks
    if ((motionEvent.getButtonState() != 0) && (motionEvent.getButtonState() != MotionEvent.BUTTON_PRIMARY)) {
      return false;
    }

    if (motionEvent.getActionMasked() == MotionEvent.ACTION_DOWN) {
      cancelAnimators();
      transform.setGestureInProgress(true);
    }

    boolean result = gesturesManager.onTouchEvent(motionEvent);

    switch (motionEvent.getActionMasked()) {
      case MotionEvent.ACTION_POINTER_DOWN:
        doubleTapFinished();
        break;

      case MotionEvent.ACTION_UP:
        doubleTapFinished();
        transform.setGestureInProgress(false);

        if (!scheduledAnimators.isEmpty()) {
          // Start all awaiting velocity animations
          animationsTimeoutHandler.removeCallbacksAndMessages(null);
          for (Animator animator : scheduledAnimators) {
            animator.start();
          }
          scheduledAnimators.clear();
        }
        break;

      case MotionEvent.ACTION_CANCEL:
        scheduledAnimators.clear();
        transform.setGestureInProgress(false);
        doubleTapFinished();
        break;
    }

    return result;
  }

  void cancelAnimators() {
    animationsTimeoutHandler.removeCallbacksAndMessages(null);
    scheduledAnimators.clear();

    cancelAnimator(scaleAnimator);
    cancelAnimator(rotateAnimator);

    dispatchCameraIdle();
  }

  private void cancelAnimator(@Nullable Animator animator) {
    if (animator != null && animator.isStarted()) {
      animator.cancel();
    }
  }

  /**
   * Posted on main thread with {@link #animationsTimeoutHandler}. Cancels all scheduled animators if needed.
   */
  @NonNull
  private final Runnable cancelAnimatorsRunnable = new Runnable() {
    @Override
    public void run() {
      cancelAnimators();
    }
  };

  /**
   * Schedules a velocity animator to be executed when user lift fingers,
   * unless canceled by the {@link #cancelAnimatorsRunnable}.
   *
   * @param animator animator ot be scheduled
   */
  private void scheduleAnimator(Animator animator) {
    scheduledAnimators.add(animator);
    animationsTimeoutHandler.removeCallbacksAndMessages(null);
    animationsTimeoutHandler.postDelayed(cancelAnimatorsRunnable, MapboxConstants.SCHEDULED_ANIMATION_TIMEOUT);
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

    private PointF doubleTapFocalPoint;
    private final float doubleTapMovementThreshold;

    StandardGestureListener(float doubleTapMovementThreshold) {
      this.doubleTapMovementThreshold = doubleTapMovementThreshold;
    }

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

      return true;
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent motionEvent) {
      int action = motionEvent.getActionMasked();
      if (action == MotionEvent.ACTION_DOWN) {
        doubleTapFocalPoint = new PointF(motionEvent.getX(), motionEvent.getY());
        doubleTapStarted();
      }

      if (motionEvent.getActionMasked() == MotionEvent.ACTION_UP) {
        float diffX = Math.abs(motionEvent.getX() - doubleTapFocalPoint.x);
        float diffY = Math.abs(motionEvent.getY() - doubleTapFocalPoint.y);
        if (diffX > doubleTapMovementThreshold || diffY > doubleTapMovementThreshold) {
          // Ignore double-tap event because we've started the quick-zoom. See #14013.
          return false;
        }

        if (!uiSettings.isZoomGesturesEnabled() || !uiSettings.isDoubleTapGesturesEnabled()) {
          return false;
        }

        // Single finger double tap
        if (constantFocalPoint != null) {
          // User provided focal point
          doubleTapFocalPoint = constantFocalPoint;
        }

        zoomInAnimated(doubleTapFocalPoint, false);

        return true;
      }

      return super.onDoubleTapEvent(motionEvent);
    }

    @Override
    public void onLongPress(MotionEvent motionEvent) {
      PointF longClickPoint = new PointF(motionEvent.getX(), motionEvent.getY());
      notifyOnMapLongClickListeners(longClickPoint);
    }

    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
      if (!uiSettings.isScrollGesturesEnabled()) {
        // don't allow a fling if scroll is disabled
        return false;
      }

      notifyOnFlingListeners();

      if (!uiSettings.isFlingVelocityAnimationEnabled()) {
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

      return true;
    }
  }

  private void doubleTapStarted() {
    // disable the move detector in preparation for the quickzoom,
    // so that we don't move the map's center slightly before the quickzoom is started (see #14227)
    gesturesManager.getMoveGestureDetector().setEnabled(false);
    doubleTapRegistered = true;
  }

  private void doubleTapFinished() {
    if (doubleTapRegistered) {
      // re-enable the move detector in case of double tap
      gesturesManager.getMoveGestureDetector().setEnabled(true);
      doubleTapRegistered = false;
    }
  }

  private final class MoveGestureListener extends MoveGestureDetector.SimpleOnMoveGestureListener {
    @Override
    public boolean onMoveBegin(@NonNull MoveGestureDetector detector) {
      if (!uiSettings.isScrollGesturesEnabled()) {
        return false;
      }

      cancelTransitionsIfRequired();
      notifyOnMoveBeginListeners(detector);
      return true;
    }

    @Override
    public boolean onMove(@NonNull MoveGestureDetector detector, float distanceX, float distanceY) {
      // first move event is often delivered with no displacement
      if (distanceX != 0 || distanceY != 0) {
        // dispatching camera start event only when the movement actually occurred
        cameraChangeDispatcher.onCameraMoveStarted(CameraChangeDispatcher.REASON_API_GESTURE);

        // Scroll the map
        transform.moveBy(-distanceX, -distanceY, 0 /*no duration*/);

        notifyOnMoveListeners(detector);
      }
      return true;
    }

    @Override
    public void onMoveEnd(@NonNull MoveGestureDetector detector, float velocityX, float velocityY) {
      dispatchCameraIdle();
      notifyOnMoveEndListeners(detector);
    }
  }

  private final class ScaleGestureListener extends StandardScaleGestureDetector.SimpleStandardOnScaleGestureListener {

    private final float minimumVelocity;
    private boolean quickZoom;

    ScaleGestureListener(float minimumVelocity) {
      this.minimumVelocity = minimumVelocity;
    }

    @Override
    public boolean onScaleBegin(@NonNull StandardScaleGestureDetector detector) {
      quickZoom = detector.getPointersCount() == 1;

      if (!uiSettings.isZoomGesturesEnabled()) {
        return false;
      }

      if (quickZoom) {
        if (!uiSettings.isQuickZoomGesturesEnabled()) {
          return false;
        }
        // re-try disabling the move detector in case double tap has been interrupted before quickzoom started
        gesturesManager.getMoveGestureDetector().setEnabled(false);
      }

      cancelTransitionsIfRequired();

      if (uiSettings.isIncreaseRotateThresholdWhenScaling()) {
        // increase rotate angle threshold when scale is detected first
        gesturesManager.getRotateGestureDetector().setAngleThreshold(
          Constants.DEFAULT_ROTATE_ANGLE_THRESHOLD
            + MapboxConstants.ROTATION_THRESHOLD_INCREASE_WHEN_SCALING
        );
      }

      notifyOnScaleBeginListeners(detector);

      return true;
    }

    @Override
    public boolean onScale(@NonNull StandardScaleGestureDetector detector) {
      // dispatching camera start event only when the movement actually occurred
      cameraChangeDispatcher.onCameraMoveStarted(CameraChangeDispatcher.REASON_API_GESTURE);

      float scaleFactor = detector.getScaleFactor();
      double zoomBy = getNewZoom(scaleFactor, quickZoom);
      PointF focalPoint = getScaleFocalPoint(detector);
      transform.zoomBy(zoomBy, focalPoint);

      notifyOnScaleListeners(detector);

      return true;
    }

    @Override
    public void onScaleEnd(@NonNull StandardScaleGestureDetector detector, float velocityX, float velocityY) {
      if (quickZoom) {
        // re-enabled the move detector if the quickzoom happened
        gesturesManager.getMoveGestureDetector().setEnabled(true);
      }

      if (uiSettings.isIncreaseRotateThresholdWhenScaling()) {
        // resetting default angle threshold
        gesturesManager.getRotateGestureDetector().setAngleThreshold(
          Constants.DEFAULT_ROTATE_ANGLE_THRESHOLD
        );
      }

      notifyOnScaleEndListeners(detector);

      float velocityXY = Math.abs(velocityX) + Math.abs(velocityY);

      if (!uiSettings.isScaleVelocityAnimationEnabled() || velocityXY < minimumVelocity) {
        // notifying listeners that camera is idle only if there is no follow-up animation
        dispatchCameraIdle();
        return;
      }

      double zoomAddition = calculateScale(velocityXY, detector.isScalingOut());
      double currentZoom = transform.getRawZoom();
      PointF focalPoint = getScaleFocalPoint(detector);
      long animationTime = (long) (Math.abs(zoomAddition) * 1000 / 4);
      scaleAnimator = createScaleAnimator(currentZoom, zoomAddition, focalPoint, animationTime);
      scheduleAnimator(scaleAnimator);
    }

    @NonNull
    private PointF getScaleFocalPoint(@NonNull StandardScaleGestureDetector detector) {
      if (constantFocalPoint != null) {
        // around user provided focal point
        return constantFocalPoint;
      } else if (quickZoom) {
        // around center
        return new PointF(uiSettings.getWidth() / 2, uiSettings.getHeight() / 2);
      } else {
        // around gesture
        return detector.getFocalPoint();
      }
    }

    private double calculateScale(double velocityXY, boolean isScalingOut) {
      double zoomAddition = (float) Math.log(velocityXY / 1000 + 1);
      if (isScalingOut) {
        zoomAddition = -zoomAddition;
      }
      return zoomAddition;
    }

    private double getNewZoom(float scaleFactor, boolean quickZoom) {
      double zoomBy = (Math.log(scaleFactor) / Math.log(Math.PI / 2)) * ZOOM_RATE * uiSettings.getZoomRate();
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
    private final float minimumScaleSpanWhenRotating;
    private final float minimumAngularVelocity;
    private final float defaultSpanSinceStartThreshold;

    RotateGestureListener(float minimumScaleSpanWhenRotating, float minimumAngularVelocity,
                          float defaultSpanSinceStartThreshold) {
      this.minimumScaleSpanWhenRotating = minimumScaleSpanWhenRotating;
      this.minimumAngularVelocity = minimumAngularVelocity;
      this.defaultSpanSinceStartThreshold = defaultSpanSinceStartThreshold;
    }

    @Override
    public boolean onRotateBegin(@NonNull RotateGestureDetector detector) {
      if (!uiSettings.isRotateGesturesEnabled()) {
        return false;
      }

      cancelTransitionsIfRequired();

      if (uiSettings.isIncreaseScaleThresholdWhenRotating()) {
        // when rotation starts, interrupting scale and increasing the threshold
        // to make rotation without scaling easier
        gesturesManager.getStandardScaleGestureDetector().setSpanSinceStartThreshold(minimumScaleSpanWhenRotating);
        gesturesManager.getStandardScaleGestureDetector().interrupt();
      }

      notifyOnRotateBeginListeners(detector);

      return true;
    }

    @Override
    public boolean onRotate(@NonNull RotateGestureDetector detector, float rotationDegreesSinceLast,
                            float rotationDegreesSinceFirst) {
      // dispatching camera start event only when the movement actually occurred
      cameraChangeDispatcher.onCameraMoveStarted(CameraChangeDispatcher.REASON_API_GESTURE);

      // Calculate map bearing value
      double bearing = transform.getRawBearing() + rotationDegreesSinceLast;

      // Rotate the map
      PointF focalPoint = getRotateFocalPoint(detector);
      transform.setBearing(bearing, focalPoint.x, focalPoint.y);

      notifyOnRotateListeners(detector);

      return true;
    }

    @Override
    public void onRotateEnd(@NonNull RotateGestureDetector detector, float velocityX,
                            float velocityY, float angularVelocity) {
      if (uiSettings.isIncreaseScaleThresholdWhenRotating()) {
        // resetting default scale threshold values
        gesturesManager.getStandardScaleGestureDetector().setSpanSinceStartThreshold(defaultSpanSinceStartThreshold);
      }

      notifyOnRotateEndListeners(detector);

      if (!uiSettings.isRotateVelocityAnimationEnabled() || Math.abs(angularVelocity) < minimumAngularVelocity) {
        // notifying listeners that camera is idle only if there is no follow-up animation
        dispatchCameraIdle();
        return;
      }

      boolean negative = angularVelocity < 0;
      angularVelocity = (float) Math.pow(angularVelocity, 2);
      angularVelocity = MathUtils.clamp(
        angularVelocity, MapboxConstants.MINIMUM_ANGULAR_VELOCITY, MapboxConstants.MAXIMUM_ANGULAR_VELOCITY);

      long animationTime = (long) (Math.log(angularVelocity + 1) * 500);

      if (negative) {
        angularVelocity = -angularVelocity;
      }

      PointF focalPoint = getRotateFocalPoint(detector);
      rotateAnimator = createRotateAnimator(angularVelocity, animationTime, focalPoint);
      scheduleAnimator(rotateAnimator);
    }

    @NonNull
    private PointF getRotateFocalPoint(@NonNull RotateGestureDetector detector) {
      if (constantFocalPoint != null) {
        // User provided focal point
        return constantFocalPoint;
      } else {
        // around gesture
        return detector.getFocalPoint();
      }
    }

    private Animator createRotateAnimator(float angularVelocity, long animationTime,
                                          @NonNull final PointF animationFocalPoint) {
      ValueAnimator animator = ValueAnimator.ofFloat(angularVelocity, 0f);
      animator.setDuration(animationTime);
      animator.setInterpolator(new DecelerateInterpolator());
      animator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
        @Override
        public void onAnimationUpdate(@NonNull ValueAnimator animation) {
          transform.setBearing(
            transform.getRawBearing() + (float) animation.getAnimatedValue(),
            animationFocalPoint.x, animationFocalPoint.y,
            0L
          );
        }
      });

      animator.addListener(new AnimatorListenerAdapter() {

        @Override
        public void onAnimationStart(Animator animation) {
          transform.cancelTransitions();
          cameraChangeDispatcher.onCameraMoveStarted(REASON_API_GESTURE);
        }

        @Override
        public void onAnimationCancel(Animator animation) {
          transform.cancelTransitions();
        }

        @Override
        public void onAnimationEnd(Animator animation) {
          dispatchCameraIdle();
        }
      });

      return animator;
    }
  }

  private final class ShoveGestureListener extends ShoveGestureDetector.SimpleOnShoveGestureListener {
    @Override
    public boolean onShoveBegin(@NonNull ShoveGestureDetector detector) {
      if (!uiSettings.isTiltGesturesEnabled()) {
        return false;
      }

      cancelTransitionsIfRequired();

      // disabling move gesture during shove
      gesturesManager.getMoveGestureDetector().setEnabled(false);

      notifyOnShoveBeginListeners(detector);

      return true;
    }

    @Override
    public boolean onShove(@NonNull ShoveGestureDetector detector,
                           float deltaPixelsSinceLast, float deltaPixelsSinceStart) {
      // dispatching camera start event only when the movement actually occurred
      cameraChangeDispatcher.onCameraMoveStarted(CameraChangeDispatcher.REASON_API_GESTURE);

      // Get tilt value (scale and clamp)
      double pitch = transform.getTilt();
      pitch -= MapboxConstants.SHOVE_PIXEL_CHANGE_FACTOR * deltaPixelsSinceLast;
      pitch = MathUtils.clamp(pitch, MapboxConstants.MINIMUM_TILT, MapboxConstants.MAXIMUM_TILT);

      // Tilt the map
      transform.setTilt(pitch);

      notifyOnShoveListeners(detector);

      return true;
    }

    @Override
    public void onShoveEnd(@NonNull ShoveGestureDetector detector, float velocityX, float velocityY) {
      dispatchCameraIdle();

      // re-enabling move gesture
      gesturesManager.getMoveGestureDetector().setEnabled(true);

      notifyOnShoveEndListeners(detector);
    }
  }

  private final class TapGestureListener implements MultiFingerTapGestureDetector.OnMultiFingerTapGestureListener {
    @Override
    public boolean onMultiFingerTap(@NonNull MultiFingerTapGestureDetector detector, int pointersCount) {
      if (!uiSettings.isZoomGesturesEnabled() || pointersCount != 2) {
        return false;
      }

      transform.cancelTransitions();
      cameraChangeDispatcher.onCameraMoveStarted(REASON_API_GESTURE);

      PointF zoomFocalPoint;
      // Single finger double tap
      if (constantFocalPoint != null) {
        // User provided focal point
        zoomFocalPoint = constantFocalPoint;
      } else {
        // Zoom in on gesture
        zoomFocalPoint = detector.getFocalPoint();
      }

      zoomOutAnimated(zoomFocalPoint, false);

      return true;
    }
  }

  private Animator createScaleAnimator(double currentZoom, double zoomAddition,
                                       @NonNull final PointF animationFocalPoint, long animationTime) {
    ValueAnimator animator = ValueAnimator.ofFloat((float) currentZoom, (float) (currentZoom + zoomAddition));
    animator.setDuration(animationTime);
    animator.setInterpolator(new DecelerateInterpolator());
    animator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {

      @Override
      public void onAnimationUpdate(@NonNull ValueAnimator animation) {
        transform.setZoom((Float) animation.getAnimatedValue(), animationFocalPoint);
      }
    });

    animator.addListener(new AnimatorListenerAdapter() {

      @Override
      public void onAnimationStart(Animator animation) {
        transform.cancelTransitions();
        cameraChangeDispatcher.onCameraMoveStarted(REASON_API_GESTURE);
      }

      @Override
      public void onAnimationCancel(Animator animation) {
        transform.cancelTransitions();
      }

      @Override
      public void onAnimationEnd(Animator animation) {
        dispatchCameraIdle();
      }
    });
    return animator;
  }

  /**
   * Zoom in by 1.
   *
   * @param zoomFocalPoint focal point of zoom animation
   * @param runImmediately if true, animation will be started right away, otherwise it will wait until
   *                       {@link MotionEvent#ACTION_UP} is registered.
   */
  void zoomInAnimated(@NonNull PointF zoomFocalPoint, boolean runImmediately) {
    zoomAnimated(true, zoomFocalPoint, runImmediately);
  }

  /**
   * Zoom out by 1.
   *
   * @param zoomFocalPoint focal point of zoom animation
   * @param runImmediately if true, animation will be started right away, otherwise it will wait until
   *                       {@link MotionEvent#ACTION_UP} is registered.
   */
  void zoomOutAnimated(@NonNull PointF zoomFocalPoint, boolean runImmediately) {
    zoomAnimated(false, zoomFocalPoint, runImmediately);
  }

  private void zoomAnimated(boolean zoomIn, @NonNull PointF zoomFocalPoint, boolean runImmediately) {
    //canceling here as well, because when using a button it will not be canceled automatically by onDown()
    cancelAnimator(scaleAnimator);

    double currentZoom = transform.getRawZoom();
    scaleAnimator = createScaleAnimator(
      currentZoom,
      zoomIn ? 1 : -1,
      zoomFocalPoint,
      MapboxConstants.ANIMATION_DURATION);
    if (runImmediately) {
      scaleAnimator.start();
    } else {
      scheduleAnimator(scaleAnimator);
    }
  }

  private void dispatchCameraIdle() {
    // we need to dispatch camera idle callback only if there is no other gestures in progress
    if (noGesturesInProgress()) {
      // invalidate the camera position, so that it's valid when fetched from the #onIdle event
      // and doesn't rely on the last frame being rendered
      transform.invalidateCameraPosition();
      cameraChangeDispatcher.onCameraIdle();
    }
  }

  private void cancelTransitionsIfRequired() {
    // we need to cancel core transitions only if there is no started gesture yet
    if (noGesturesInProgress()) {
      transform.cancelTransitions();
    }
  }

  private boolean noGesturesInProgress() {
    return (!uiSettings.isScrollGesturesEnabled() || !gesturesManager.getMoveGestureDetector().isInProgress())
      && (!uiSettings.isZoomGesturesEnabled() || !gesturesManager.getStandardScaleGestureDetector().isInProgress())
      && (!uiSettings.isRotateGesturesEnabled() || !gesturesManager.getRotateGestureDetector().isInProgress())
      && (!uiSettings.isTiltGesturesEnabled() || !gesturesManager.getShoveGestureDetector().isInProgress());
  }

  private boolean isZoomValid(double mapZoom) {
    return mapZoom >= MapboxConstants.MINIMUM_ZOOM && mapZoom <= MapboxConstants.MAXIMUM_ZOOM;
  }

  void notifyOnMapClickListeners(@NonNull PointF tapPoint) {
    for (MapboxMap.OnMapClickListener listener : onMapClickListenerList) {
      if (listener.onMapClick(projection.fromScreenLocation(tapPoint))) {
        return;
      }
    }
  }

  void notifyOnMapLongClickListeners(@NonNull PointF longClickPoint) {
    for (MapboxMap.OnMapLongClickListener listener : onMapLongClickListenerList) {
      if (listener.onMapLongClick(projection.fromScreenLocation(longClickPoint))) {
        return;
      }
    }
  }

  void notifyOnFlingListeners() {
    for (MapboxMap.OnFlingListener listener : onFlingListenerList) {
      listener.onFling();
    }
  }

  void notifyOnMoveBeginListeners(@NonNull MoveGestureDetector detector) {
    for (MapboxMap.OnMoveListener listener : onMoveListenerList) {
      listener.onMoveBegin(detector);
    }
  }

  void notifyOnMoveListeners(@NonNull MoveGestureDetector detector) {
    for (MapboxMap.OnMoveListener listener : onMoveListenerList) {
      listener.onMove(detector);
    }
  }

  void notifyOnMoveEndListeners(@NonNull MoveGestureDetector detector) {
    for (MapboxMap.OnMoveListener listener : onMoveListenerList) {
      listener.onMoveEnd(detector);
    }
  }

  void notifyOnRotateBeginListeners(@NonNull RotateGestureDetector detector) {
    for (MapboxMap.OnRotateListener listener : onRotateListenerList) {
      listener.onRotateBegin(detector);
    }
  }

  void notifyOnRotateListeners(@NonNull RotateGestureDetector detector) {
    for (MapboxMap.OnRotateListener listener : onRotateListenerList) {
      listener.onRotate(detector);
    }
  }

  void notifyOnRotateEndListeners(@NonNull RotateGestureDetector detector) {
    for (MapboxMap.OnRotateListener listener : onRotateListenerList) {
      listener.onRotateEnd(detector);
    }
  }

  void notifyOnScaleBeginListeners(@NonNull StandardScaleGestureDetector detector) {
    for (MapboxMap.OnScaleListener listener : onScaleListenerList) {
      listener.onScaleBegin(detector);
    }
  }

  void notifyOnScaleListeners(@NonNull StandardScaleGestureDetector detector) {
    for (MapboxMap.OnScaleListener listener : onScaleListenerList) {
      listener.onScale(detector);
    }
  }

  void notifyOnScaleEndListeners(@NonNull StandardScaleGestureDetector detector) {
    for (MapboxMap.OnScaleListener listener : onScaleListenerList) {
      listener.onScaleEnd(detector);
    }
  }

  void notifyOnShoveBeginListeners(@NonNull ShoveGestureDetector detector) {
    for (MapboxMap.OnShoveListener listener : onShoveListenerList) {
      listener.onShoveBegin(detector);
    }
  }

  void notifyOnShoveListeners(@NonNull ShoveGestureDetector detector) {
    for (MapboxMap.OnShoveListener listener : onShoveListenerList) {
      listener.onShove(detector);
    }
  }

  void notifyOnShoveEndListeners(@NonNull ShoveGestureDetector detector) {
    for (MapboxMap.OnShoveListener listener : onShoveListenerList) {
      listener.onShoveEnd(detector);
    }
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

  void setGesturesManager(@NonNull Context context, @NonNull AndroidGesturesManager gesturesManager,
                          boolean attachDefaultListeners, boolean setDefaultMutuallyExclusives) {
    initializeGesturesManager(gesturesManager, setDefaultMutuallyExclusives);
    initializeGestureListeners(context, attachDefaultListeners);
  }
}