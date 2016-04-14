package com.mapbox.mapboxsdk.maps.widgets;

import android.animation.ObjectAnimator;
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
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.util.AttributeSet;
import android.view.View;
import android.widget.Toast;

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
public final class UserLocationView extends View {

    protected LatLng latLng;
    protected PointF screenLocation;

    // Foreground
    private Drawable foregroundRotatedDrawable;
    private Drawable foregroundDrawable;

    private float compassDirection;
    private MyBearingListener mBearingChangeListener;
//    private double mCurrentBearing;
//    private float gpsDirection;

    // Shadow
    private Drawable backgroundDrawable;

    // Accuracy
    private Paint accurancyPaint;
    private int maxAccurancy = 50;
    private float accurancy = 100;

    private double tilt;

    private Rect viewBounds;
    private Rect foregroundBounds;
    private Rect backgroundBounds;

    private int backgroundOffsetLeft;
    private int backgroundOffsetTop;
    private int backgroundOffsetRight;
    private int backgroundOffsetBottom;

//    private boolean paused;
//    private boolean showMarker;
//    private boolean showDirection;
//    private boolean showAccurancy;


    private UserLocationBehaviour userLocationBehaviour;

    @MyLocationTracking.Mode
    private int myLocationTrackingMode;

    @MyBearingTracking.Mode
    private int myBearingTrackingMode;

    private MapboxMap mapboxMap;
    private Projection projection;

//    private LatLng markerScreenCoordinate;
    //    private LatLng currentMapViewCoordinate;
//    private PointF markerScreenPoint;

    private Location location;
    private UserLocationListener userLocationListener;

    //    private ObjectAnimator mMarkerDirectionAnimator;
    private ValueAnimator mMarkerCoordinateAnimator;
//    private ObjectAnimator mMarkerAccuracyAnimator;

//    private MapboxMap.OnMyLocationChangeListener onMyLocationChangeListener;

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

        userLocationBehaviour = new UserLocationBehaviourFactory().getBehaviouralModel(MyLocationTracking.TRACKING_NONE);

        viewBounds = new Rect();

//        setBackgroundColor(ContextCompat.getColor(context,android.R.color.darker_gray));

        accurancyPaint = new Paint();
        accurancyPaint.setColor(ContextCompat.getColor(context, R.color.mapbox_blue));
        accurancyPaint.setAlpha(80);

