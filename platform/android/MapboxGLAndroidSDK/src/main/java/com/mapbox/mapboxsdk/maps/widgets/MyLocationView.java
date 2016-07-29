package com.mapbox.mapboxsdk.maps.widgets;

import android.animation.ValueAnimator;
import android.content.Context;
import android.graphics.Camera;
import android.graphics.Canvas;
import android.graphics.Color;
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

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.LocationListener;
import com.mapbox.mapboxsdk.location.LocationServices;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Projection;

import java.lang.ref.WeakReference;

/**
 * UI element overlaid on a map to show the user's location.
 */
public class MyLocationView extends View {

    private MyLocationBehavior myLocationBehavior;
    private MapboxMap mapboxMap;
    private Projection projection;
    private int[] contentPadding = new int[4];

    private Location location;
    private LatLng latLng;
    private LatLng interpolatedLocation;
    private LatLng previousLocation;
    private long locationUpdateTimestamp;

    private float gpsDirection;
    private float previousDirection;

    private float accuracy;
    private Paint accuracyPaint;

    private ValueAnimator locationChangeAnimator;
    private ValueAnimator accuracyAnimator;
    private ValueAnimator directionAnimator;

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

    private int foregroundTintColor;
    private int backgroundTintColor;

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
    private float bearing;
    private float tilt;

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
        if(isInEditMode()){
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

        if (defaultDrawable.getIntrinsicWidth() != bearingDrawable.getIntrinsicWidth() || defaultDrawable.getIntrinsicHeight() != bearingDrawable.getIntrinsicHeight()) {
            throw new RuntimeException("The dimensions from location and bearing drawables should be match");
        }

        foregroundDrawable = defaultDrawable;
        foregroundBearingDrawable = bearingDrawable;
        setForegroundDrawableTint(foregroundTintColor);

        invalidateBounds();
    }

    public final void setForegroundDrawableTint(@ColorInt int color) {
        if (color != Color.TRANSPARENT) {
            foregroundTintColor = color;
            if (foregroundDrawable != null) {
                foregroundDrawable.mutate().setColorFilter(color, PorterDuff.Mode.SRC_IN);
            }
            if (foregroundBearingDrawable != null) {
                foregroundBearingDrawable.mutate().setColorFilter(color, PorterDuff.Mode.SRC_IN);
            }
        }
        invalidate();
    }

    public final void setShadowDrawable(Drawable drawable) {
        setShadowDrawable(drawable, 0, 0, 0, 0);
    }

    public final void setShadowDrawable(Drawable drawable, int left, int top, int right, int bottom) {
        if (drawable != null) {
            backgroundDrawable = drawable;
        }

        backgroundOffsetLeft = left;
        backgroundOffsetTop = top;
        backgroundOffsetRight = right;
        backgroundOffsetBottom = bottom;
        setShadowDrawableTint(backgroundTintColor);

        invalidateBounds();
    }

    public final void setShadowDrawableTint(@ColorInt int color) {
        if (color != Color.TRANSPARENT) {
            backgroundTintColor = color;
            if (backgroundDrawable == null) {
                return;
            }
            backgroundDrawable.mutate().setColorFilter(color, PorterDuff.Mode.SRC_IN);
        }
        invalidate();
    }

    public final void setAccuracyTint(@ColorInt int color) {
        int alpha = accuracyPaint.getAlpha();
        accuracyPaint.setColor(color);
        accuracyPaint.setAlpha(alpha);
        invalidate();
    }

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
        backgroundBounds = new Rect(-backgroundWidth / 2 + horizontalOffset, -backgroundHeight / 2 + verticalOffset, backgroundWidth / 2 + horizontalOffset, backgroundHeight / 2 + verticalOffset);
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

        if (location == null || foregroundBounds == null || backgroundBounds == null || accuracyAnimator == null || screenLocation == null) {
            // Not ready yet
            return;
        }

        final PointF pointF = screenLocation;

        float metersPerPixel = (float) projection.getMetersPerPixelAtLatitude(location.getLatitude());
        float accuracyPixels = (Float) accuracyAnimator.getAnimatedValue() / metersPerPixel / 2;
        float maxRadius = getWidth() / 2;
        accuracyPixels = accuracyPixels <= maxRadius ? accuracyPixels : maxRadius;

        // put matrix in origin
        matrix.reset();

