package com.mapbox.mapboxsdk.maps.widgets;

import android.animation.ValueAnimator;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.PointF;
import android.graphics.PorterDuff;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.LayerDrawable;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Location;
import android.os.SystemClock;
import android.support.annotation.ColorInt;
import android.support.annotation.FloatRange;
import android.support.annotation.IntRange;
import android.support.annotation.NonNull;
import android.support.v4.content.ContextCompat;
import android.support.v4.view.animation.FastOutLinearInInterpolator;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.LocationListener;
import com.mapbox.mapboxsdk.location.LocationServices;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Projection;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.utils.ColorUtils;

import java.lang.ref.WeakReference;

/**
 * UI element overlaid on a map to show the user's location.
 */
public class MyLocationView extends View {

    private MyLocationBehaviour myLocationBehaviour;
    private MapboxMap mapboxMap;
    private Projection projection;
    private int maxSize;
    private int[] contentPadding = new int[4];

    private Location location;
    private LatLng latLng;
    private LatLng interpolatedLocation;
    private LatLng previousLocation;
    private long locationUpdateTimestamp;

    private float gpsDirection;
    private float compassDirection;
    private float previousDirection;

    private float accuracy = 0;
    private Paint accuracyPaint = new Paint();

    private ValueAnimator locationChangeAnimator;
    private ValueAnimator accuracyAnimator;

    private Drawable foregroundDrawable;
    private Drawable foregroundBearingDrawable;
    private Drawable backgroundDrawable;

    private Rect foregroundBounds;
    private Rect backgroundBounds;

    private int backgroundOffsetLeft;
    private int backgroundOffsetTop;
    private int backgroundOffsetRight;
    private int backgroundOffsetBottom;

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
        setEnabled(false);

        // behavioural model
        myLocationBehaviour = new MyLocationBehaviourFactory().getBehaviouralModel(MyLocationTracking.TRACKING_NONE);
        compassListener = new CompassListener(context);
        maxSize = (int) context.getResources().getDimension(R.dimen.userlocationview_size);

