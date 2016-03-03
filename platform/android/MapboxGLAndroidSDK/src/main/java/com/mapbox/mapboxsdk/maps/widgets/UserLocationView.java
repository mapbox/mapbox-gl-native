package com.mapbox.mapboxsdk.maps.widgets;

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
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

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

import java.lang.ref.WeakReference;

/**
 * UI element overlaid on a map to show the user's location.
 */
public final class UserLocationView extends View {

    private MapboxMap mMapboxMap;
    private Projection mProjection;

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

    private UserLocationViewDrawableHolder mHolder = new UserLocationViewDrawableHolder();
    private UserLocationViewDrawableHolder mBearingHolder = new UserLocationViewDrawableHolder();
    private UserLocationViewDrawableHolder mStaleHolder = new UserLocationViewDrawableHolder();

    private int mShadowX = 0;
    private int mShadowY = 0;

    private Rect mDirtyRect;
    private RectF mDirtyRectF;

    private LatLng mMarkerCoordinate;
    private ValueAnimator mMarkerCoordinateAnimator;
    private float mGpsMarkerDirection;
    private float mCompassMarkerDirection;
    private ObjectAnimator mMarkerDirectionAnimator;
    private float mMarkerAccuracy;
    private ObjectAnimator mMarkerAccuracyAnimator;

    private LatLng mCurrentMapViewCoordinate;
    private double mCurrentBearing;
    private boolean mPaused = false;
    private Location mUserLocation;
    private UserLocationListener mUserLocationListener;

    private MapboxMap.OnMyLocationChangeListener mOnMyLocationChangeListener;

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
        int accuracyColor = ContextCompat.getColor(context,R.color.my_location_ring);

        float density = resources.getDisplayMetrics().density;
        mMarkerCoordinate = new LatLng(0.0, 0.0);
        mMarkerScreenPoint = new PointF();
        mMarkerScreenMatrix = new Matrix();

        mAccuracyPaintFill = new Paint(Paint.ANTI_ALIAS_FLAG);
        mAccuracyPaintFill.setStyle(Paint.Style.FILL);

        mAccuracyPaintStroke = new Paint(Paint.ANTI_ALIAS_FLAG);
        mAccuracyPaintStroke.setStyle(Paint.Style.STROKE);
        mAccuracyPaintStroke.setStrokeWidth(0.5f * density);
        setUserLocationAccuracyColor(accuracyColor);

        mAccuracyPath = new Path();
        mAccuracyBounds = new RectF();

