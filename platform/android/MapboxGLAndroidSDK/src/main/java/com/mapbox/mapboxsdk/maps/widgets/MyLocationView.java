package com.mapbox.mapboxsdk.maps.widgets;

import android.animation.ValueAnimator;
import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Camera;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PointF;
import android.graphics.PorterDuff;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Location;
import android.os.Bundle;
import android.os.Parcelable;
import android.os.SystemClock;
import android.support.annotation.ColorInt;
import android.support.annotation.FloatRange;
import android.support.annotation.IntRange;
import android.support.annotation.NonNull;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Projection;
import com.mapbox.services.android.telemetry.location.LocationEngine;
import com.mapbox.services.android.telemetry.location.LocationEngineListener;
import com.mapbox.services.android.telemetry.location.LocationEnginePriority;

import java.lang.ref.WeakReference;

import timber.log.Timber;

/**
 * UI element overlaid on a map to show the user's location.
 * <p>
 * Use {@link MyLocationViewSettings} to manipulate the state of this view.
 * </p>
 *
 * @deprecated use location layer plugin from
 * https://github.com/mapbox/mapbox-plugins-android/tree/master/plugins/locationlayer instead.
 */
@Deprecated
public class MyLocationView extends View {

  private static final int UNDEFINED_TINT_COLOR = -1;
  private MyLocationBehavior myLocationBehavior;
  private MapboxMap mapboxMap;

  private Projection projection;
  private float[] projectedCoordinate = new float[2];
  private float projectedX;
  private float projectedY;

  private float contentPaddingX;
  private float contentPaddingY;

  private LatLng latLng;
  private Location location;
  private LocationEngine locationEngine;
  private long locationUpdateTimestamp;
  private float previousDirection;

  private float accuracy;
  private Paint accuracyPaint;
  private float accuracyThreshold;

  private ValueAnimator locationChangeAnimator;
  private ValueAnimator accuracyAnimator;
  private ValueAnimator directionAnimator;
  private boolean locationChangeAnimationEnabled = true;

  private ValueAnimator.AnimatorUpdateListener invalidateSelfOnUpdateListener =
    new ValueAnimator.AnimatorUpdateListener() {
      @Override
      public void onAnimationUpdate(ValueAnimator animation) {
        invalidate();
      }
    };

  private Drawable foregroundDrawable;
  private Drawable foregroundBearingDrawable;
  private Drawable backgroundDrawable;

  private Rect foregroundBounds;
  private Rect backgroundBounds;

  private int backgroundOffsetLeft;
  private int backgroundOffsetTop;
  private int backgroundOffsetRight;
  private int backgroundOffsetBottom;

  private Matrix matrix;
  private Camera camera;
  private PointF screenLocation;

  // camera vars
  private double tilt;
  private double bearing;
  private float magneticHeading;

  // Controls the compass update rate in milliseconds
  private static final int COMPASS_UPDATE_RATE_MS = 500;

  @MyLocationTracking.Mode
  private int myLocationTrackingMode;

  @MyBearingTracking.Mode
  private int myBearingTrackingMode;

  private GpsLocationListener userLocationListener;
  private CompassListener compassListener;

  public MyLocationView(Context context) {
    super(context);
    init(context);
  }

  public MyLocationView(Context context, AttributeSet attrs) {
    super(context, attrs);
    init(context);
  }

  public MyLocationView(Context context, AttributeSet attrs, int defStyleAttr) {
    super(context, attrs, defStyleAttr);
    init(context);
  }

  private void init(Context context) {
    if (isInEditMode()) {
      return;
    }

    setEnabled(false);

    // setup LayoutParams
    ViewGroup.LayoutParams lp = new ViewGroup.LayoutParams(
      ViewGroup.LayoutParams.MATCH_PARENT,
      ViewGroup.LayoutParams.MATCH_PARENT);
    setLayoutParams(lp);

    matrix = new Matrix();
    camera = new Camera();
    camera.setLocation(0, 0, -1000);
    accuracyPaint = new Paint();

    myLocationBehavior = new MyLocationBehaviorFactory().getBehavioralModel(MyLocationTracking.TRACKING_NONE);
    compassListener = new CompassListener(context);
  }

