package com.mapbox.mapboxsdk.maps.widgets;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
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
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.view.animation.Animation;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.LocationListener;
import com.mapbox.mapboxsdk.location.LocationServices;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Projection;
import com.mapbox.mapboxsdk.maps.UiSettings;

import java.lang.ref.WeakReference;

/**
 * UI element overlaid on a map to show the user's location.
 */
public class UserLocationView extends View {

    private LatLng latLng;
    private float gpsDirection;
    private float compassDirection;

    private Drawable foregroundDrawable;
    private Drawable foregroundBearingDrawable;
    private Drawable backgroundDrawable;

    private int[] contentPadding = new int[4];

    private MyBearingListener mBearingChangeListener;

    // Accuracy
    private Paint accuracyPaint;
    private int maxAccuracy = 100;
    private float accuracy = 0;

    private Rect foregroundBounds;
    private Rect backgroundBounds;

    private int backgroundOffsetLeft;
    private int backgroundOffsetTop;
    private int backgroundOffsetRight;
    private int backgroundOffsetBottom;

    private UserLocationBehaviour userLocationBehaviour;

    @MyLocationTracking.Mode
    private int myLocationTrackingMode;

    @MyBearingTracking.Mode
    private int myBearingTrackingMode;

    private MapboxMap mapboxMap;
    private Projection projection;

    private Location location;
    private UserLocationListener userLocationListener;

    private ValueAnimator locationChangeAnimator;
    private MarkerCoordinateAnimatorListener markerCoordinateAnimatorListener;
    public LatLng interpolatedLocation;
    public LatLng previousLocation;

    private float previousDirection;

    private long locationUpdateTimestamp;
    private long locationUpdateDuration;

    public UserLocationView(Context context) {
        super(context);
        init(context);
    }