        setUserLocationDrawable(ContextCompat.getDrawable(getContext(), R.drawable.my_location));
        setUserLocationBearingDrawable(ContextCompat.getDrawable(getContext(), R.drawable.my_location_bearing));
        setUserLocationStaleDrawable(ContextCompat.getDrawable(getContext(), R.drawable.my_location_stale));
    }

    public void setMapboxMap(MapboxMap mapboxMap) {
        mMapboxMap = mapboxMap;
        mProjection = mapboxMap.getProjection();
    }

    @Override
    public void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (!mShowMarker) {
            return;
        }
        UserLocationViewDrawableHolder holder = getCurrentDrawableHolder();
        drawShadow(canvas, holder);
        draw(canvas, holder);
    }

    private UserLocationViewDrawableHolder getCurrentDrawableHolder() {
        UserLocationViewDrawableHolder holder = mShowDirection ? mBearingHolder : mHolder;
        return mStaleMarker ? mStaleHolder : holder;
    }

    private void drawShadow(Canvas canvas, UserLocationViewDrawableHolder holder) {
        if (!holder.hasShadow()) {
            return;
        }
        canvas.save();
        canvas.translate(mShadowX, mShadowY);
        canvas.concat(mMarkerScreenMatrix);
        boolean willDraw = !canvas.quickReject(holder.mShadowBounds, Canvas.EdgeType.AA);

        if (willDraw) {
            holder.mShadow.draw(canvas);
        }
        canvas.restore();
    }

    private void draw(Canvas canvas, UserLocationViewDrawableHolder holder) {
        canvas.save();
        canvas.concat(mMarkerScreenMatrix);
        boolean willDraw = true;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN || !canvas.isHardwareAccelerated()) {
            willDraw = mShowAccuracy && !mStaleMarker && !canvas.quickReject(mAccuracyPath, Canvas.EdgeType.AA);
        }
        willDraw |= !canvas.quickReject(holder.mBounds, Canvas.EdgeType.AA);

        if (willDraw) {
            if (mShowAccuracy && !mStaleMarker) {
                canvas.drawPath(mAccuracyPath, mAccuracyPaintFill);
                canvas.drawPath(mAccuracyPath, mAccuracyPaintStroke);
            }
            holder.mDrawable.draw(canvas);
        }
        canvas.restore();
    }


    public void setUserLocationDrawable(@NonNull Drawable drawable) {
        setUserLocationDrawable(drawable, null);
    }

    public void setUserLocationDrawable(@NonNull Drawable drawable, @Nullable Drawable shadowDrawable) {
        mHolder.setDrawable(drawable);
        mHolder.setShadow(shadowDrawable);
        update();
    }

    public void setUserLocationBearingDrawable(@NonNull Drawable drawable) {
        setUserLocationBearingDrawable(drawable, null);
    }

    public void setUserLocationBearingDrawable(@NonNull Drawable drawable, @Nullable Drawable shadowDrawable) {
        mBearingHolder.setDrawable(drawable);
        mBearingHolder.setShadow(shadowDrawable);
        update();
    }

    public void setUserLocationStaleDrawable(@NonNull Drawable drawable) {
        setUserLocationStaleDrawable(drawable, null);
    }

    public void setUserLocationStaleDrawable(@NonNull Drawable drawable, @Nullable Drawable shadowDrawable) {
        mStaleHolder.setDrawable(drawable);
        mStaleHolder.setShadow(shadowDrawable);
        update();
    }

    public void setUserLocationShadowOffset(int x, int y) {
        mShadowX = x;
        mShadowY = y;
        update();
    }

    public void setUserLocationAccuracyColor(int color) {
        mAccuracyPaintFill.setColor(color);
        mAccuracyPaintFill.setAlpha((int) (255 * 0.25f));
        mAccuracyPaintStroke.setColor(color);
        mAccuracyPaintStroke.setAlpha((int) (255 * 0.5f));
        update();
    }

    public void setMyLocationTrackingMode(@MyLocationTracking.Mode int myLocationTrackingMode) {
        mMyLocationTrackingMode = myLocationTrackingMode;

        if (myLocationTrackingMode != MyLocationTracking.TRACKING_NONE && mUserLocation != null) {
            // center map directly if we have a location fix
            mMarkerCoordinate = new LatLng(mUserLocation.getLatitude(), mUserLocation.getLongitude());
            mMapboxMap.moveCamera(CameraUpdateFactory.newLatLng(new LatLng(mUserLocation)));

            // center view directly
            mMarkerScreenMatrix.reset();
            mMarkerScreenPoint = getMarkerScreenPoint();
            mMarkerScreenMatrix.setTranslate(mMarkerScreenPoint.x, mMarkerScreenPoint.y);
        }
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
                mMarkerScreenPoint = getMarkerScreenPoint();
                mMarkerScreenMatrix.reset();
                mMarkerScreenMatrix.setTranslate(
                        mMarkerScreenPoint.x,
                        mMarkerScreenPoint.y);

            } else if (mMyLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
                double bearing;
                if (mShowDirection) {
                    bearing = mMyBearingTrackingMode == MyBearingTracking.COMPASS ? mBearingChangeListener.getCompassBearing() : mUserLocation.getBearing();
                } else {
                    bearing = mMapboxMap.getCameraPosition().bearing;
                }

                if (mCurrentMapViewCoordinate == null) {
                    mCurrentMapViewCoordinate = mMapboxMap.getCameraPosition().target;
                }

                // only update if there is an actual change
                if ((!mCurrentMapViewCoordinate.equals(mMarkerCoordinate)) || (!(mCurrentBearing == bearing))) {
                    CameraPosition cameraPosition = new CameraPosition.Builder()
                            .target(mMarkerCoordinate)
                            .bearing(bearing)
                            .build();
                    mMapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition), 300, null);
                    mMarkerScreenMatrix.reset();
                    mMarkerScreenPoint = getMarkerScreenPoint();
                    mMarkerScreenMatrix.setTranslate(mMarkerScreenPoint.x, mMarkerScreenPoint.y);

                    // set values for next check for actual change
                    mCurrentMapViewCoordinate = mMarkerCoordinate;
                    mCurrentBearing = bearing;
                }
            }

            // rotate so arrow in points to bearing
            if (mShowDirection) {
                if (mMyBearingTrackingMode == MyBearingTracking.COMPASS && mMyLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
                    mMarkerScreenMatrix.preRotate((float)(mCompassMarkerDirection + mMapboxMap.getCameraPosition().bearing));
                } else if (mMyBearingTrackingMode == MyBearingTracking.GPS) {
                    if (mMyLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
                        mMarkerScreenMatrix.preRotate((float)(mGpsMarkerDirection +  mMapboxMap.getCameraPosition().bearing));
                    } else {
                        mMarkerScreenMatrix.preRotate(mGpsMarkerDirection);
                    }
                }
            }

            // adjust accuracy circle
            if (mShowAccuracy && !mStaleMarker) {
                mAccuracyPath.reset();
                mAccuracyPath.addCircle(0.0f, 0.0f,
                        (float) (mMarkerAccuracy / mMapboxMap.getProjection().getMetersPerPixelAtLatitude(
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

            UserLocationViewDrawableHolder holder = getCurrentDrawableHolder();


            RectF dotBounds = holder.mBounds;
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

            if (mUserLocationListener == null) {
                mUserLocationListener = new UserLocationListener(this);
            }

            // Register for Location Updates
            locationServices.addLocationListener(mUserLocationListener);
        } else {
            // Disable location and user dot
            setLocation(null);

            // Deregister for Location Updates
            locationServices.removeLocationListener(mUserLocationListener);
        }

        locationServices.toggleGPS(enableGps);
    }

    public void setMyBearingTrackingMode(@MyBearingTracking.Mode int myBearingTrackingMode) {
        mMyBearingTrackingMode = myBearingTrackingMode;

        if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
            mShowAccuracy = false;
            mShowDirection = true;
            mBearingChangeListener.onResume();
        } else {
            mBearingChangeListener.onPause();
            if (myBearingTrackingMode == MyBearingTracking.GPS) {
                mShowDirection = (mUserLocation != null) && mUserLocation.hasBearing();
            } else {
                mShowDirection = false;
            }
        }
        update();
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
            if (locationView != null && !locationView.isPaused()) {
                locationView.setLocation(location);
            }
        }
    }

    private boolean isStale(Location location) {
        if (location != null && mMyBearingTrackingMode != MyBearingTracking.COMPASS) {
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
            mMarkerCoordinate = new LatLng(location);
        } else {
            // moving map under the tracker
            mMarkerCoordinate = new LatLng(location);
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
                update();
            }
        }
    }

    void updateOnNextFrame() {
        mMapboxMap.invalidate();
    }

    public void onPause() {
        mPaused = true;
        mBearingChangeListener.onPause();
        cancelAnimations();
        toggleGps(false);
    }

    public void onResume() {
        mPaused = false;
        if (mMyBearingTrackingMode == MyBearingTracking.COMPASS) {
            mBearingChangeListener.onResume();
        }
        if (isEnabled()) {
            toggleGps(true);
        }
    }

    public void setOnMyLocationChangeListener(@Nullable MapboxMap.OnMyLocationChangeListener listener) {
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

    public boolean isPaused() {
        return mPaused;
    }

    public PointF getMarkerScreenPoint() {
        if (mMyLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
            mMarkerScreenPoint = mProjection.toScreenLocation(mMarkerCoordinate);
        } else {
            int[] contentPadding = mMapboxMap.getPadding();
            mMarkerScreenPoint = new PointF(((getMeasuredWidth() + contentPadding[0] - contentPadding[2]) / 2)
                    , ((getMeasuredHeight() - contentPadding[3] + contentPadding[1]) / 2));
        }
        return mMarkerScreenPoint;
    }
}