  @Deprecated
  public void init(LocationEngine locationSource) {
    this.locationEngine = locationSource;
  }

  /**
   * Set the foreground drawable, for internal use only.
   *
   * @param defaultDrawable The drawable shown when showing this view
   * @param bearingDrawable The drawable shown when tracking of bearing is enabled
   */
  public final void setForegroundDrawables(Drawable defaultDrawable, Drawable bearingDrawable) {
    if (defaultDrawable == null) {
      return;
    }

    if (bearingDrawable == null) {
      // if user only provided one resource
      // use same for bearing mode
      bearingDrawable = defaultDrawable.getConstantState().newDrawable();
    }

    if (backgroundDrawable == null) {
      // if the user didn't provide a background resource we will use the foreground resource instead,
      // we need to create a new drawable to handle tinting correctly
      backgroundDrawable = defaultDrawable.getConstantState().newDrawable();
    }

    if (defaultDrawable.getIntrinsicWidth() != bearingDrawable.getIntrinsicWidth()
      || defaultDrawable.getIntrinsicHeight() != bearingDrawable.getIntrinsicHeight()) {
      throw new RuntimeException("The dimensions from location and bearing drawables should be match");
    }

    foregroundDrawable = defaultDrawable;
    foregroundBearingDrawable = bearingDrawable;

    invalidateBounds();
  }

  /**
   * Set the foreground drawable tint, for internal use only.
   *
   * @param color The color to tint the drawable with
   */
  public final void setForegroundDrawableTint(@ColorInt int color) {
    applyDrawableTint(foregroundDrawable, color);
    applyDrawableTint(foregroundBearingDrawable, color);
    invalidate();
  }

  /**
   * Set the shadow drawable, for internal use only.
   *
   * @param drawable The drawable shown as shadow
   */
  public final void setShadowDrawable(Drawable drawable) {
    setShadowDrawable(drawable, 0, 0, 0, 0);
  }

  /**
   * Set the shadow drawable with some additional offset.
   *
   * @param drawable The drawable shown as shadow
   * @param left     The left offset margin
   * @param top      The top offset margin
   * @param right    The right offset margin
   * @param bottom   The bottom offset margin
   */
  public final void setShadowDrawable(Drawable drawable, int left, int top, int right, int bottom) {
    if (drawable != null) {
      backgroundDrawable = drawable;
    }

    backgroundOffsetLeft = left;
    backgroundOffsetTop = top;
    backgroundOffsetRight = right;
    backgroundOffsetBottom = bottom;

    invalidateBounds();
  }

  /**
   * Set the shadow drawable tint color, for internal use only.
   *
   * @param color The tint color to apply
   */
  public final void setShadowDrawableTint(@ColorInt int color) {
    if (backgroundDrawable == null) {
      return;
    }
    applyDrawableTint(backgroundDrawable, color);
    invalidate();
  }

  /**
   * Set the accuracy tint color, for internal use only.
   *
   * @param color The tint color to apply
   */
  public final void setAccuracyTint(@ColorInt int color) {
    int alpha = accuracyPaint.getAlpha();
    accuracyPaint.setColor(color);
    accuracyPaint.setAlpha(alpha);
    invalidate();
  }

  /**
   * Set the accuracy alpha value, for internal use only.
   *
   * @param alpha The alpha accuracy value to apply
   */
  public final void setAccuracyAlpha(@IntRange(from = 0, to = 255) int alpha) {
    accuracyPaint.setAlpha(alpha);
    invalidate();
  }

