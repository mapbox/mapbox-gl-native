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
import android.util.AttributeSet;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapzen.android.lost.api.LocationListener;
import com.mapzen.android.lost.api.LocationRequest;
import com.mapzen.android.lost.api.LocationServices;
import com.mapzen.android.lost.api.LostApiClient;

import java.util.ArrayDeque;

final class UserLocationView extends View {

    private MapView mMapView;

    private static final int BLUE_COLOR = 0x39ACCBFF;

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
    private float mMarkerDirection;
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

        // Setup location services
        mLocationClient = new LostApiClient.Builder(getContext()).build();
        mLocationRequest = LocationRequest.create()
                .setFastestInterval(1000)
                .setSmallestDisplacement(3.0f)
                .setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY);

        // Setup sensors
        mBearingChangeListener = new MyBearingListener(context);

        // Setup the custom paint
        Resources resources = context.getResources();
        mDensity = resources.getDisplayMetrics().density;
        mMarkerCoordinate = new LatLng(0.0, 0.0);
        mMarkerScreenPoint = new PointF();
        mMarkerScreenMatrix = new Matrix();

        mAccuracyPaintFill = new Paint();
        mAccuracyPaintFill.setAntiAlias(true);
        mAccuracyPaintFill.setStyle(Paint.Style.FILL);
        mAccuracyPaintFill.setColor(BLUE_COLOR);
        mAccuracyPaintFill.setAlpha((int) (255 * 0.25f));

        mAccuracyPaintStroke = new Paint();
        mAccuracyPaintStroke.setAntiAlias(true);
        mAccuracyPaintStroke.setStyle(Paint.Style.STROKE);
        mAccuracyPaintStroke.setStrokeWidth(0.5f * mDensity);
        mAccuracyPaintStroke.setColor(BLUE_COLOR);
        mAccuracyPaintStroke.setAlpha((int) (255 * 0.5f));

        mAccuracyPath = new Path();
        mAccuracyBounds = new RectF();

        mUserLocationDrawable = ContextCompat.getDrawable(getContext(), R.drawable.user_location);
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

        mUserLocationBearingDrawable = ContextCompat.getDrawable(getContext(), R.drawable.user_location_bearing);
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

        mUserLocationStaleDrawable = ContextCompat.getDrawable(getContext(), R.drawable.user_location_stale);
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

        boolean willDraw =
                mShowAccuracy && !mStaleMarker && !canvas.quickReject(mAccuracyPath, Canvas.EdgeType.AA);
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
            mMapView.setCenterCoordinate(new LatLng(mUserLocation.getLatitude(), mUserLocation.getLongitude()));
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
            mMarkerScreenPoint = mMapView.toScreenLocation(mMarkerCoordinate);
            mMarkerScreenMatrix.reset();
            mMarkerScreenMatrix.setTranslate(
                    mMarkerScreenPoint.x,
                    mMarkerScreenPoint.y);

            // rotate so arrow in points to bearing
            if (mShowDirection) {
                mMarkerScreenMatrix.preRotate(mMarkerDirection +
                        (float) mMapView.getDirection());
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

    public void setMyBearingTrackingMode(@MyBearingTracking.Mode int myBearingTrackingMode) {
        mMyBearingTrackingMode = myBearingTrackingMode;

        if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
            mBearingChangeListener.onStart(getContext());
        } else {
            mBearingChangeListener.onStop();
        }
    }

    @MyBearingTracking.Mode
    public int getMyBearingTrackingMode() {
        return mMyBearingTrackingMode;
    }

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
        private static final int UPDATE_RATE_MS = 500;
        private AngleLowPassFilter mLowPassFilter;

        // Compass data
        private float mCompassBearing;
        private long mCompassUpdateNextTimestamp = 0;

        public MyBearingListener(Context context) {
            mSensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
            mSensorRotationVector = mSensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);
            mLowPassFilter = new AngleLowPassFilter();
        }

        public void onStart(Context context) {
            mRotationDevice = ((WindowManager) context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getRotation();
            mSensorManager.registerListener(this, mSensorRotationVector, UPDATE_RATE_MS * 1000);
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

            mLowPassFilter.add(mOrientation[0]);
            long currentTime = System.currentTimeMillis();
            if (currentTime < mCompassUpdateNextTimestamp) {
                return;
            }

            mCompassUpdateNextTimestamp = currentTime + UPDATE_RATE_MS;
            mGeomagneticField = new GeomagneticField(
                    (float) mUserLocation.getLatitude(),
                    (float) mUserLocation.getLongitude(),
                    (float) mUserLocation.getAltitude(),
                    currentTime);
            mCompassBearing = (float) Math.toDegrees(mLowPassFilter.average()) + mGeomagneticField.getDeclination();
            setCompass(mCompassBearing);
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {
            // TODO add accuracy to the equiation
        }

        private class AngleLowPassFilter {

            private final int LENGTH = 5;

            private float sumSin, sumCos;

            private ArrayDeque<Float> queue = new ArrayDeque<>();

            public void add(float radians) {
                sumSin += (float) Math.sin(radians);
                sumCos += (float) Math.cos(radians);
                queue.add(radians);

                if (queue.size() > LENGTH) {
                    float old = queue.poll();
                    sumSin -= Math.sin(old);
                    sumCos -= Math.cos(old);
                }
            }

            public float average() {
                int size = queue.size();
                return (float) Math.atan2(sumSin / size, sumCos / size);
            }
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

    private boolean isStale(Location location) {
        if (location != null) {
            long ageInNanos;
            if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
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
                    mMarkerDirection = mUserLocation.getBearing();
                }
                float oldDir = mMarkerDirection;
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
            // set bearing on map
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
            float oldDir = mMarkerDirection;
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
            mMarkerDirection = bearing;
        } else if (mMyLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
            // change map direction
            if (mMyBearingTrackingMode == MyBearingTracking.COMPASS) {
                mMarkerDirection = bearing;
                mMapView.setBearing(mMarkerDirection);
            }
        }
    }

    void updateOnNextFrame() {
        mMapView.update();
    }

    public void pause() {
        mPaused = true;
        toggleGps(false);
    }

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
        return mMarkerDirection;
    }

    // public for animator only
    public void setDirection(float direction) {
        mMarkerDirection = direction % 360.0f;
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