        setShadowDrawable(ContextCompat.getDrawable(context, R.drawable.ic_userlocationview_shadow));
        setForegroundDrawable(ContextCompat.getDrawable(context, R.drawable.ic_userlocationview_bearing));
        setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(v.getContext(), "Click: " + getMeasuredWidth() + " " + getMeasuredHeight(), Toast.LENGTH_SHORT).show();
            }
        });


        mBearingChangeListener = new MyBearingListener(context);
    }

    public final void setForegroundDrawable(Drawable drawable) {
        foregroundDrawable = drawable;
        invalidateBounds();
    }

    public final void setForegroundDrawableTint(@ColorInt int color) {
        if (foregroundDrawable == null) {
            return;
        }
        foregroundDrawable.mutate().setColorFilter(color, PorterDuff.Mode.SRC_IN);
    }

    public final PointF getForegroundCenter() {
        return new PointF(foregroundBounds.centerX(), foregroundBounds.centerY());
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

    public final void setAccurancyTint(@ColorInt int color, @IntRange(from = 0, to = 100) int alpha) {
        accurancyPaint.setColor(color);
        accurancyPaint.setAlpha(alpha);
    }

    private void invalidateBounds() {
        if (backgroundDrawable == null || foregroundDrawable == null) {
            return;
        }

        int backgroundWidth = backgroundDrawable.getIntrinsicWidth();
        int backgroundHeight = backgroundDrawable.getIntrinsicHeight();

        int foregroundWidth = foregroundDrawable.getIntrinsicWidth();
        int foregroundHeight = foregroundDrawable.getIntrinsicHeight();

        int horizontalOffset = backgroundOffsetLeft - backgroundOffsetRight;
        int verticalOffset = backgroundOffsetTop - backgroundOffsetBottom;

        int accurancyWidth = 2 * maxAccurancy;

        backgroundBounds = new Rect(accurancyWidth - (backgroundWidth / 2) + horizontalOffset, accurancyWidth + verticalOffset - (backgroundWidth / 2), accurancyWidth + (backgroundWidth / 2) + horizontalOffset, accurancyWidth + (backgroundHeight / 2) + verticalOffset);
        foregroundBounds = new Rect(accurancyWidth - (foregroundWidth / 2), accurancyWidth - (foregroundHeight / 2), accurancyWidth + (foregroundWidth / 2), accurancyWidth + (foregroundHeight / 2));

        // invoke a new measure
        invalidate();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (foregroundBounds != null) {
            canvas.drawCircle(foregroundBounds.centerX(), foregroundBounds.centerY(), foregroundBounds.width() / 2 + (accurancy / 100 * (getWidth() - foregroundBounds.width()) / 2), accurancyPaint);
        }

        if (backgroundDrawable != null) {
            backgroundDrawable.draw(canvas);
        }

        // todo optimise
        if (foregroundDrawable != null && foregroundBounds != null) {
            getRotateDrawable(foregroundDrawable, compassDirection).draw(canvas);
        }
    }

    private Drawable getRotateDrawable(final Drawable d, final float angle) {
        final Drawable[] arD = {d};
        foregroundRotatedDrawable = new LayerDrawable(arD) {
            @Override
            public void draw(final Canvas canvas) {
                canvas.save();
                canvas.rotate(angle, foregroundBounds.centerX(), foregroundBounds.centerY());
                super.draw(canvas);
                canvas.restore();
            }
        };
        return foregroundRotatedDrawable;
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);

        // position drawables within view
        if (foregroundDrawable != null && foregroundBounds != null) {
            foregroundDrawable.setBounds(foregroundBounds);
            viewBounds.left = foregroundBounds.left;
            viewBounds.top = foregroundBounds.top;
            viewBounds.right = foregroundBounds.right;
            viewBounds.bottom = foregroundBounds.bottom;
        }

        if (backgroundDrawable != null && backgroundBounds != null) {
            backgroundDrawable.setBounds(backgroundBounds);
            viewBounds.union(backgroundBounds);
        }

        setMeasuredDimension(4 * maxAccurancy, 4 * maxAccurancy);
    }

    public void setTilt(@FloatRange(from = 0, to = 60.0f) double tilt) {
        this.tilt = tilt;
        setRotationX((float) tilt);
    }

    public void setDirection(@FloatRange(from = 0, to = 360.0f) float direction) {
        compassDirection = direction % 360.0f;
        updateOnNextFrame();
    }