  private void invalidateBounds() {
    if (backgroundDrawable == null || foregroundDrawable == null || foregroundBearingDrawable == null) {
      return;
    }

    int backgroundWidth = backgroundDrawable.getIntrinsicWidth();
    int backgroundHeight = backgroundDrawable.getIntrinsicHeight();
    int horizontalOffset = backgroundOffsetLeft - backgroundOffsetRight;
    int verticalOffset = backgroundOffsetTop - backgroundOffsetBottom;
    backgroundBounds = new Rect(-backgroundWidth / 2 + horizontalOffset,
      -backgroundHeight / 2 + verticalOffset, backgroundWidth / 2 + horizontalOffset, backgroundHeight / 2
      + verticalOffset);
    backgroundDrawable.setBounds(backgroundBounds);

    int foregroundWidth = foregroundDrawable.getIntrinsicWidth();
    int foregroundHeight = foregroundDrawable.getIntrinsicHeight();
    foregroundBounds = new Rect(-foregroundWidth / 2, -foregroundHeight / 2, foregroundWidth / 2, foregroundHeight / 2);
    foregroundDrawable.setBounds(foregroundBounds);
    foregroundBearingDrawable.setBounds(foregroundBounds);

    // invoke a new draw
    invalidate();
  }

  @Override
  protected void onDraw(Canvas canvas) {
    super.onDraw(canvas);

    if (location == null || foregroundBounds == null || backgroundBounds == null || accuracyAnimator == null
      || screenLocation == null) {
      // Not ready yet
      return;
    }

    final PointF pointF = screenLocation;
    float metersPerPixel = (float) projection.getMetersPerPixelAtLatitude(location.getLatitude());
    float accuracyPixels = (Float) accuracyAnimator.getAnimatedValue() / metersPerPixel;

    // reset
    matrix.reset();
    projectedCoordinate[0] = 0;
    projectedCoordinate[1] = 0;

    // put camera in position
    camera.save();
    camera.rotate((float) tilt, 0, 0);
    camera.getMatrix(matrix);

    if (myBearingTrackingMode != MyBearingTracking.NONE && directionAnimator != null) {
      matrix.preRotate((Float) directionAnimator.getAnimatedValue());
    }

    matrix.preTranslate(0, contentPaddingY);
    matrix.postTranslate(pointF.x, pointF.y - contentPaddingY);

    // concat our matrix on canvas
    canvas.concat(matrix);

    // calculate focal point
    matrix.mapPoints(projectedCoordinate);
    projectedX = pointF.x - projectedCoordinate[0];
    projectedY = pointF.y - projectedCoordinate[1];

    // restore orientation from camera
    camera.restore();

    // draw circle
    canvas.drawCircle(0, 0, accuracyPixels, accuracyPaint);

    // draw shadow
    if (backgroundDrawable != null) {
      backgroundDrawable.draw(canvas);
    }

    // draw foreground
    if (myBearingTrackingMode == MyBearingTracking.NONE) {
      if (foregroundDrawable != null) {
        foregroundDrawable.draw(canvas);
      }
    } else if (foregroundBearingDrawable != null && foregroundBounds != null) {
      if (myBearingTrackingMode == MyBearingTracking.GPS || compassListener.isSensorAvailable()) {
        foregroundBearingDrawable.draw(canvas);
      } else {
        // We are tracking MyBearingTracking.COMPASS, but sensor is not available.
        foregroundDrawable.draw(canvas);
      }
    }
  }