        // apply tilt to camera
        camera.save();
        camera.rotate(tilt, 0, bearing);

        // map camera matrix on our matrix
        camera.getMatrix(matrix);

        //
        if (myBearingTrackingMode != MyBearingTracking.NONE && directionAnimator != null) {
            matrix.preRotate((Float) directionAnimator.getAnimatedValue());
        }

        // put matrix at location of MyLocationView
        matrix.postTranslate(pointF.x, pointF.y);

        // concat our matrix on canvas
        canvas.concat(matrix);

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
            foregroundBearingDrawable.draw(canvas);
        }
    }

    public void setTilt(@FloatRange(from = 0, to = 60.0f) double tilt) {
        this.tilt = (float) tilt;
    }

    public void setBearing(double bearing) {
        this.bearing = (float) bearing;
    }

    public void setCameraPosition(CameraPosition position) {
        setTilt(position.tilt);
        setBearing(position.bearing);
    }

    public void onPause() {
        compassListener.onPause();
        toggleGps(false);
    }

    public void onResume() {
        if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
            compassListener.onResume();
        }
        if (isEnabled()) {
            toggleGps(true);
        }
    }

    public void update() {
        if (isEnabled()) {
            myLocationBehavior.invalidate();
        } else {
            setVisibility(View.INVISIBLE);
        }
    }

    public void setMapboxMap(MapboxMap mapboxMap) {
        this.mapboxMap = mapboxMap;
        this.projection = mapboxMap.getProjection();
    }

    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);
        setVisibility(enabled ? View.VISIBLE : View.INVISIBLE);
        toggleGps(enabled);
    }

    @Override
    protected Parcelable onSaveInstanceState() {
        Bundle bundle = new Bundle();
        bundle.putParcelable("superState", super.onSaveInstanceState());
        bundle.putFloat("tilt", tilt);
        return bundle;
    }

    @Override
    public void onRestoreInstanceState(Parcelable state){
        if (state instanceof Bundle){
            Bundle bundle = (Bundle) state;
            tilt = bundle.getFloat("tilt");
            state = bundle.getParcelable("superState");
        }
        super.onRestoreInstanceState(state);
    }

    /**
     * Enabled / Disable GPS location updates along with updating the UI
     *
     * @param enableGps true if GPS is to be enabled, false if GPS is to be disabled
     */
    private void toggleGps(boolean enableGps) {
        LocationServices locationServices = LocationServices.getLocationServices(getContext());
        if (enableGps) {
            // Set an initial location if one available
            Location lastLocation = locationServices.getLastLocation();

            if (lastLocation != null) {
                setLocation(lastLocation);
            }

            if (userLocationListener == null) {
                userLocationListener = new GpsLocationListener(this);
            }

            locationServices.addLocationListener(userLocationListener);
        } else {
            // Disable location and user dot
            location = null;
            locationServices.removeLocationListener(userLocationListener);
        }

        locationServices.toggleGPS(enableGps);
    }

    public Location getLocation() {
        return location;
    }

    public void setLocation(Location location) {
        if (location == null) {
            this.location = null;
            return;
        }

        this.location = location;
        myLocationBehavior.updateLatLng(location);
    }

    public void setMyBearingTrackingMode(@MyBearingTracking.Mode int myBearingTrackingMode) {
        this.myBearingTrackingMode = myBearingTrackingMode;
        if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
            compassListener.onResume();
        } else {
            compassListener.onPause();
            if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
                // always face north
                gpsDirection = bearing;
                setCompass(gpsDirection);
            }
        }
        invalidate();
        update();
    }

    public void setMyLocationTrackingMode(@MyLocationTracking.Mode int myLocationTrackingMode) {
        this.myLocationTrackingMode = myLocationTrackingMode;

        MyLocationBehaviorFactory factory = new MyLocationBehaviorFactory();
        myLocationBehavior = factory.getBehavioralModel(myLocationTrackingMode);

        if (myLocationTrackingMode != MyLocationTracking.TRACKING_NONE && location != null) {
            // center map directly if we have a location fix
            myLocationBehavior.updateLatLng(location);
            mapboxMap.moveCamera(CameraUpdateFactory.newLatLng(new LatLng(location)));
        }
        invalidate();
        update();
    }

    private void setCompass(float bearing) {
        float oldDir = previousDirection;
        if (directionAnimator != null) {
            oldDir = (Float) directionAnimator.getAnimatedValue();
            directionAnimator.end();
            directionAnimator = null;
        }

        float newDir = bearing;
        float diff = oldDir - newDir;
        if (diff > 180.0f) {
            newDir += 360.0f;
        } else if (diff < -180.0f) {
            newDir -= 360.f;
        }
        previousDirection = newDir;

        directionAnimator = ValueAnimator.ofFloat(oldDir, newDir);
        directionAnimator.setDuration(375);
        directionAnimator.addUpdateListener(invalidateSelfOnUpdateListener);
        directionAnimator.start();
    }

    public float getCenterX() {
        return getX() + getMeasuredWidth() / 2;
    }

    public float getCenterY() {
        return getY() + getMeasuredHeight() / 2;
    }

    public void setContentPadding(int[] padding) {
        contentPadding = padding;
    }

    private static class GpsLocationListener implements LocationListener {

        private WeakReference<MyLocationView> mUserLocationView;

        public GpsLocationListener(MyLocationView myLocationView) {
            mUserLocationView = new WeakReference<>(myLocationView);
        }

        /**
         * Callback method for receiving location updates from LocationServices.
         *
         * @param location The new Location data
         */
        @Override
        public void onLocationChanged(Location location) {
            MyLocationView locationView = mUserLocationView.get();
            if (locationView != null) {
                locationView.setLocation(location);
            }
        }
    }

    private class CompassListener implements SensorEventListener {

        private SensorManager mSensorManager;
        private Sensor mAccelerometer;
        private Sensor mMagnetometer;
        private boolean paused;

        private float mCurrentDegree = 0f;

        private float[] mOrientation = new float[3];
        private float[] mGData = new float[3];
        private float[] mMData = new float[3];
        private float[] mR = new float[16];
        private float[] mI = new float[16];

        // Controls the sensor updateLatLng rate in milliseconds
        private static final int UPDATE_RATE_MS = 500;

        // Compass data
        private long mCompassUpdateNextTimestamp = 0;

        public CompassListener(Context context) {
            mSensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
            mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
            mMagnetometer = mSensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
        }

        public void onResume() {
            paused = false;
            mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_GAME);
            mSensorManager.registerListener(this, mMagnetometer, SensorManager.SENSOR_DELAY_GAME);
        }

        public void onPause() {
            paused = true;
            mSensorManager.unregisterListener(this, mAccelerometer);
            mSensorManager.unregisterListener(this, mMagnetometer);
        }

        @Override
        public void onSensorChanged(SensorEvent event) {
            if (paused) {
                return;
            }

            int type = event.sensor.getType();
            if (type == Sensor.TYPE_ACCELEROMETER) {
                System.arraycopy(event.values, 0, mGData, 0, 3);
            } else if (type == Sensor.TYPE_MAGNETIC_FIELD) {
                System.arraycopy(event.values, 0, mMData, 0, 3);
            } else {
                // we should not be here.
                return;
            }

            long currentTime = SystemClock.elapsedRealtime();
            if (currentTime < mCompassUpdateNextTimestamp) {
                return;
            }

            SensorManager.getRotationMatrix(mR, mI, mGData, mMData);
            SensorManager.getOrientation(mR, mOrientation);
            setCompass(mCurrentDegree = (int) (mOrientation[0] * 180.0f / Math.PI));
            mCompassUpdateNextTimestamp = currentTime + UPDATE_RATE_MS;
        }

        public float getCurrentDegree() {
            return mCurrentDegree;
        }

        public boolean isPaused() {
            return paused;
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

        public MyLocationBehavior getBehavioralModel(@MyLocationTracking.Mode int mode) {
            if (mode == MyLocationTracking.TRACKING_NONE) {
                return new MyLocationShowBehavior();
            } else {
                return new MyLocationTrackingBehavior();
            }
        }
    }

    private abstract class MyLocationBehavior {

        abstract void updateLatLng(@NonNull Location location);

        public void updateLatLng(double lat, double lon) {
            if (latLng != null) {
                latLng.setLatitude(lat);
                latLng.setLongitude(lon);
            }
        }

        protected void updateAccuracy(@NonNull Location location) {
            if (accuracyAnimator != null && accuracyAnimator.isRunning()) {
                // use current accuracy as a starting point
                accuracy = (Float) accuracyAnimator.getAnimatedValue();
                accuracyAnimator.end();
            }

            accuracyAnimator = ValueAnimator.ofFloat(accuracy * 10, location.getAccuracy() * 10);
            accuracyAnimator.setDuration(750);
            accuracyAnimator.start();
            accuracy = location.getAccuracy();
        }

        abstract void invalidate();
    }

    private class MyLocationTrackingBehavior extends MyLocationBehavior {

        @Override
        void updateLatLng(@NonNull Location location) {
            if (latLng == null) {
                // first location fix
                latLng = new LatLng(location);
                locationUpdateTimestamp = SystemClock.elapsedRealtime();
            }

            // updateLatLng timestamp
            long previousUpdateTimeStamp = locationUpdateTimestamp;
            locationUpdateTimestamp = SystemClock.elapsedRealtime();

            // calculate animation duration
            long locationUpdateDuration;
            if (previousUpdateTimeStamp == 0) {
                locationUpdateDuration = 0;
            } else {
                locationUpdateDuration = locationUpdateTimestamp - previousUpdateTimeStamp;
            }

            // calculate interpolated location
            previousLocation = latLng;
            latLng = new LatLng(location);
            interpolatedLocation = new LatLng((latLng.getLatitude() + previousLocation.getLatitude()) / 2, (latLng.getLongitude() + previousLocation.getLongitude()) / 2);

            // build new camera
            CameraPosition.Builder builder = new CameraPosition.Builder().target(interpolatedLocation);

            // add direction
            if (myBearingTrackingMode == MyBearingTracking.GPS) {
                if (location.hasBearing()) {
                    builder.bearing(location.getBearing());
                }
                gpsDirection = location.getBearing();
                setCompass(gpsDirection);
            } else if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
                if (!compassListener.isPaused()) {
                    builder.bearing(compassListener.getCurrentDegree());
                    setCompass(0);
                }
            }

            // accuracy
            updateAccuracy(location);

            // ease to new camera position with a linear interpolator
            mapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(builder.build()), (int) locationUpdateDuration, false /*linear interpolator*/);
        }

        @Override
        void invalidate() {
            int[] mapPadding = mapboxMap.getPadding();
            float x = (getWidth() + mapPadding[0] - mapPadding[2]) / 2 + (contentPadding[0] - contentPadding[2]) / 2;
            float y = (getHeight() - mapPadding[3] + mapPadding[1]) / 2 + (contentPadding[1] - contentPadding[3]) / 2;
            screenLocation = new PointF(x, y);
            MyLocationView.this.invalidate();
        }
    }

    private class MyLocationShowBehavior extends MyLocationBehavior {

        @Override
        void updateLatLng(@NonNull final Location location) {
            if (latLng == null) {
                // first location update
                latLng = new LatLng(location);
                locationUpdateTimestamp = SystemClock.elapsedRealtime();
            }

            // update LatLng location
            previousLocation = latLng;
            latLng = new LatLng(location);

            // update LatLng direction
            if (myBearingTrackingMode == MyBearingTracking.GPS && location.hasBearing()) {
                gpsDirection = location.getBearing();
                setCompass(gpsDirection);
            }

            // update LatLng accuracy
            updateAccuracy(location);

            // calculate updateLatLng time + add some extra offset to improve animation
            long previousUpdateTimeStamp = locationUpdateTimestamp;
            locationUpdateTimestamp = SystemClock.elapsedRealtime();
            long locationUpdateDuration = (long) ((locationUpdateTimestamp - previousUpdateTimeStamp) * 1.3);

            // calculate interpolated entity
            interpolatedLocation = new LatLng((latLng.getLatitude() + previousLocation.getLatitude()) / 2, (latLng.getLongitude() + previousLocation.getLongitude()) / 2);

            // animate changes
            if (locationChangeAnimator != null) {
                locationChangeAnimator.end();
                locationChangeAnimator = null;
            }

            locationChangeAnimator = ValueAnimator.ofFloat(0.0f, 1.0f);
            locationChangeAnimator.setDuration((long) (locationUpdateDuration * 1.2));
            locationChangeAnimator.addUpdateListener(new MarkerCoordinateAnimatorListener(this,
                    previousLocation, interpolatedLocation
            ));
            locationChangeAnimator.start();

            // use interpolated location as current location
            latLng = interpolatedLocation;
        }

        @Override
        void invalidate() {
            screenLocation = projection.toScreenLocation(latLng);
            MyLocationView.this.invalidate();
        }
    }
}