    public UserLocationView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    public UserLocationView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context);
    }

    private void init(Context context) {
        setEnabled(false);

        userLocationBehaviour = new UserLocationBehaviourFactory().getBehaviouralModel(MyLocationTracking.TRACKING_NONE);
        markerCoordinateAnimatorListener = new MarkerCoordinateAnimatorListener(userLocationBehaviour);

        accuracyPaint = new Paint();
        accuracyPaint.setColor(ContextCompat.getColor(context, R.color.mapbox_blue));
        accuracyPaint.setAlpha(100);

        setShadowDrawable(ContextCompat.getDrawable(context, R.drawable.ic_userlocationview_shadow));
        setForegroundDrawables(ContextCompat.getDrawable(context, R.drawable.ic_userlocationview_normal), ContextCompat.getDrawable(context, R.drawable.ic_userlocationview_bearing));
        mBearingChangeListener = new MyBearingListener(context);
    }

    public final void setForegroundDrawables(Drawable defaultDrawable, Drawable bearingDrawable) {
        if (defaultDrawable.getIntrinsicWidth() != bearingDrawable.getIntrinsicWidth() || defaultDrawable.getIntrinsicHeight() != bearingDrawable.getIntrinsicHeight()) {
            throw new RuntimeException("Default and bearing drawable dimensions should be match");
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
        backgroundDrawable = drawable;
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
        accuracyPaint.setColor(color);
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

        int accurancyWidth = 2 * maxAccuracy;

        backgroundBounds = new Rect(accurancyWidth - (backgroundWidth / 2) + horizontalOffset, accurancyWidth + verticalOffset - (backgroundWidth / 2), accurancyWidth + (backgroundWidth / 2) + horizontalOffset, accurancyWidth + (backgroundHeight / 2) + verticalOffset);
        foregroundBounds = new Rect(accurancyWidth - (foregroundWidth / 2), accurancyWidth - (foregroundHeight / 2), accurancyWidth + (foregroundWidth / 2), accurancyWidth + (foregroundHeight / 2));

        // invoke a new measure
        invalidate();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        // Not ready yet
        if (foregroundBounds == null || backgroundBounds == null) {
            return;
        }

        // Draw circle
        canvas.drawCircle(foregroundBounds.centerX(), foregroundBounds.centerY(), foregroundBounds.width() / 2 + (accuracy / 100 * (getWidth() - foregroundBounds.width()) / 2), accuracyPaint);

        // Draw shadow
        if (backgroundDrawable != null) {
            backgroundDrawable.draw(canvas);
        }

        // Draw foreground
        if (myBearingTrackingMode == MyBearingTracking.NONE) {
            if (foregroundDrawable != null) {
                foregroundDrawable.draw(canvas);
            }
        } else {
            // todo optimise
            if (foregroundBearingDrawable != null && foregroundBounds != null) {
                if (myBearingTrackingMode == MyBearingTracking.GPS) {
                    getRotateDrawable(foregroundBearingDrawable, gpsDirection).draw(canvas);
                } else {
                    getRotateDrawable(foregroundBearingDrawable, compassDirection).draw(canvas);
                }
            }
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

        setMeasuredDimension(4 * maxAccuracy, 4 * maxAccuracy);
    }

    public void setTilt(@FloatRange(from = 0, to = 60.0f) double tilt) {
        setRotationX((float) tilt);
    }

    public void setAccuracy(float accuracy) {
        this.accuracy = accuracy;
        invalidate();
    }

    void updateOnNextFrame() {
        mapboxMap.invalidate();
    }

    public void onPause() {
        mBearingChangeListener.onPause();
        toggleGps(false);
    }

    public void onResume() {
        if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
            mBearingChangeListener.onResume();
        }
        if (isEnabled()) {
            toggleGps(true);
        }
    }

    public void update() {
        if (isEnabled()) {
            userLocationBehaviour.invalidate();
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
                location = lastLocation;
            }

            if (userLocationListener == null) {
                userLocationListener = new UserLocationListener(this);
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
        userLocationBehaviour.update(location);
    }

    public void setMyBearingTrackingMode(@MyBearingTracking.Mode int myBearingTrackingMode) {
        this.myBearingTrackingMode = myBearingTrackingMode;
        if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
            mBearingChangeListener.onResume();
        } else {
            mBearingChangeListener.onPause();
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

        UserLocationBehaviourFactory factory = new UserLocationBehaviourFactory();
        userLocationBehaviour = factory.getBehaviouralModel(myLocationTrackingMode);

        if (myLocationTrackingMode != MyLocationTracking.TRACKING_NONE && location != null) {
            // center map directly if we have a location fix
            userLocationBehaviour.update(location);
            mapboxMap.moveCamera(CameraUpdateFactory.newLatLng(new LatLng(location)));
        }
        invalidate();
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

    private static class UserLocationListener implements LocationListener {

        private WeakReference<UserLocationView> mUserLocationView;

        public UserLocationListener(UserLocationView userLocationView) {
            mUserLocationView = new WeakReference<>(userLocationView);
        }


        /**
         * Callback method for receiving location updates from LocationServices.
         *
         * @param location The new Location data
         */
        @Override
        public void onLocationChanged(Location location) {
            UserLocationView locationView = mUserLocationView.get();
            if (locationView != null) {
                locationView.setLocation(location);
            }
        }
    }

    private class MyBearingListener implements SensorEventListener {

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

        // Controls the sensor update rate in milliseconds
        private static final int UPDATE_RATE_MS = 300;

        // Compass data
        private float mCompassBearing;
        private long mCompassUpdateNextTimestamp = 0;

        public MyBearingListener(Context context) {
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

                mCompassBearing = (float) (Math.toDegrees(azimuthInRadians) + 360) % 360;
                if (mCompassBearing < 0) {
                    // only allow positive degrees
                    mCompassBearing += 360;
                }

                if (mCompassBearing > mCurrentDegree + 15 || mCompassBearing < mCurrentDegree - 15) {
                    mCurrentDegree = mCompassBearing;
                    setCompass(mCurrentDegree);
                }
            }
            mCompassUpdateNextTimestamp = currentTime + UPDATE_RATE_MS;
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {
            // TODO add accuracy to the equiation
        }
    }

    private class MarkerCoordinateAnimatorListener implements ValueAnimator.AnimatorUpdateListener {

        private UserLocationBehaviour behaviour;
        private double mFromLat;
        private double mFromLng;
        private double mToLat;
        private double mToLng;

        private double mCurrentLat;
        private double mCurrentLon;

        private MarkerCoordinateAnimatorListener(UserLocationBehaviour behaviour) {
            this.behaviour = behaviour;
        }

        @Override
        public void onAnimationUpdate(ValueAnimator animation) {
            float frac = animation.getAnimatedFraction();
            double latitude = mFromLat + (mToLat - mFromLat) * frac;
            double longitude = mFromLng + (mToLng - mFromLng) * frac;
            behaviour.update(latitude, longitude);
            mCurrentLat = latitude;
            mCurrentLon = longitude;
            updateOnNextFrame();
        }

        public LatLng getLatLng() {
            return new LatLng(mCurrentLat, mCurrentLon);
        }

        public void update() {
            mFromLat = mCurrentLat;
            mFromLng = mCurrentLon;
            mToLat = interpolatedLocation.getLatitude();
            mToLng = interpolatedLocation.getLongitude();
        }
    }

    private class UserLocationBehaviourFactory {

        public UserLocationBehaviour getBehaviouralModel(@MyLocationTracking.Mode int mode) {
            if (mode == MyLocationTracking.TRACKING_NONE) {
                return new UserLocationShowBehaviour();
            } else {
                return new UserLocationTrackingBehaviour();
            }
        }
    }

    private abstract class UserLocationBehaviour {

        abstract void update(@NonNull Location location);

        public void update(double lat, double lon) {
            if (latLng != null) {
                latLng.setLatitude(lat);
                latLng.setLongitude(lon);
            }
        }

        abstract void invalidate();
    }

    private class UserLocationTrackingBehaviour extends UserLocationBehaviour {

        @Override
        void update(@NonNull Location location) {
            if (latLng == null) {
                // first location fix
                latLng = new LatLng(location);
                locationUpdateTimestamp = SystemClock.elapsedRealtime();
                return;
            }

            // update timestamp
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
            if (myBearingTrackingMode == MyBearingTracking.GPS){
                if(location.hasBearing()) {
                    builder.bearing(location.getBearing());
                    gpsDirection = 0;
                }
            }

            // add accuracy
            if (location.hasAccuracy()) {
                accuracy = location.getAccuracy();
            }

            // animate to new camera
            mapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(builder.build()), (int) locationUpdateDuration, null);
        }

        @Override
        void invalidate() {
            int[] mapPadding = mapboxMap.getPadding();
            UiSettings uiSettings = mapboxMap.getUiSettings();
            setX((uiSettings.getWidth() - getWidth() + mapPadding[0] - mapPadding[2]) / 2 + (contentPadding[0] - contentPadding[2]) / 2);
            setY((uiSettings.getHeight() - getHeight() - mapPadding[3] + mapPadding[1]) / 2 + (contentPadding[1] - contentPadding[3]) / 2);
        }
    }

    private class UserLocationShowBehaviour extends UserLocationBehaviour {

        @Override
        void update(@NonNull final Location location) {

            if (latLng == null) {
                // first location update
                latLng = new LatLng(location);
                locationUpdateTimestamp = SystemClock.elapsedRealtime();
                return;
            }

            // update location
            previousLocation = latLng;
            latLng = new LatLng(location);

            // calculate update time
            long previousUpdateTimeStamp = locationUpdateTimestamp;
            locationUpdateTimestamp = SystemClock.elapsedRealtime();
            locationUpdateDuration = locationUpdateTimestamp - previousUpdateTimeStamp;

            // calculate interpolated entity
            interpolatedLocation = new LatLng((latLng.getLatitude() + previousLocation.getLatitude()) / 2, (latLng.getLongitude() + previousLocation.getLongitude()) / 2);

            Log.v("TAG", "LOCATIONS " + previousLocation + " " + interpolatedLocation);


            if (locationChangeAnimator == null) {
                Log.v("TAG", "START");
                locationChangeAnimator = ValueAnimator.ofFloat(1.0f);
//                locationChangeAnimator.setRepeatMode(ValueAnimator.RESTART);
//                locationChangeAnimator.setRepeatCount(ValueAnimator.INFINITE);
                locationChangeAnimator.setDuration(locationUpdateDuration + 500);

               markerCoordinateAnimatorListener  = new MarkerCoordinateAnimatorListener(this);
                markerCoordinateAnimatorListener.update();
                locationChangeAnimator.addListener(new AnimatorListenerAdapter() {
                    @Override
                    public void onAnimationEnd(Animator animation) {
                        super.onAnimationEnd(animation);
                        Log.v("TAG", "END");
                        markerCoordinateAnimatorListener.update();
                    }

                    @Override
                    public void onAnimationRepeat(Animator animation) {
                        super.onAnimationRepeat(animation);
                        Log.v("TAG", "REPEAT@");
                        markerCoordinateAnimatorListener.update();
                    }

                    @Override
                    public void onAnimationStart(Animator animation) {
                        super.onAnimationStart(animation);
                        Log.v("TAG","START");
                    }
                });
                locationChangeAnimator.addUpdateListener(markerCoordinateAnimatorListener);
            } else {
                Log.v("TAG", "CONTINUE");
                markerCoordinateAnimatorListener.update();
//                long currentPlayTime = locationChangeAnimator.getCurrentPlayTime();
//                locationChangeAnimator.setDuration(locationChangeAnimator.getDuration()+locationUpdateTimestamp);
//                locationChangeAnimator.setCurrentPlayTime(currentPlayTime);
            }

            if (location.hasBearing()) {
                gpsDirection = clamp(location.getBearing() - (float) mapboxMap.getCameraPosition().bearing);
            }

            if (location.hasAccuracy()) {
                accuracy = location.getAccuracy();
            }

            if (!locationChangeAnimator.isRunning()) {
                locationChangeAnimator.start();
            }

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
        }
    }

    public class Anim extends Animation{

    }
}
