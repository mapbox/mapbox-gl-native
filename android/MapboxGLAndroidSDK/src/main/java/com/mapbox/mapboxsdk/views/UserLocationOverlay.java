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
import android.hardware.GeomagneticField;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Location;
import android.os.Build;
import android.os.SystemClock;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.WindowManager;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.BoundingBox;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapzen.android.lost.api.LocationListener;
import com.mapzen.android.lost.api.LocationRequest;
import com.mapzen.android.lost.api.LocationServices;
import com.mapzen.android.lost.api.LostApiClient;

final class UserLocationOverlay implements Overlay {

    //region CLASS VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //endregion

    //region CLASS METHODS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //endregion

    //region FIELDS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    private final Context mContext;
    private MapView mMapView;

    private boolean mShowMarker;
    private boolean mShowDirection;
    private boolean mShowAccuracy;

    private PointF mMarkerScreenPoint;
    private Matrix mMarkerScreenMatrix;

    private Paint mAccuracyPaintFill;
    private Paint mAccuracyPaintStroke;
    private Path mAccuracyPath;
    private RectF mAccuracyBounds;

    private Drawable mUserLocationDrawable;
    private RectF mUserLocationDrawableBoundsF;

    private Drawable mUserLocationBearingDrawable;
    private RectF mUserLocationBearingDrawableBoundsF;

    private Drawable mUserLocationStaleDrawable;
    private RectF mUserLocationStaleDrawableBoundsF;

    private LatLng mMarkerCoordinate;
    private ValueAnimator mMarkerCoordinateAnimator;
    private float mGpsMarkerDirection;
    private float mCompassMarkerDirection;
    private ObjectAnimator mMarkerDirectionAnimator;
    private float mMarkerAccuracy;
    private ObjectAnimator mMarkerAccuracyAnimator;

    private boolean mPaused = false;
    private LostApiClient mLocationClient;
    private LocationRequest mLocationRequest;
    private Location mUserLocation;
    private MyLocationListener mLocationListener;

    MapView.OnMyLocationChangeListener mOnMyLocationChangeListener;

    @MyLocationTracking.Mode
    private int mMyLocationTrackingMode;

    @MyBearingTracking.Mode
    private int mMyBearingTrackingMode;

    // Compass data
    private MyBearingListener mBearingChangeListener;
    private boolean mEnabled;

    //endregion

    //region INJECTED DEPENDENCIES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //endregion

    //region INJECTED VIEWS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //endregion

    //region CONSTRUCTOR ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    public UserLocationOverlay(Context context) {
        mContext = context;

        // View configuration
        setEnabled(false);

