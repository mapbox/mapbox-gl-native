package com.mapbox.mapboxsdk.views;

import android.animation.ObjectAnimator;
import android.animation.ValueAnimator;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Location;
import android.os.Build;
import android.os.SystemClock;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.LocationListener;
import com.mapbox.mapboxsdk.location.LocationServices;

final class UserLocationView extends View implements LocationListener {

    private MapView mMapView;

    private float mDensity;

    private boolean mShowMarker;
    private boolean mShowDirection;
    private boolean mShowAccuracy;
    private boolean mStaleMarker;

    private PointF mMarkerScreenPoint;
    private Matrix mMarkerScreenMatrix;

    private Paint mAccuracyPaintFill;
    private Paint mAccuracyPaintStroke;
    private Path mAccuracyPath;
    private RectF mAccuracyBounds;

    private Drawable mUserLocationDrawable;
    private RectF mUserLocationDrawableBoundsF;
    private Rect mUserLocationDrawableBounds;

    private Drawable mUserLocationBearingDrawable;
    private RectF mUserLocationBearingDrawableBoundsF;
    private Rect mUserLocationBearingDrawableBounds;

    private Drawable mUserLocationStaleDrawable;
    private RectF mUserLocationStaleDrawableBoundsF;
    private Rect mUserLocationStaleDrawableBounds;

    private Rect mDirtyRect;
    private RectF mDirtyRectF;

    private LatLng mMarkerCoordinate;
    private ValueAnimator mMarkerCoordinateAnimator;
    private float mGpsMarkerDirection;
    private float mCompassMarkerDirection;
    private ObjectAnimator mMarkerDirectionAnimator;
    private float mMarkerAccuracy;
    private ObjectAnimator mMarkerAccuracyAnimator;

    private boolean mPaused = false;
    private Location mUserLocation;

    MapView.OnMyLocationChangeListener mOnMyLocationChangeListener;

    @MyLocationTracking.Mode
    private int mMyLocationTrackingMode;

    @MyBearingTracking.Mode
    private int mMyBearingTrackingMode;

    // Compass data
    private MyBearingListener mBearingChangeListener;

    public UserLocationView(Context context) {
        super(context);
        initialize(context);
    }