        // default implementation
        setShadowDrawable(ContextCompat.getDrawable(context, R.drawable.ic_userlocationview_shadow));
        setForegroundDrawables(ContextCompat.getDrawable(context, R.drawable.ic_userlocationview_normal), ContextCompat.getDrawable(context, R.drawable.ic_userlocationview_bearing));
        setAccuracyTint(ColorUtils.getAccentColor(context));
        setAccuracyAlpha(100);
    }

    public final void setForegroundDrawables(Drawable defaultDrawable, Drawable bearingDrawable) {
        if (defaultDrawable == null || bearingDrawable == null) {
            Log.e(MapboxConstants.TAG, "Not setting foreground drawables MyLocationView: default = " + defaultDrawable + " bearing = " + bearingDrawable);
            return;
        }
        if (defaultDrawable.getIntrinsicWidth() != bearingDrawable.getIntrinsicWidth() || defaultDrawable.getIntrinsicHeight() != bearingDrawable.getIntrinsicHeight()) {
            throw new RuntimeException("The dimensions from location and bearing drawables should be match");
        }
        foregroundDrawable = defaultDrawable;
        foregroundBearingDrawable = bearingDrawable;
        invalidateBounds();
    }

    public final void setForegroundDrawableTint(@ColorInt int color) {
        if (foregroundDrawable != null) {
            foregroundDrawable.mutate().setColorFilter(color, PorterDuff.Mode.SRC_IN);
        }
        if (foregroundBearingDrawable != null) {
            foregroundBearingDrawable.mutate().setColorFilter(color, PorterDuff.Mode.SRC_IN);
        }
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

        invalidateBounds();
    }

    public final void setShadowDrawableTint(@ColorInt int color) {
        if (backgroundDrawable == null) {
            return;
        }

        backgroundDrawable.mutate().setColorFilter(color, PorterDuff.Mode.SRC_IN);
    }

    public final void setAccuracyTint(@ColorInt int color) {
        int alpha = accuracyPaint.getAlpha();
        accuracyPaint.setColor(color);
        accuracyPaint.setAlpha(alpha);
    }

    public final void setAccuracyAlpha(@IntRange(from = 0, to = 255) int alpha) {
        accuracyPaint.setAlpha(alpha);
    }

    private void invalidateBounds() {
        if (backgroundDrawable == null || foregroundDrawable == null || foregroundBearingDrawable == null) {
            return;
        }

        int backgroundWidth = backgroundDrawable.getIntrinsicWidth();
        int backgroundHeight = backgroundDrawable.getIntrinsicHeight();

        int foregroundWidth = foregroundDrawable.getIntrinsicWidth();
        int foregroundHeight = foregroundDrawable.getIntrinsicHeight();

        int horizontalOffset = backgroundOffsetLeft - backgroundOffsetRight;
        int verticalOffset = backgroundOffsetTop - backgroundOffsetBottom;

        int accuracyWidth = 2 * maxSize;

        backgroundBounds = new Rect(accuracyWidth - (backgroundWidth / 2) + horizontalOffset, accuracyWidth + verticalOffset - (backgroundWidth / 2), accuracyWidth + (backgroundWidth / 2) + horizontalOffset, accuracyWidth + (backgroundHeight / 2) + verticalOffset);
        foregroundBounds = new Rect(accuracyWidth - (foregroundWidth / 2), accuracyWidth - (foregroundHeight / 2), accuracyWidth + (foregroundWidth / 2), accuracyWidth + (foregroundHeight / 2));

        // invoke a new measure
        invalidate();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (location == null || foregroundBounds == null || backgroundBounds == null || accuracyAnimator == null) {
            // Not ready yet
            return;
        }

        // Draw circle
        float metersPerPixel = (float) projection.getMetersPerPixelAtLatitude(location.getLatitude());
        float accuracyPixels = (Float) accuracyAnimator.getAnimatedValue() / metersPerPixel;
        float maxRadius = getWidth() / 2;
        canvas.drawCircle(foregroundBounds.centerX(), foregroundBounds.centerY(), accuracyPixels <= maxRadius ? accuracyPixels : maxRadius, accuracyPaint);

        // Draw shadow
        if (backgroundDrawable != null) {
            backgroundDrawable.draw(canvas);
        }

        // Draw foreground
        if (myBearingTrackingMode == MyBearingTracking.NONE) {
            if (foregroundDrawable != null) {
                foregroundDrawable.draw(canvas);
            }
        } else if (foregroundBearingDrawable != null && foregroundBounds != null) {
            getRotateDrawable(foregroundBearingDrawable, myBearingTrackingMode == MyBearingTracking.COMPASS ? compassDirection : gpsDirection).draw(canvas);
        }
    }

    private Drawable getRotateDrawable(final Drawable d, final float angle) {
        return new LayerDrawable(new Drawable[]{d}) {
            @Override
            public void draw(final Canvas canvas) {
                canvas.save();
                canvas.rotate(angle, foregroundBounds.centerX(), foregroundBounds.centerY());
                super.draw(canvas);
                canvas.restore();
            }
        };
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);

        if (foregroundDrawable != null && foregroundBounds != null) {
            foregroundDrawable.setBounds(foregroundBounds);
        }

        if (foregroundBearingDrawable != null && foregroundBounds != null) {
            foregroundBearingDrawable.setBounds(foregroundBounds);
        }

        if (backgroundDrawable != null && backgroundBounds != null) {
            backgroundDrawable.setBounds(backgroundBounds);
        }

        setMeasuredDimension(4 * maxSize, 4 * maxSize);
    }

    public void setTilt(@FloatRange(from = 0, to = 60.0f) double tilt) {
        setRotationX((float) tilt);
    }

    void updateOnNextFrame() {
        mapboxMap.invalidate();
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
            myLocationBehaviour.invalidate();
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
        myLocationBehaviour.updateLatLng(location);
    }

    public void setMyBearingTrackingMode(@MyBearingTracking.Mode int myBearingTrackingMode) {
        this.myBearingTrackingMode = myBearingTrackingMode;
        if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
            compassListener.onResume();
        } else {
            compassListener.onPause();
            if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
                // always face north
                gpsDirection = 0;
            }
        }
        invalidate();
        update();
    }

    public void setMyLocationTrackingMode(@MyLocationTracking.Mode int myLocationTrackingMode) {
        this.myLocationTrackingMode = myLocationTrackingMode;

        MyLocationBehaviourFactory factory = new MyLocationBehaviourFactory();
        myLocationBehaviour = factory.getBehaviouralModel(myLocationTrackingMode);

        if (myLocationTrackingMode != MyLocationTracking.TRACKING_NONE && location != null) {
            // center map directly if we have a location fix
            myLocationBehaviour.updateLatLng(location);
            mapboxMap.moveCamera(CameraUpdateFactory.newLatLng(new LatLng(location)));
        }
        invalidate();
        update();
    }

    private void setCompass(float bearing) {
        if (myLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
            float oldDir = compassDirection;
            float newDir = bearing;
            float diff = oldDir - newDir;
            if (diff > 180.0f) {
                newDir += 360.0f;
            } else if (diff < -180.0f) {
                newDir -= 360.f;
            }
            compassDirection = newDir;
            invalidate();
        } else {
            compassDirection = 0;
        }
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

        private boolean paused;
        private SensorManager mSensorManager;
        private Sensor mAccelerometer;
        private Sensor mMagnetometer;
        private float[] mLastAccelerometer = new float[3];
        private float[] mLastMagnetometer = new float[3];
        private boolean mLastAccelerometerSet = false;
        private boolean mLastMagnetometerSet = false;
        private float[] mR = new float[9];
        private float[] mOrientation = new float[3];
        private float mCurrentDegree = 0f;

        // Controls the sensor updateLatLng rate in milliseconds
        private static final int UPDATE_RATE_MS = 300;

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

            long currentTime = SystemClock.elapsedRealtime();
            if (currentTime < mCompassUpdateNextTimestamp) {
                return;
            }

            if (event.sensor == mAccelerometer) {
                System.arraycopy(event.values, 0, mLastAccelerometer, 0, event.values.length);
                mLastAccelerometerSet = true;
            } else if (event.sensor == mMagnetometer) {
                System.arraycopy(event.values, 0, mLastMagnetometer, 0, event.values.length);
                mLastMagnetometerSet = true;
            }

            if (mLastAccelerometerSet && mLastMagnetometerSet) {
                SensorManager.getRotationMatrix(mR, null, mLastAccelerometer, mLastMagnetometer);
                SensorManager.getOrientation(mR, mOrientation);
                float azimuthInRadians = mOrientation[0];

                float compassBearing = (float) (Math.toDegrees(azimuthInRadians) + 360) % 360;
                if (compassBearing < 0) {
                    // only allow positive degrees
                    compassBearing += 360;
                }

                if (compassBearing > mCurrentDegree + 15 || compassBearing < mCurrentDegree - 15) {
                    mCurrentDegree = compassBearing;
                    setCompass(mCurrentDegree);
                }
            }
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

        private MyLocationBehaviour behaviour;
        private double fromLat;
        private double fromLng;
        private double toLat;
        private double toLng;

        private MarkerCoordinateAnimatorListener(MyLocationBehaviour myLocationBehaviour, LatLng from, LatLng to) {
            behaviour = myLocationBehaviour;
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
            behaviour.updateLatLng(latitude, longitude);
            updateOnNextFrame();
        }
    }

    private class MyLocationBehaviourFactory {

        public MyLocationBehaviour getBehaviouralModel(@MyLocationTracking.Mode int mode) {
            if (mode == MyLocationTracking.TRACKING_NONE) {
                return new MyLocationShowBehaviour();
            } else {
                return new MyLocationTrackingBehaviour();
            }
        }
    }

    private abstract class MyLocationBehaviour {

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

    private class MyLocationTrackingBehaviour extends MyLocationBehaviour {

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
                // TODO remove 10 * hack to multiply duration to workaround easing interpolation (easeCamera)
                locationUpdateDuration = 10 * (locationUpdateTimestamp - previousUpdateTimeStamp);
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
                    gpsDirection = 0;
                }
            } else if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
                if (!compassListener.isPaused()) {
                    builder.bearing(compassListener.getCurrentDegree());
                    compassDirection = 0;
                }
            }

            updateAccuracy(location);

            // animate to new camera
            mapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(builder.build()), (int) locationUpdateDuration, null);
        }

        @Override
        void invalidate() {
            int[] mapPadding = mapboxMap.getPadding();
            UiSettings uiSettings = mapboxMap.getUiSettings();
            setX((uiSettings.getWidth() - getWidth() + mapPadding[0] - mapPadding[2]) / 2 + (contentPadding[0] - contentPadding[2]) / 2);
            setY((uiSettings.getHeight() - getHeight() - mapPadding[3] + mapPadding[1]) / 2 + (contentPadding[1] - contentPadding[3]) / 2);
            MyLocationView.this.invalidate();
        }
    }

    private class MyLocationShowBehaviour extends MyLocationBehaviour {

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
            if (location.hasBearing()) {
                gpsDirection = clamp(location.getBearing() - (float) mapboxMap.getCameraPosition().bearing);
            }

            // update LatLng accuracy
            updateAccuracy(location);

            // calculate updateLatLng time + add some extra offset to improve animation
            long previousUpdateTimeStamp = locationUpdateTimestamp;
            locationUpdateTimestamp = SystemClock.elapsedRealtime();
            long locationUpdateDuration = (long) ((locationUpdateTimestamp - previousUpdateTimeStamp) * 1.2);

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
            locationChangeAnimator.setInterpolator(new FastOutLinearInInterpolator());
            locationChangeAnimator.start();

            // use interpolated location as current location
            latLng = interpolatedLocation;
        }

        private float clamp(float direction) {
            float diff = previousDirection - direction;
            if (diff > 180.0f) {
                direction += 360.0f;
            } else if (diff < -180.0f) {
                direction -= 360.f;
            }
            previousDirection = direction;
            return direction;
        }

        @Override
        void invalidate() {
            PointF screenLocation = projection.toScreenLocation(latLng);
            if (screenLocation != null) {
                setX((screenLocation.x - getWidth() / 2));
                setY((screenLocation.y - getHeight() / 2));
            }
            MyLocationView.this.invalidate();
        }
    }
}