  /**
   * Set the tilt value, for internal use only.
   *
   * @param tilt The tilt to apply
   */
  public void setTilt(@FloatRange(from = 0, to = 60.0f) double tilt) {
    this.tilt = tilt;
    if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
      mapboxMap.getUiSettings().setFocalPoint(getCenter());
    }
    invalidate();
  }

  /**
   * Set the bearing value, for internal use only.
   *
   * @param bearing The bearing to apply
   */
  public void setBearing(double bearing) {
    this.bearing = bearing;
    if (myLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
      if (myBearingTrackingMode == MyBearingTracking.GPS) {
        if (location != null) {
          setCompass(location.getBearing() - bearing);
        }
      } else if (myBearingTrackingMode == MyBearingTracking.COMPASS && compassListener.isSensorAvailable()) {
        setCompass(magneticHeading - bearing);
      }
    }
  }

  /**
   * Set the bearing and tilt from a camera position, for internal use only.
   *
   * @param position The camera position to extract bearing and tilt from
   */
  public void setCameraPosition(CameraPosition position) {
    if (position != null) {
      setBearing(position.bearing);
      setTilt(position.tilt);
    }
  }

  /**
   * Called when the hosting activity is starting, for internal use only.
   */
  public void onStart() {
    if (myBearingTrackingMode == MyBearingTracking.COMPASS && compassListener.isSensorAvailable()) {
      compassListener.onResume();
    }
    if (isEnabled()) {
      toggleGps(true);
    }
  }

  /**
   * Called when the hosting activity is stopping, for internal use only.
   */
  public void onStop() {
    compassListener.onPause();
    toggleGps(false);
  }

  @Override
  protected void onDetachedFromWindow() {
    super.onDetachedFromWindow();
    // cleanup to prevent memory leaks
    if (locationChangeAnimator != null) {
      locationChangeAnimator.cancel();
      locationChangeAnimator = null;
    }

    if (accuracyAnimator != null) {
      accuracyAnimator.cancel();
      accuracyAnimator = null;
    }

    if (directionAnimator != null) {
      directionAnimator.cancel();
      directionAnimator = null;
    }

    if (userLocationListener != null) {
      locationEngine.removeLocationEngineListener(userLocationListener);
      locationEngine = null;
      userLocationListener = null;
    }
  }

  /**
   * Update current locationstate.
   */
  public void update() {
    if (isEnabled()) {
      myLocationBehavior.invalidate();
    } else {
      setVisibility(View.INVISIBLE);
    }
  }

  // TODO refactor MapboxMap out
  public void setMapboxMap(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
    this.projection = mapboxMap.getProjection();
  }

  /**
   * Set the enabled state, for internal use only.
   *
   * @param enabled The value to set the state to
   */
  @Override
  public void setEnabled(boolean enabled) {
    setEnabled(enabled, false);
  }

  /**
   * Set the enabled state, for internal use only.
   *
   * @param enabled                The value to set the state to
   * @param isCustomLocationEngine Flag handling for handling user provided custom location engine
   */
  public void setEnabled(boolean enabled, boolean isCustomLocationEngine) {
    super.setEnabled(enabled);
    setVisibility(enabled ? View.VISIBLE : View.INVISIBLE);
    toggleGps(enabled, isCustomLocationEngine);
  }

  /**
   * Save the view instance state, for internal use only.
   *
   * @return the marshaled representation of the view state
   */
  @Override
  protected Parcelable onSaveInstanceState() {
    Bundle bundle = new Bundle();
    bundle.putParcelable("superState", super.onSaveInstanceState());
    bundle.putDouble("tilt", tilt);
    return bundle;
  }

  /**
   * Restore the view instance state, for internal use only.
   *
   * @param state the marshalled representation of the state to restore
   */
  @Override
  public void onRestoreInstanceState(Parcelable state) {
    if (state instanceof Bundle) {
      Bundle bundle = (Bundle) state;
      tilt = bundle.getDouble("tilt");
      state = bundle.getParcelable("superState");
    }
    super.onRestoreInstanceState(state);
  }

  private void toggleGps(boolean enableGps) {
    toggleGps(enableGps, mapboxMap != null && mapboxMap.getTrackingSettings().isCustomLocationSource());
  }

  /**
   * Enabled / Disable GPS location updates along with updating the UI, for internal use only.
   *
   * @param enableGps true if GPS is to be enabled, false if GPS is to be disabled
   */
  private void toggleGps(boolean enableGps, boolean isCustomLocationEngine) {
    if (enableGps) {
      if (locationEngine == null) {
        if (!isCustomLocationEngine) {
          locationEngine = Mapbox.getLocationEngine();
        } else {
          return;
        }
      }

      if (userLocationListener == null) {
        userLocationListener = new GpsLocationListener(this, locationEngine);
      }

      locationEngine.addLocationEngineListener(userLocationListener);
      locationEngine.setPriority(LocationEnginePriority.HIGH_ACCURACY);
      locationEngine.activate();
    } else {
      if (locationEngine == null) {
        return;
      }
      // Disable location and user dot
      location = null;
      locationEngine.removeLocationEngineListener(userLocationListener);
      locationEngine.removeLocationUpdates();
      locationEngine.deactivate();
      restoreLocationEngine();
    }
  }

  /**
   * Get the current location.
   *
   * @return the current location
   */
  public Location getLocation() {
    return location;
  }

  /**
   * Set the current location, for internal use only.
   *
   * @param location The current location
   */
  public void setLocation(Location location) {
    if (location == null) {
      this.location = null;
      return;
    }

    this.location = location;
    myLocationBehavior.updateLatLng(location);

    if (mapboxMap != null && myBearingTrackingMode == MyBearingTracking.GPS
      && myLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
      setBearing(mapboxMap.getCameraPosition().bearing);
    }
  }

  /**
   * Set location change animation enabled, for internal use only.
   *
   * @param locationChangeAnimationEnabled True if location changes are animated
   */
  public void setLocationChangeAnimationEnabled(boolean locationChangeAnimationEnabled) {
    this.locationChangeAnimationEnabled = locationChangeAnimationEnabled;
  }

  /**
   * Set accuracy circle threshold. Circle won't be displayed if accuracy is below set value.
   * For internal use only.
   *
   * @param accuracyThreshold Value below which circle won't be displayed
   */
  public void setAccuracyThreshold(float accuracyThreshold) {
    this.accuracyThreshold = accuracyThreshold;
  }

  /**
   * Set the bearing tracking mode, for internal use only.
   *
   * @param myBearingTrackingMode The bearing tracking mode
   */
  public void setMyBearingTrackingMode(@MyBearingTracking.Mode int myBearingTrackingMode) {
    this.myBearingTrackingMode = myBearingTrackingMode;
    if (myBearingTrackingMode == MyBearingTracking.COMPASS && compassListener.isSensorAvailable()) {
      compassListener.onResume();
    } else {
      compassListener.onPause();
      if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
        // always face north
        setCompass(0);
      } else {
        myLocationBehavior.invalidate();
      }
    }
    invalidate();
  }

  /**
   * Set the location tracking mode, for internla use only.
   *
   * @param myLocationTrackingMode The location tracking mode
   */
  public void setMyLocationTrackingMode(@MyLocationTracking.Mode int myLocationTrackingMode) {
    MyLocationBehaviorFactory factory = new MyLocationBehaviorFactory();
    myLocationBehavior = factory.getBehavioralModel(myLocationTrackingMode);

    if (location != null) {
      if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
        // center map directly
        mapboxMap.moveCamera(CameraUpdateFactory.newLatLng(new LatLng(location)));
      } else {
        // do not use interpolated location from tracking mode
        latLng = null;
      }
      myLocationBehavior.updateLatLng(location);
    }

    this.myLocationTrackingMode = myLocationTrackingMode;
    invalidate();
  }

  /**
   * Get the location tracking mode, for internal use only.
   *
   * @return The location tracking mode
   */
  @MyLocationTracking.Mode
  public int getMyLocationTrackingMode() {
    return myLocationTrackingMode;
  }


  /**
   * Get the bearing tracking mode, for internal use only.
   *
   * @return the bearing tracking mode
   */
  @MyBearingTracking.Mode
  public int getMyBearingTrackingMode() {
    return myBearingTrackingMode;
  }

  /**
   * Set the compass bearing value, for internal use only.
   *
   * @param bearing The compas bearing value
   */
  private void setCompass(double bearing) {
    setCompass(bearing, 0 /* no animation */);
  }

  private void setCompass(double bearing, long duration) {
    float oldDir = previousDirection;
    if (directionAnimator != null) {
      oldDir = (Float) directionAnimator.getAnimatedValue();
      directionAnimator.end();
      directionAnimator = null;
    }

    float newDir = (float) bearing;
    float diff = oldDir - newDir;
    if (diff > 180.0f) {
      newDir += 360.0f;
    } else if (diff < -180.0f) {
      newDir -= 360.f;
    }
    previousDirection = newDir;

    directionAnimator = ValueAnimator.ofFloat(oldDir, newDir);
    directionAnimator.setDuration(duration);
    directionAnimator.addUpdateListener(invalidateSelfOnUpdateListener);
    directionAnimator.start();
  }

  /**
   * Get the center of this view in screen coordinates.
   *
   * @return the center of the view
   */
  public PointF getCenter() {
    return new PointF(getCenterX(), getCenterY());
  }

  /**
   * Get the x value of the center of this view.
   *
   * @return the x value of the center of the view
   */
  private float getCenterX() {
    return (getX() + getMeasuredWidth()) / 2 + contentPaddingX - projectedX;
  }

  /**
   * Get the y value of the center of this view.
   *
   * @return the y value of the center of the view
   */
  private float getCenterY() {
    return (getY() + getMeasuredHeight()) / 2 + contentPaddingY - projectedY;
  }

  public void setContentPadding(int[] padding) {
    contentPaddingX = (padding[0] - padding[2]) / 2;
    contentPaddingY = (padding[1] - padding[3]) / 2;
  }

  /**
   * Set the location source from which location updates are received, for internal use only.
   *
   * @param locationEngine The location engine to receive updates from
   */
  public void setLocationSource(LocationEngine locationEngine) {
    toggleGps(false);
    this.locationEngine = locationEngine;
    this.userLocationListener = null;
    setEnabled(isEnabled(), locationEngine != null);
  }

  private void applyDrawableTint(Drawable drawable, @ColorInt int color) {
    if (color == UNDEFINED_TINT_COLOR) {
      removeTintColorFilter(drawable);
    } else {
      applyTintColorFilter(drawable, color);
    }
  }

  private void removeTintColorFilter(Drawable drawable) {
    if (drawable != null) {
      drawable.mutate().setColorFilter(null);
    }
  }

  private void applyTintColorFilter(Drawable drawable, @ColorInt int color) {
    if (drawable != null) {
      drawable.mutate().setColorFilter(color, PorterDuff.Mode.SRC_IN);
    }
  }

  private void restoreLocationEngine() {
    locationEngine.setPriority(LocationEnginePriority.LOW_POWER);
    locationEngine.activate();
  }

  private static class GpsLocationListener implements LocationEngineListener {

    private WeakReference<MyLocationView> userLocationView;
    private WeakReference<LocationEngine> locationSource;

    GpsLocationListener(MyLocationView myLocationView, LocationEngine locationEngine) {
      userLocationView = new WeakReference<>(myLocationView);
      locationSource = new WeakReference<>(locationEngine);
    }

    @SuppressLint("MissingPermission")
    @Override
    public void onConnected() {
      MyLocationView locationView = userLocationView.get();
      LocationEngine locationEngine = locationSource.get();
      if (locationView != null && locationEngine != null) {
        Location lastKnownLocation = locationEngine.getLastLocation();
        if (lastKnownLocation != null) {
          locationView.setLocation(lastKnownLocation);
        }
        locationEngine.requestLocationUpdates();
      }
    }

    /**
     * Callback method for receiving location updates from LocationServices.
     *
     * @param location The new Location data
     */
    @Override
    public void onLocationChanged(Location location) {
      MyLocationView locationView = userLocationView.get();
      if (locationView != null) {
        locationView.setLocation(location);
      }
    }
  }

  private class CompassListener implements SensorEventListener {

    private final SensorManager sensorManager;

    private Sensor rotationVectorSensor;
    private float[] matrix = new float[9];
    private float[] rotationVectorValue;
    private float[] truncatedRotationVectorValue = new float[4];

    private float[] orientation = new float[3];
    private boolean reportMissingSensor = true;
    // Compass data
    private long compassUpdateNextTimestamp = 0;

    CompassListener(Context context) {
      sensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
      rotationVectorSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);
    }

    public void onResume() {
      sensorManager.registerListener(this, rotationVectorSensor, SensorManager.SENSOR_DELAY_GAME);
    }

    public void onPause() {
      sensorManager.unregisterListener(this, rotationVectorSensor);
    }

    public boolean isSensorAvailable() {
      if (rotationVectorSensor == null && reportMissingSensor) {
        reportMissingSensor = false;
        Timber.e("Sensor.TYPE_ROTATION_VECTOR is missing from this device. Unable to use MyBearingTracking.COMPASS.");
      }
      return rotationVectorSensor != null;
    }

    @Override
    public void onSensorChanged(SensorEvent event) {

      // check when the last time the compass was updated, return if too soon.
      long currentTime = SystemClock.elapsedRealtime();
      if (currentTime < compassUpdateNextTimestamp) {
        return;
      }

      if (event.sensor.getType() == Sensor.TYPE_ROTATION_VECTOR) {
        rotationVectorValue = getRotationVectorFromSensorEvent(event);
        SensorManager.getRotationMatrixFromVector(matrix, rotationVectorValue);
        SensorManager.getOrientation(matrix, orientation);

        magneticHeading = (float) Math.toDegrees(SensorManager.getOrientation(matrix, orientation)[0]);
        if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
          // Change the user location view orientation to reflect the device orientation
          rotateCamera(magneticHeading);
          setCompass(0, COMPASS_UPDATE_RATE_MS);
        } else {
          // Change compass direction
          setCompass(magneticHeading - bearing, COMPASS_UPDATE_RATE_MS);
        }

        compassUpdateNextTimestamp = currentTime + COMPASS_UPDATE_RATE_MS;
      }
    }

    /**
     * Pulls out the rotation vector from a SensorEvent, with a maximum length
     * vector of four elements to avoid potential compatibility issues.
     *
     * @param event the sensor event
     * @return the events rotation vector, potentially truncated
     */
    @NonNull
    float[] getRotationVectorFromSensorEvent(@NonNull SensorEvent event) {
      if (event.values.length > 4) {
        // On some Samsung devices SensorManager.getRotationMatrixFromVector
        // appears to throw an exception if rotation vector has length > 4.
        // For the purposes of this class the first 4 values of the
        // rotation vector are sufficient (see crbug.com/335298 for details).
        // Only affects Android 4.3
        System.arraycopy(event.values, 0, truncatedRotationVectorValue, 0, 4);
        return truncatedRotationVectorValue;
      } else {
        return event.values;
      }
    }

    private void rotateCamera(float rotation) {
      CameraPosition.Builder builder = new CameraPosition.Builder();
      builder.bearing(rotation);
      mapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(builder.build()), COMPASS_UPDATE_RATE_MS,
        false /*linear interpolator*/, null);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }

  }

  private class MarkerCoordinateAnimatorListener implements ValueAnimator.AnimatorUpdateListener {

    private MyLocationBehavior behavior;
    private double fromLat;
    private double fromLng;
    private double toLat;
    private double toLng;

    private MarkerCoordinateAnimatorListener(MyLocationBehavior myLocationBehavior, LatLng from, LatLng to) {
      behavior = myLocationBehavior;
      fromLat = from.getLatitude();
      fromLng = from.getLongitude();
      toLat = to.getLatitude();
      toLng = to.getLongitude();
    }

    @Override
    public void onAnimationUpdate(ValueAnimator animation) {
      float frac = animation.getAnimatedFraction();
      double latitude = fromLat + (toLat - fromLat) * frac;
      double longitude = fromLng + (toLng - fromLng) * frac;
      behavior.updateLatLng(latitude, longitude);
      update();
    }
  }

  private class MyLocationBehaviorFactory {

    MyLocationBehavior getBehavioralModel(@MyLocationTracking.Mode int mode) {
      if (mode == MyLocationTracking.TRACKING_NONE) {
        return new MyLocationShowBehavior();
      } else {
        return new MyLocationTrackingBehavior();
      }
    }
  }

  private abstract class MyLocationBehavior {

    MyLocationBehavior() {
      if (latLng != null) {
        locationUpdateTimestamp = SystemClock.elapsedRealtime();
      }
    }

    void updateLatLng(@NonNull Location newLocation) {
      location = newLocation;
    }

    void updateLatLng(double lat, double lon) {
      if (latLng != null) {
        latLng.setLatitude(lat);
        latLng.setLongitude(lon);
      }
    }

    void updateAccuracy(@NonNull Location location) {
      if (accuracyAnimator != null && accuracyAnimator.isRunning()) {
        // use current accuracy as a starting point
        accuracy = (Float) accuracyAnimator.getAnimatedValue();
        accuracyAnimator.end();
      }

      float newAccuracy = location.getAccuracy() >= accuracyThreshold ? location.getAccuracy() : 0f;
      accuracyAnimator = ValueAnimator.ofFloat(accuracy, newAccuracy);
      accuracyAnimator.setDuration(750);
      accuracyAnimator.start();
      accuracy = newAccuracy;
    }

    abstract void invalidate();
  }

  private class MyLocationTrackingBehavior extends MyLocationBehavior {

    @Override
    void updateLatLng(@NonNull Location location) {
      super.updateLatLng(location);
      if (latLng == null) {
        // first location fix
        latLng = new LatLng(location);
        locationUpdateTimestamp = SystemClock.elapsedRealtime();
      }

      // updateLatLng timestamp
      float previousUpdateTimeStamp = locationUpdateTimestamp;
      locationUpdateTimestamp = SystemClock.elapsedRealtime();

      // calculate animation duration
      int animationDuration;
      if (previousUpdateTimeStamp == 0) {
        animationDuration = 0;
      } else {
        animationDuration = (int) ((locationUpdateTimestamp - previousUpdateTimeStamp) * 1.1f)
        /*make animation slightly longer*/;
      }

      // calculate interpolated location
      latLng = new LatLng(location);
      CameraPosition.Builder builder = new CameraPosition.Builder().target(latLng);

      // add direction
      if (myBearingTrackingMode == MyBearingTracking.GPS) {
        if (location.hasBearing()) {
          builder.bearing(location.getBearing());
        }
        setCompass(0, COMPASS_UPDATE_RATE_MS);
      }

      // accuracy
      updateAccuracy(location);

      if (locationChangeAnimationEnabled && animationDuration > 0) {
        // ease to new camera position with a linear interpolator
        mapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(builder.build()), animationDuration, false, null,
          true);
      } else {
        mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(builder.build()));
      }
    }

    @Override
    void invalidate() {
      int[] mapPadding = mapboxMap.getPadding();
      float x = (getWidth() + mapPadding[0] - mapPadding[2]) / 2 + contentPaddingX;
      float y = (getHeight() - mapPadding[3] + mapPadding[1]) / 2 + contentPaddingY;
      screenLocation = new PointF(x, y);
      MyLocationView.this.invalidate();
    }
  }

  private class MyLocationShowBehavior extends MyLocationBehavior {

    @Override
    void updateLatLng(@NonNull final Location location) {
      super.updateLatLng(location);
      if (latLng == null) {
        // first location update
        latLng = new LatLng(location);
        locationUpdateTimestamp = SystemClock.elapsedRealtime();
      }

      // update LatLng location
      LatLng newLocation = new LatLng(location);

      // update LatLng accuracy
      updateAccuracy(location);

      // calculate updateLatLng time + add some extra offset to improve animation
      long previousUpdateTimeStamp = locationUpdateTimestamp;
      locationUpdateTimestamp = SystemClock.elapsedRealtime();
      long locationUpdateDuration = (long) ((locationUpdateTimestamp - previousUpdateTimeStamp) * 1.2f);

      // animate changes
      if (locationChangeAnimator != null) {
        locationChangeAnimator.end();
        locationChangeAnimator = null;
      }

      locationChangeAnimator = ValueAnimator.ofFloat(0.0f, 1.0f);
      if (locationChangeAnimationEnabled) {
        locationChangeAnimator.setDuration(locationUpdateDuration);
      } else {
        locationChangeAnimator.setDuration(0);
      }
      locationChangeAnimator.addUpdateListener(new MarkerCoordinateAnimatorListener(this,
        latLng, newLocation
      ));
      locationChangeAnimator.start();
      latLng = newLocation;
    }

    @Override
    void invalidate() {
      if (latLng != null) {
        screenLocation = projection.toScreenLocation(latLng);
      }
      MyLocationView.this.invalidate();
    }
  }
}