    public UserLocationView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initialize(context);
    }

    public UserLocationView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initialize(context);
    }

    private void initialize(Context context) {
        // View configuration
        setEnabled(false);
        setWillNotDraw(false);

        // Layout params
        ViewGroup.LayoutParams lp = new ViewGroup.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT);
        setLayoutParams(lp);

        // Setup sensors
        mBearingChangeListener = new MyBearingListener(context);

        // Setup the custom paint
        Resources resources = context.getResources();
        int accuracyColor = resources.getColor(R.color.my_location_ring);

        mDensity = resources.getDisplayMetrics().density;
        mMarkerCoordinate = new LatLng(0.0, 0.0);
        mMarkerScreenPoint = new PointF();
        mMarkerScreenMatrix = new Matrix();

        mAccuracyPaintFill = new Paint();
        mAccuracyPaintFill.setAntiAlias(true);
        mAccuracyPaintFill.setStyle(Paint.Style.FILL);
        mAccuracyPaintFill.setColor(accuracyColor);
        mAccuracyPaintFill.setAlpha((int) (255 * 0.25f));

        mAccuracyPaintStroke = new Paint();
        mAccuracyPaintStroke.setAntiAlias(true);
        mAccuracyPaintStroke.setStyle(Paint.Style.STROKE);
        mAccuracyPaintStroke.setStrokeWidth(0.5f * mDensity);
        mAccuracyPaintStroke.setColor(accuracyColor);
        mAccuracyPaintStroke.setAlpha((int) (255 * 0.5f));

        mAccuracyPath = new Path();
        mAccuracyBounds = new RectF();

        mUserLocationDrawable = ContextCompat.getDrawable(getContext(), R.drawable.my_location);
        mUserLocationDrawableBounds = new Rect(
                -mUserLocationDrawable.getIntrinsicWidth() / 2,
                -mUserLocationDrawable.getIntrinsicHeight() / 2,
                mUserLocationDrawable.getIntrinsicWidth() / 2,
                mUserLocationDrawable.getIntrinsicHeight() / 2);
        mUserLocationDrawableBoundsF = new RectF(
                -mUserLocationDrawable.getIntrinsicWidth() / 2,
                -mUserLocationDrawable.getIntrinsicHeight() / 2,
                mUserLocationDrawable.getIntrinsicWidth() / 2,
                mUserLocationDrawable.getIntrinsicHeight() / 2);
        mUserLocationDrawable.setBounds(mUserLocationDrawableBounds);

        mUserLocationBearingDrawable = ContextCompat.getDrawable(getContext(), R.drawable.my_location_bearing);
        mUserLocationBearingDrawableBounds = new Rect(
                -mUserLocationBearingDrawable.getIntrinsicWidth() / 2,
                -mUserLocationBearingDrawable.getIntrinsicHeight() / 2,
                mUserLocationBearingDrawable.getIntrinsicWidth() / 2,
                mUserLocationBearingDrawable.getIntrinsicHeight() / 2);
        mUserLocationBearingDrawableBoundsF = new RectF(
                -mUserLocationBearingDrawable.getIntrinsicWidth() / 2,
                -mUserLocationBearingDrawable.getIntrinsicHeight() / 2,
                mUserLocationBearingDrawable.getIntrinsicWidth() / 2,
                mUserLocationBearingDrawable.getIntrinsicHeight() / 2);
        mUserLocationBearingDrawable.setBounds(mUserLocationBearingDrawableBounds);

        mUserLocationStaleDrawable = ContextCompat.getDrawable(getContext(), R.drawable.my_location_stale);
        mUserLocationStaleDrawableBounds = new Rect(
                -mUserLocationStaleDrawable.getIntrinsicWidth() / 2,
                -mUserLocationStaleDrawable.getIntrinsicHeight() / 2,
                mUserLocationStaleDrawable.getIntrinsicWidth() / 2,
                mUserLocationStaleDrawable.getIntrinsicHeight() / 2);
        mUserLocationStaleDrawableBoundsF = new RectF(
                -mUserLocationStaleDrawable.getIntrinsicWidth() / 2,
                -mUserLocationStaleDrawable.getIntrinsicHeight() / 2,
                mUserLocationStaleDrawable.getIntrinsicWidth() / 2,
                mUserLocationStaleDrawable.getIntrinsicHeight() / 2);
        mUserLocationStaleDrawable.setBounds(mUserLocationStaleDrawableBounds);
    }

    public void setMapView(MapView mapView) {
        mMapView = mapView;
    }

    public void onStart() {
        if (mMyBearingTrackingMode == MyBearingTracking.COMPASS) {
            mBearingChangeListener.onStart(getContext());
        }
    }

    public void onStop() {
        mBearingChangeListener.onStop();
        cancelAnimations();
    }

    @Override
    public void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (!mShowMarker) {
            return;
        }

        canvas.concat(mMarkerScreenMatrix);

        Drawable dotDrawable = mShowDirection ? mUserLocationBearingDrawable : mUserLocationDrawable;
        dotDrawable = mStaleMarker ? mUserLocationStaleDrawable : dotDrawable;
        // IMPORTANT also update in update()
        RectF dotBounds = mShowDirection ? mUserLocationBearingDrawableBoundsF : mUserLocationDrawableBoundsF;
        dotBounds = mStaleMarker ? mUserLocationStaleDrawableBoundsF : dotBounds;

        boolean willDraw = true;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN || !canvas.isHardwareAccelerated()) {
            willDraw = mShowAccuracy && !mStaleMarker && !canvas.quickReject(mAccuracyPath, Canvas.EdgeType.AA);
        }
        willDraw |= !canvas.quickReject(dotBounds, Canvas.EdgeType.AA);

        if (willDraw) {
            if (mShowAccuracy && !mStaleMarker) {
                canvas.drawPath(mAccuracyPath, mAccuracyPaintFill);
                canvas.drawPath(mAccuracyPath, mAccuracyPaintStroke);
            }
            dotDrawable.draw(canvas);
        }
    }

    public void setMyLocationTrackingMode(@MyLocationTracking.Mode int myLocationTrackingMode) {
        mMyLocationTrackingMode = myLocationTrackingMode;

        if (myLocationTrackingMode != MyLocationTracking.TRACKING_NONE && mUserLocation != null) {
            // center map directly if we have a location fix
            mMapView.setCenterCoordinate(new LatLng(mUserLocation));
        }
    }

    @MyLocationTracking.Mode
    public int getMyLocationTrackingMode() {
        return mMyLocationTrackingMode;
    }

    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);
        setVisibility(enabled ? View.VISIBLE : View.INVISIBLE);
        toggleGps(enabled);
    }

    public void update() {
        if (isEnabled() && mShowMarker) {
            setVisibility(View.VISIBLE);

            mStaleMarker = isStale(mUserLocation);

            // compute new marker position
            // TODO add JNI method that takes existing pointf
            if (mMyLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
                mMarkerScreenPoint = mMapView.toScreenLocation(mMarkerCoordinate);
                mMarkerScreenMatrix.reset();
                mMarkerScreenMatrix.setTranslate(
                        mMarkerScreenPoint.x,
                        mMarkerScreenPoint.y);
            } else if (mMyLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
                mMarkerScreenMatrix.setTranslate(getMeasuredWidth() / 2, getMeasuredHeight() / 2);
                mMapView.setCenterCoordinate(mMarkerCoordinate, true);
            }

            // rotate so arrow in points to bearing
            if (mShowDirection) {
                if (mMyBearingTrackingMode == MyBearingTracking.COMPASS && mMyLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
                    mMarkerScreenMatrix.preRotate(mCompassMarkerDirection + (float) mMapView.getDirection());
                } else if (mMyBearingTrackingMode == MyBearingTracking.GPS) {
                    if (mMyLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
                        mMarkerScreenMatrix.preRotate(mGpsMarkerDirection + (float) mMapView.getDirection());
                    } else {
                        mMarkerScreenMatrix.preRotate(mGpsMarkerDirection);
                    }
                }
            }

            // adjust accuracy circle
            if (mShowAccuracy && !mStaleMarker) {
                mAccuracyPath.reset();
                mAccuracyPath.addCircle(0.0f, 0.0f,
                        (float) (mMarkerAccuracy / mMapView.getMetersPerPixelAtLatitude(
                                mMarkerCoordinate.getLatitude())),
                        Path.Direction.CW);

                mAccuracyPath.computeBounds(mAccuracyBounds, false);
                mAccuracyBounds.inset(-1.0f, -1.0f);
            }

            // invalidate changed pixels
            if (mDirtyRect == null) {
                mDirtyRect = new Rect();
                mDirtyRectF = new RectF();
            } else {
                // the old marker location
                invalidate(mDirtyRect);
            }

            RectF dotBounds = mShowDirection ? mUserLocationBearingDrawableBoundsF : mUserLocationDrawableBoundsF;
            dotBounds = mStaleMarker ? mUserLocationStaleDrawableBoundsF : dotBounds;
            RectF largerBounds = mShowAccuracy && !mStaleMarker && mAccuracyBounds.contains(dotBounds)
                    ? mAccuracyBounds : dotBounds;
            mMarkerScreenMatrix.mapRect(mDirtyRectF, largerBounds);
            mDirtyRectF.roundOut(mDirtyRect);
            invalidate(mDirtyRect); // the new marker location
        } else {
            setVisibility(View.INVISIBLE);
        }
    }

    public Location getLocation() {
        return mUserLocation;
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

            // Register for Location Updates
            locationServices.addLocationListener(this);
        } else {
            // Disable location and user dot
            setLocation(null);

            // Deregister for Location Updates
            locationServices.removeLocationListener(this);
        }

        locationServices.toggleGPS(enableGps);
    }

    public void setMyBearingTrackingMode(@MyBearingTracking.Mode int myBearingTrackingMode) {
        mMyBearingTrackingMode = myBearingTrackingMode;

        if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
            mShowAccuracy = false;
            mShowDirection = false;
            mBearingChangeListener.onStart(getContext());
        } else if (myBearingTrackingMode == MyBearingTracking.GPS) {
            mBearingChangeListener.onStop();
            if (mUserLocation != null && mUserLocation.hasBearing()) {
                mShowDirection = true;
            } else {
                mShowDirection = false;
            }
        } else {
            mShowDirection = false;
        }
    }

    @MyBearingTracking.Mode
    public int getMyBearingTrackingMode() {
        return mMyBearingTrackingMode;
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

        public void onStart(Context context) {
            mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_GAME);
            mSensorManager.registerListener(this, mMagnetometer, SensorManager.SENSOR_DELAY_GAME);
        }

        public void onStop() {
            mSensorManager.unregisterListener(this, mAccelerometer);
            mSensorManager.unregisterListener(this, mMagnetometer);
        }

        public float getCompassBearing() {
            return mCompassBearing;
        }

        @Override
        public void onSensorChanged(SensorEvent event) {
            if (mPaused) {
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
                float azimuthInDegress = (float) (Math.toDegrees(azimuthInRadians) + 360) % 360;
                mCompassBearing = mCurrentDegree;
                mCurrentDegree = -azimuthInDegress;
            }
            mCompassUpdateNextTimestamp = currentTime + UPDATE_RATE_MS;
            setCompass(mCompassBearing);
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {
            // TODO add accuracy to the equiation
        }
    }

    /**
     * Callback method for receiving location updates from LocationServices.
     *
     * @param location The new Location data
     */
    @Override
    public void onLocationChanged(Location location) {
        if (mPaused) {
            return;
        }
        setLocation(location);
    }

    private boolean isStale(Location location) {
        if (location != null) {
            long ageInNanos;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
                ageInNanos = SystemClock.elapsedRealtimeNanos() -
                        location.getElapsedRealtimeNanos();
            } else {
                ageInNanos = (System.currentTimeMillis() - location.getTime()) * 1000 * 1000;
            }
            final long oneMinuteInNanos = 60L * 1000 * 1000 * 1000;
            return ageInNanos > oneMinuteInNanos;
        } else {
            return false;
        }
    }

    // Handles location updates from GPS
    private void setLocation(Location location) {
        // if null we should hide the marker
        if (location == null) {
            mShowMarker = false;
            mShowDirection = false;
            mShowAccuracy = false;

            cancelAnimations();

            mUserLocation = null;
            return;
        }

        if (mMarkerCoordinateAnimator != null) {
            mMarkerCoordinateAnimator.end();
            mMarkerCoordinateAnimator = null;
        }

        if (mMarkerDirectionAnimator != null) {
            mMarkerDirectionAnimator.end();
            mMarkerDirectionAnimator = null;
        }

        if (mMarkerAccuracyAnimator != null) {
            mMarkerAccuracyAnimator.end();
            mMarkerAccuracyAnimator = null;
        }

        mShowMarker = true;

        LatLng previousCoordinate;
        if (mUserLocation == null) {
            previousCoordinate = new LatLng(location);
        } else {
            previousCoordinate = new LatLng(mUserLocation);
        }

        if (mMyLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
            // moving marker above map
            mMarkerCoordinateAnimator = ValueAnimator.ofFloat(0.0f, 1.0f);
            mMarkerCoordinateAnimator.setDuration(1000);
            mMarkerCoordinateAnimator.addUpdateListener(new MarkerCoordinateAnimatorListener(
                    previousCoordinate, new LatLng(location)
            ));
            mMarkerCoordinateAnimator.start();
        } else {
            // moving map under the tracker
            mMarkerCoordinate = new LatLng(location);
            mMapView.setCenterCoordinate(mMarkerCoordinate, true);
        }

        if (mMyLocationTrackingMode == MyLocationTracking.TRACKING_NONE && mMyBearingTrackingMode == MyBearingTracking.GPS) {
            // show GPS direction
            mShowDirection = location.hasBearing();
            if (mShowDirection) {
                if (mUserLocation != null && mUserLocation.hasBearing()) {
                    mGpsMarkerDirection = mUserLocation.getBearing();
                }
                float oldDir = mGpsMarkerDirection;
                float newDir = location.getBearing();
                float diff = oldDir - newDir;
                if (diff > 180.0f) {
                    newDir += 360.0f;
                } else if (diff < -180.0f) {
                    newDir -= 360.f;
                }
                mMarkerDirectionAnimator = ObjectAnimator.ofFloat(this, "direction", oldDir, newDir);
                mMarkerDirectionAnimator.setDuration(1000);
                mMarkerDirectionAnimator.start();
            }
        } else if (mMyLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW && mMyBearingTrackingMode == MyBearingTracking.GPS) {
            // always show north & rotate map below
            mShowDirection = true;
            mGpsMarkerDirection = 0;
            if (location.hasBearing()) {
                mMapView.setBearing(location.getBearing());
            }
        }

        mShowAccuracy = location.hasAccuracy();
        if (mShowAccuracy) {
            if (mUserLocation != null && mUserLocation.hasAccuracy()) {
                mMarkerAccuracy = mUserLocation.getAccuracy();
            }
            mMarkerAccuracyAnimator = ObjectAnimator.ofFloat(this, "accuracy", location.getAccuracy());
            mMarkerAccuracyAnimator.setDuration(1000);
            mMarkerAccuracyAnimator.start();
        }

        mUserLocation = location;
        updateOnNextFrame();

        if (mOnMyLocationChangeListener != null) {
            mOnMyLocationChangeListener.onMyLocationChange(location);
        }
    }

    // handles compass sensor updates
    private void setCompass(float bearing) {
        if (mMyLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
            // animate marker
            mShowDirection = true;
            float oldDir = mCompassMarkerDirection;
            float newDir = bearing;
            float diff = oldDir - newDir;
            if (diff > 180.0f) {
                newDir += 360.0f;
            } else if (diff < -180.0f) {
                newDir -= 360.f;
            }
            mMarkerDirectionAnimator = ObjectAnimator.ofFloat(this, "direction", oldDir, newDir);
            mMarkerDirectionAnimator.setDuration(1000);
            mMarkerDirectionAnimator.start();
            mCompassMarkerDirection = bearing;

        } else if (mMyLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
            cancelAnimations();
            if (mMyBearingTrackingMode == MyBearingTracking.COMPASS) {
                // always show north & change map direction
                mShowDirection = true;
                mGpsMarkerDirection = 0;
                mCompassMarkerDirection = 0;
                mMapView.setBearing(bearing);
            }
        }
    }

    void updateOnNextFrame() {
        mMapView.update();
    }

    /**
     * Called from MapView.onPause()
     */
    public void pause() {
        mPaused = true;
        toggleGps(false);
    }

    /**
     * Called from MapView.onResume()
     */
    public void resume() {
        mPaused = false;
        if (isEnabled()) {
            toggleGps(true);
        }
    }

    public void setOnMyLocationChangeListener(@Nullable MapView.OnMyLocationChangeListener listener) {
        mOnMyLocationChangeListener = listener;
    }

    // public for animator only
    public float getDirection() {
        if (mMyBearingTrackingMode == MyBearingTracking.COMPASS) {
            return mCompassMarkerDirection;
        }
        return mGpsMarkerDirection;
    }

    // public for animator only
    public void setDirection(float direction) {
        if (mMyBearingTrackingMode == MyBearingTracking.COMPASS) {
            mCompassMarkerDirection = direction % 360.0f;
        } else {
            mGpsMarkerDirection = direction % 360.0f;
        }
        updateOnNextFrame();
    }

    // public for animator only
    public float getAccuracy() {
        return mMarkerAccuracy;
    }

    // public for animator only
    public void setAccuracy(float accuracy) {
        mMarkerAccuracy = accuracy;
        updateOnNextFrame();
    }

    private class MarkerCoordinateAnimatorListener implements ValueAnimator.AnimatorUpdateListener {

        private double mFromLat;
        private double mFromLng;
        private double mToLat;
        private double mToLng;

        private MarkerCoordinateAnimatorListener(LatLng from, LatLng to) {
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
            mMarkerCoordinate.setLatitude(latitude);
            mMarkerCoordinate.setLongitude(longitude);
            updateOnNextFrame();
        }
    }

    public void cancelAnimations() {
        if (mMarkerCoordinateAnimator != null) {
            mMarkerCoordinateAnimator.cancel();
            mMarkerCoordinateAnimator = null;
        }

        if (mMarkerDirectionAnimator != null) {
            mMarkerDirectionAnimator.cancel();
            mMarkerDirectionAnimator = null;
        }

        if (mMarkerAccuracyAnimator != null) {
            mMarkerAccuracyAnimator.cancel();
            mMarkerAccuracyAnimator = null;
        }
    }

}