//
//    public void setMaxAccurancy(int maxAccurancy) {
//        this.maxAccurancy = maxAccurancy;
//        invalidateBounds();
//    }

    public void setAccurancy(float accurancy) {
        this.accurancy = accurancy;
        invalidate();
    }

    void updateOnNextFrame() {
        mapboxMap.invalidate();
    }

    public void onPause() {
//        mBearingChangeListener.onPause();
//        cancelAnimations();
        toggleGps(false);
    }

    public void onResume() {
        if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
//            mBearingChangeListener.onResume();
        }
        if (isEnabled()) {
            toggleGps(true);
        }
    }

    public void update() {
        if (isEnabled()) {
            userLocationBehaviour.invalidate();
//
//            if (myLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
//                // show location on map
//                markerScreenPoint = getMarkerScreenPoint();
//
//            } else if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
//                LatLng currentTarget = mapboxMap.getCameraPosition().target;
//                if (!currentTarget.equals(new LatLng(location))) {
//                    getMarkerScreenPoint();
//                }
//            }


//
//        if (isEnabled() && showMarker) {
//            setVisibility(View.VISIBLE);
//
////            mStaleMarker = isStale(mUserLocation);
//
//            // compute new marker position
//            // TODO add JNI method that takes existing pointf
//            if (myLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
//                // show location on map
//                markerScreenPoint = getMarkerScreenPoint();
//                setX(markerScreenPoint.x - getWidth() / 2);
//                setY(markerScreenPoint.y - getHeight() / 2);
//            } else if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
//                double bearing = 0;
//                if (showDirection) {
//                    bearing = myBearingTrackingMode == MyBearingTracking.COMPASS ? mBearingChangeListener.getCompassBearing() : location.getBearing();
//                } else {
//                    bearing = mapboxMap.getCameraPosition().bearing;
//                }
//
//                if (currentMapViewCoordinate == null) {
//                    currentMapViewCoordinate = mapboxMap.getCameraPosition().target;
//                }
//
//                // only update if there is an actual change
//                if ((!currentMapViewCoordinate.equals(markerScreenCoordinate)) || (!(mCurrentBearing == bearing))) {
//
//                    CameraPosition cameraPosition = new CameraPosition.Builder()
//                            .target(markerScreenCoordinate)
//                            .bearing(bearing)
//                            .build();
//                    mapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition), 300, null);
//
////                    mMarkerScreenMatrix.reset();
//                    markerScreenPoint = getMarkerScreenPoint();
////                    mMarkerScreenMatrix.setTranslate(mMarkerScreenPoint.x, mMarkerScreenPoint.y);
//
//                    // set values for next check for actual change
//                    currentMapViewCoordinate = markerScreenCoordinate;
//                    mCurrentBearing = bearing;
//                }
//            }

//            // rotate so arrow in points to bearing
//            if (showDirection) {
//                if (myBearingTrackingMode == MyBearingTracking.COMPASS && myLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
//                    setDirection(compassDirection);
////                    mMarkerScreenMatrix.preRotate((float)(mCompassMarkerDirection + mMapboxMap.getCameraPosition().bearing));
//                } else if (myBearingTrackingMode == MyBearingTracking.GPS) {
//                    if (myLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
//                        setDirection(gpsDirection);
////                        mMarkerScreenMatrix.preRotate((float)(mGpsMarkerDirection +  mMapboxMap.getCameraPosition().bearing));
//                    } else {
////                        MarkerScreenMatrix.preRotate(gpsDirection);
//                    }
//                }
//            }
//
//            // adjust accuracy circle
//            if (mShowAccuracy && !mStaleMarker) {
//                mAccuracyPath.reset();
//                mAccuracyPath.addCircle(0.0f, 0.0f,
//                        (float) (mMarkerAccuracy / mMapboxMap.getProjection().getMetersPerPixelAtLatitude(
//                                mMarkerCoordinate.getLatitude())),
//                        Path.Direction.CW);
//
//                mAccuracyPath.computeBounds(mAccuracyBounds, false);
//                mAccuracyBounds.inset(-1.0f, -1.0f);
//            }
//
//            // invalidate changed pixels
//            if (mDirtyRect == null) {
//                mDirtyRect = new Rect();
//                mDirtyRectF = new RectF();
//            } else {
//                // the old marker location
//                invalidate(mDirtyRect);
//            }

//            UserLocationViewDrawableHolder holder = getCurrentDrawableHolder();
//
//
//            RectF dotBounds = holder.mBounds;
//            RectF largerBounds = mShowAccuracy && !mStaleMarker && mAccuracyBounds.contains(dotBounds)
//                    ? mAccuracyBounds : dotBounds;
//            mMarkerScreenMatrix.mapRect(mDirtyRectF, largerBounds);
//            mDirtyRectF.roundOut(mDirtyRect);
//            invalidate(mDirtyRect); // the new marker location
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

            // Register for Location Updates
            locationServices.addLocationListener(userLocationListener);
        } else {
            // Disable location and user dot
            location = null;

            // Deregister for Location Updates
            locationServices.removeLocationListener(userLocationListener);
        }

        locationServices.toggleGPS(enableGps);
    }

    public Location getLocation() {
        return location;
    }

    public void setLocation(Location location) {
//        // if null we should hide the marker
//        if (location == null) {
//            showMarker = false;
//            showDirection = false;
//            showAccurancy = false;
//
//            cancelAnimations();
//
//            location = null;
//            return;
//        }
//
//        cancelAnimations();
//        showMarker = true;

        if (location == null) {
            return;
        }
        this.location = location;
        userLocationBehaviour.update(location);

//        LatLng previousCoordinate;
//        if (this.location == null) {
//            previousCoordinate = new LatLng(location);
//        } else {
//            previousCoordinate = new LatLng(this.location);
//        }
//
//        markerScreenCoordinate = new LatLng(location);
//        if (myLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
//            // moving marker above map
//            mMarkerCoordinateAnimator = ValueAnimator.ofFloat(0.0f, 1.0f);
//            mMarkerCoordinateAnimator.setDuration(1000);
//            mMarkerCoordinateAnimator.addUpdateListener(new MarkerCoordinateAnimatorListener(
//                    previousCoordinate, new LatLng(location)
//            ));
//            mMarkerCoordinateAnimator.start();
//        }
//        this.location = location;


//
//        if (myLocationTrackingMode == MyLocationTracking.TRACKING_NONE && myBearingTrackingMode == MyBearingTracking.GPS) {
//            // show GPS direction
//            showDirection = location.hasBearing();
//            if (showDirection) {
//                if (this.location != null && this.location.hasBearing()) {
//                    gpsDirection = this.location.getBearing();
//                }
//                float oldDir = gpsDirection;
//                float newDir = location.getBearing();
//                float diff = oldDir - newDir;
//                if (diff > 180.0f) {
//                    newDir += 360.0f;
//                } else if (diff < -180.0f) {
//                    newDir -= 360.f;
//                }
//                mMarkerDirectionAnimator = ObjectAnimator.ofFloat(this, "direction", oldDir, newDir);
//                mMarkerDirectionAnimator.setDuration(1000);
//                mMarkerDirectionAnimator.start();
//            }
//        } else if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW && myBearingTrackingMode == MyBearingTracking.GPS) {
//            // always show north & rotate map below
//            showDirection = true;
//            gpsDirection = 0;
//        }
//
//        showAccurancy = location.hasAccuracy();
//        if (showAccurancy) {
//            if (this.location != null && this.location.hasAccuracy()) {
//                accurancy = this.location.getAccuracy();
//            }
//            mMarkerAccuracyAnimator = ObjectAnimator.ofFloat(this, "accuracy", location.getAccuracy());
//            mMarkerAccuracyAnimator.setDuration(1000);
//            mMarkerAccuracyAnimator.start();
//        }
//
//        this.location = location;
//        updateOnNextFrame();
//
//        if (onMyLocationChangeListener != null) {
//            onMyLocationChangeListener.onMyLocationChange(location);
//        }
    }

    public void setOnMyLocationChangeListener(@Nullable MapboxMap.OnMyLocationChangeListener listener) {
//        mOnMyLocationChangeListener = listener;
    }


    public void setMyBearingTrackingMode(@MyBearingTracking.Mode int myBearingTrackingMode) {
        this.myBearingTrackingMode = myBearingTrackingMode;

        if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
//            showAccurancy = false;
//            showDirection = true;
            mBearingChangeListener.onResume();
        } else {
            mBearingChangeListener.onPause();
            if (myBearingTrackingMode == MyBearingTracking.GPS) {
//                showDirection = (location != null) && location.hasBearing();
            } else {
//                showDirection = false;
            }
        }
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
    }

    private void setCompass(float bearing) {
        if (myLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
//            // animate marker
////            showDirection = true;
//            float oldDir = compassDirection;
//            float newDir = bearing;
//            float diff = oldDir - newDir;
//            if (diff > 180.0f) {
//                newDir += 360.0f;
//            } else if (diff < -180.0f) {
//                newDir -= 360.f;
//            }
//            mMarkerDirectionAnimator = ObjectAnimator.ofFloat(this, "direction", oldDir, newDir);
//            mMarkerDirectionAnimator.setDuration(1000);
//            mMarkerDirectionAnimator.start();
//            compassDirection = bearing;

        } else if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
//            cancelAnimations();
//            if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
//                // always show north & change map direction
//                showDirection = true;
//                gpsDirection = 0;
//                compassDirection = 0;
//                update();
//            }
        } else {
//            showDirection = false;
        }
    }

    public PointF getMarkerScreenPoint() {
        return userLocationBehaviour.getScreenLocation();
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
            mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_GAME);
            mSensorManager.registerListener(this, mMagnetometer, SensorManager.SENSOR_DELAY_GAME);
        }

        public void onPause() {
            mSensorManager.unregisterListener(this, mAccelerometer);
            mSensorManager.unregisterListener(this, mMagnetometer);
        }

        public float getCompassBearing() {
            return mCurrentDegree;
        }

        @Override
        public void onSensorChanged(SensorEvent event) {
//            if (paused) {
//                return;
//            }

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

        private MarkerCoordinateAnimatorListener(UserLocationBehaviour behaviour, LatLng from, LatLng to) {
            this.behaviour = behaviour;
            mFromLat = from.getLatitude();
            mFromLng = from.getLongitude();
            mToLat = to.getLatitude();
            mToLng = to.getLongitude();
        }

        @Override
        public void onAnimationUpdate(ValueAnimator animation) {
            float frac = animation.getAnimatedFraction();
            double latitude = mFromLat + (mToLat - mFromLat) * frac;
            double longitude = mFromLng + (mToLng - mFromLng) * frac;
            behaviour.update(latitude, longitude);
            updateOnNextFrame();
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

        abstract void update(Location location);

        public void update(double lat, double lon) {
            latLng.setLatitude(lat);
            latLng.setLongitude(lon);
        }

        abstract void invalidate();

        public PointF getScreenLocation() {
            invalidate();
            return screenLocation;
        }
    }

    private class UserLocationTrackingBehaviour extends UserLocationBehaviour {

        @Override
        void update(Location location) {
            if (latLng == null || !latLng.equals(new LatLng(location))) {
                latLng = new LatLng(location);
                CameraPosition cameraPosition = new CameraPosition.Builder()
                        .target(latLng)
                        .build();
                mapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(cameraPosition), 300, null);
            }
        }

        @Override
        void invalidate() {
            int[] contentPadding = mapboxMap.getPadding();
            UiSettings uiSettings = mapboxMap.getUiSettings();
            screenLocation = new PointF(((uiSettings.getWidth() - getWidth() + contentPadding[0] - contentPadding[2]) / 2)
                    , ((uiSettings.getHeight() - getHeight() - contentPadding[3] + contentPadding[1]) / 2));
            if(screenLocation!=null) {
                setX(screenLocation.x);
                setY(screenLocation.y);
            }
        }
    }

    private class UserLocationShowBehaviour extends UserLocationBehaviour {

        @Override
        void update(Location location) {
            LatLng previousCoordinate;
            if (latLng == null) {
                previousCoordinate = new LatLng(location);
            } else {
                previousCoordinate = latLng;
            }

            latLng = new LatLng(location);
            mMarkerCoordinateAnimator = ValueAnimator.ofFloat(0.0f, 1.0f);
            mMarkerCoordinateAnimator.setDuration(1000);
            mMarkerCoordinateAnimator.addUpdateListener(new MarkerCoordinateAnimatorListener(this, previousCoordinate, latLng));
            mMarkerCoordinateAnimator.start();
        }

        @Override
        void invalidate() {
            screenLocation = projection.toScreenLocation(latLng);
            if (screenLocation != null) {
                setX(screenLocation.x - getWidth() / 2);
                setY(screenLocation.y - getHeight() / 2);
            }
        }
    }
}