        // Setup location services
        mLocationClient = new LostApiClient.Builder(context).build();
        mLocationRequest = LocationRequest.create()
                .setFastestInterval(1000)
                .setSmallestDisplacement(3.0f)
                .setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY);

        // Setup sensors
        mBearingChangeListener = new MyBearingListener(context);

        // Setup the custom paint
        Resources resources = context.getResources();
        int accuracyColor = resources.getColor(R.color.my_location_ring);

        float density = resources.getDisplayMetrics().density;
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
        mAccuracyPaintStroke.setStrokeWidth(0.5f * density);
        mAccuracyPaintStroke.setColor(accuracyColor);
        mAccuracyPaintStroke.setAlpha((int) (255 * 0.5f));

        mAccuracyPath = new Path();
        mAccuracyBounds = new RectF();

        mUserLocationDrawable = ContextCompat.getDrawable(context, R.drawable.my_location);
        Rect userLocationDrawableBounds = new Rect(
                -mUserLocationDrawable.getIntrinsicWidth() / 2,
                -mUserLocationDrawable.getIntrinsicHeight() / 2,
                mUserLocationDrawable.getIntrinsicWidth() / 2,
                mUserLocationDrawable.getIntrinsicHeight() / 2);
        mUserLocationDrawableBoundsF = new RectF(
                -mUserLocationDrawable.getIntrinsicWidth() / 2,
                -mUserLocationDrawable.getIntrinsicHeight() / 2,
                mUserLocationDrawable.getIntrinsicWidth() / 2,
                mUserLocationDrawable.getIntrinsicHeight() / 2);
        mUserLocationDrawable.setBounds(userLocationDrawableBounds);

        mUserLocationBearingDrawable = ContextCompat.getDrawable(context, R.drawable.my_location_bearing);
        Rect userLocationBearingDrawableBounds = new Rect(
                -mUserLocationBearingDrawable.getIntrinsicWidth() / 2,
                -mUserLocationBearingDrawable.getIntrinsicHeight() / 2,
                mUserLocationBearingDrawable.getIntrinsicWidth() / 2,
                mUserLocationBearingDrawable.getIntrinsicHeight() / 2);
        mUserLocationBearingDrawableBoundsF = new RectF(
                -mUserLocationBearingDrawable.getIntrinsicWidth() / 2,
                -mUserLocationBearingDrawable.getIntrinsicHeight() / 2,
                mUserLocationBearingDrawable.getIntrinsicWidth() / 2,
                mUserLocationBearingDrawable.getIntrinsicHeight() / 2);
        mUserLocationBearingDrawable.setBounds(userLocationBearingDrawableBounds);

        mUserLocationStaleDrawable = ContextCompat.getDrawable(context, R.drawable.my_location_stale);
        Rect userLocationStaleDrawableBounds = new Rect(
                -mUserLocationStaleDrawable.getIntrinsicWidth() / 2,
                -mUserLocationStaleDrawable.getIntrinsicHeight() / 2,
                mUserLocationStaleDrawable.getIntrinsicWidth() / 2,
                mUserLocationStaleDrawable.getIntrinsicHeight() / 2);
        mUserLocationStaleDrawableBoundsF = new RectF(
                -mUserLocationStaleDrawable.getIntrinsicWidth() / 2,
                -mUserLocationStaleDrawable.getIntrinsicHeight() / 2,
                mUserLocationStaleDrawable.getIntrinsicWidth() / 2,
                mUserLocationStaleDrawable.getIntrinsicHeight() / 2);
        mUserLocationStaleDrawable.setBounds(userLocationStaleDrawableBounds);
    }


    //endregion

    //region LIFE CYCLE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //endregion

    //region ACCESSORS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //endregion

    //region PRIVATE METHODS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    /**
     * Enabled / Disable GPS location updates along with updating the UI
     *
     * @param enableGps true if GPS is to be enabled, false if GPS is to be disabled
     */
    private void toggleGps(boolean enableGps) {
        if (mLocationClient == null) {
            return;
        }

        if (enableGps) {
            if (!mLocationClient.isConnected()) {
                mUserLocation = null;
                mLocationClient.connect();
                Location lastLocation = LocationServices.FusedLocationApi.getLastLocation();
                if (lastLocation != null) {
                    setLocation(lastLocation);
                }
                mLocationListener = new MyLocationListener();
                LocationServices.FusedLocationApi.requestLocationUpdates(mLocationRequest, mLocationListener);
            }
        } else {
            if (mLocationClient.isConnected()) {
                LocationServices.FusedLocationApi.removeLocationUpdates(mLocationListener);
                mLocationListener = null;
                mLocationClient.disconnect();
                mUserLocation = null;
            }
        }
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
                mMapView.setBearing(mUserLocation.getBearing());
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
            if (mMyBearingTrackingMode == MyBearingTracking.COMPASS) {
                // always show north & change map direction
                mShowDirection = true;
                mCompassMarkerDirection = 0;
                mMapView.setBearing(bearing);
            }
        }
    }

    void updateOnNextFrame() {
        if (mMapView != null) {
            mMapView.update();
        }
    }

    //endregion

    //region PUBLIC METHODS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    public void pause() {
        mPaused = true;
        toggleGps(false);
    }

    public void resume() {
        mPaused = false;
        if (mEnabled) {
            toggleGps(true);
        }
    }

    public void setOnMyLocationChangeListener(@Nullable MapView.OnMyLocationChangeListener listener) {
        mOnMyLocationChangeListener = listener;
    }

    public void onStart() {
        if (mMyBearingTrackingMode == MyBearingTracking.COMPASS) {
            mBearingChangeListener.onStart(mContext);
        }
    }

    public void onStop() {
        mBearingChangeListener.onStop();
        cancelAnimations();
    }

    public void setMyLocationTrackingMode(@MyLocationTracking.Mode int myLocationTrackingMode) {
        mMyLocationTrackingMode = myLocationTrackingMode;

        if (myLocationTrackingMode != MyLocationTracking.TRACKING_NONE && mUserLocation != null) {
            // center map directly if we have a location fix
            mMapView.setCenterCoordinate(new LatLng(mUserLocation.getLatitude(), mUserLocation.getLongitude()));
        }
    }

    @MyLocationTracking.Mode
    public int getMyLocationTrackingMode() {
        return mMyLocationTrackingMode;
    }

    public void setEnabled(boolean enabled) {
        mEnabled = enabled;
        toggleGps(enabled);
    }

    public Location getLocation() {
        return mUserLocation;
    }

    public void setMyBearingTrackingMode(@MyBearingTracking.Mode int myBearingTrackingMode) {
        mMyBearingTrackingMode = myBearingTrackingMode;

        if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
            mShowDirection = false;
            mBearingChangeListener.onStart(mContext);
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

    //endregion

    //region Overlay ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    @Override
    public void onOverlayDraw(MapView mapView, Canvas canvas, BoundingBox wgsBounds, LatLng wgsCenter, float bearing, float zoom) {

        if (!mShowMarker) {
            return;
        }

        boolean staleMarker = isStale(mUserLocation);

        // compute new marker position
        if (mMyLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
            mMapView.toScreenLocation(mMarkerCoordinate, mMarkerScreenPoint);
            mMarkerScreenMatrix.reset();
            mMarkerScreenMatrix.setTranslate(
                    mMarkerScreenPoint.x,
                    mMarkerScreenPoint.y);
        } else if (mMyLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
            mMapView.setCenterCoordinate(mMarkerCoordinate, true);
        }

        // rotate so arrow in points to bearing
        if (mShowDirection) {
            if (mMyBearingTrackingMode == MyBearingTracking.COMPASS) {
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
        if (mShowAccuracy && !staleMarker) {
            mAccuracyPath.reset();
            mAccuracyPath.addCircle(0.0f, 0.0f,
                    (float) (mMarkerAccuracy / mMapView.getMetersPerPixelAtLatitude(
                            mMarkerCoordinate.getLatitude())),
                    Path.Direction.CW);

            mAccuracyPath.computeBounds(mAccuracyBounds, false);
            mAccuracyBounds.inset(-1.0f, -1.0f);
        }

        canvas.save();

        canvas.concat(mMarkerScreenMatrix);

        Drawable dotDrawable = mShowDirection ? mUserLocationBearingDrawable : mUserLocationDrawable;
        dotDrawable = staleMarker ? mUserLocationStaleDrawable : dotDrawable;
        RectF dotBounds = mShowDirection ? mUserLocationBearingDrawableBoundsF : mUserLocationDrawableBoundsF;
        dotBounds = staleMarker ? mUserLocationStaleDrawableBoundsF : dotBounds;

        boolean willDraw = true;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN || !canvas.isHardwareAccelerated()) {
            willDraw = mShowAccuracy && !staleMarker && !canvas.quickReject(mAccuracyPath, Canvas.EdgeType.AA);
        }
        willDraw |= !canvas.quickReject(dotBounds, Canvas.EdgeType.AA);

        if (willDraw) {
            if (mShowAccuracy && !staleMarker) {
                canvas.drawPath(mAccuracyPath, mAccuracyPaintFill);
                canvas.drawPath(mAccuracyPath, mAccuracyPaintStroke);
            }
            dotDrawable.draw(canvas);
        }

        canvas.restore();
    }

    @Override
    public void onOverlayTouchEvent(MotionEvent event) {

    }

    @Override
    public void onOverlayAttached(MapView mapView) {
        mMapView = mapView;
    }

    @Override
    public void onOverlayDetached() {

    }

    @Override
    public void onOverlaySingleTapConfirmed(LatLng pressPosition) {

    }

    @Override
    public void onOverlayLongPress(LatLng pressPosition) {

    }

    @Override
    public boolean isOverlayDrawEnabled() {
        return mEnabled;
    }

    @Override
    public void onMapViewPixelBoundsChanged(Rect mapPixelBounds) {

    }


    //endregion

    //region LISTENERS  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    private class MyBearingListener implements SensorEventListener {

        // Sensor model
        private SensorManager mSensorManager;
        private Sensor mSensorRotationVector;
        private int mRotationDevice;

        // Sensor data sensor rotation vector
        private float[] mRotationMatrix = new float[16];
        private float[] mRemappedMatrix = new float[16];
        private float[] mOrientation = new float[3];

        // Location data
        private GeomagneticField mGeomagneticField;

        // Controls the sensor update rate in milliseconds
        private static final int UPDATE_RATE_MS = 300;

        // Compass data
        private float mCompassBearing;
        private long mCompassUpdateNextTimestamp = 0;

        public MyBearingListener(Context context) {
            mSensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
            mSensorRotationVector = mSensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);
        }

        public void onStart(Context context) {
            mRotationDevice = ((WindowManager) context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getRotation();
            mSensorManager.registerListener(this, mSensorRotationVector, UPDATE_RATE_MS * 2500);
        }

        public void onStop() {
            mSensorManager.unregisterListener(this, mSensorRotationVector);
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

            switch (event.sensor.getType()) {
                case Sensor.TYPE_ROTATION_VECTOR:
                    SensorManager.getRotationMatrixFromVector(mRotationMatrix, event.values);
                    break;
            }

            switch (mRotationDevice) {
                case Surface.ROTATION_0:
                    // Portrait
                    SensorManager.getOrientation(mRotationMatrix, mOrientation);
                    break;
                default:
                    // Landscape
                    SensorManager.remapCoordinateSystem(mRotationMatrix, SensorManager.AXIS_MINUS_Y, SensorManager.AXIS_MINUS_X, mRemappedMatrix);
                    SensorManager.getOrientation(mRemappedMatrix, mOrientation);
                    break;
            }

            mCompassUpdateNextTimestamp = currentTime + UPDATE_RATE_MS;
            mGeomagneticField = new GeomagneticField(
                    (float) mUserLocation.getLatitude(),
                    (float) mUserLocation.getLongitude(),
                    (float) mUserLocation.getAltitude(),
                    currentTime);
            mCompassBearing = (float) Math.toDegrees(mOrientation[0] + mGeomagneticField.getDeclination());
            setCompass(mCompassBearing);
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {
            // TODO add accuracy to the equiation
        }
    }


    private class MyLocationListener implements LocationListener {
        @Override
        public void onLocationChanged(Location location) {
            if (mPaused) {
                return;
            }
            setLocation(location);
        }
    }

    //endregion

    //region EVENTS  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //endregion

    //region INNER CLASSES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

    //endregion

}
