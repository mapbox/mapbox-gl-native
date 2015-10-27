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
import android.location.Location;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapzen.android.lost.api.LocationListener;
import com.mapzen.android.lost.api.LocationRequest;
import com.mapzen.android.lost.api.LocationServices;
import com.mapzen.android.lost.api.LostApiClient;

final class UserLocationView extends View {

    private MapView mMapView;

    private static final int BLUE_COLOR = 0x39ACCBFF;

    private float mDensity;

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

    @Override
    public void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (!mShowMarker) {
            return;
        }

        canvas.concat(mMarkerScreenMatrix);

        Drawable dotDrawable = mShowDirection ? mUserLocationBearingDrawable : mUserLocationDrawable;
        // IMPORTANT also update in uodate()
        RectF dotBounds = mShowDirection ? mUserLocationBearingDrawableBoundsF : mUserLocationDrawableBoundsF;

        boolean willDraw;
        willDraw = mShowAccuracy && !canvas.quickReject(mAccuracyPath, Canvas.EdgeType.AA);
        willDraw |= !canvas.quickReject(dotBounds, Canvas.EdgeType.AA);

        dotBounds.offset(
                (int) -mMarkerScreenPoint.x,
                (int) -mMarkerScreenPoint.y);

        if (willDraw) {
            if (mShowAccuracy) {
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
            if (mShowAccuracy) {
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
            RectF largerBounds = mAccuracyBounds.contains(dotBounds) ? mAccuracyBounds : dotBounds;
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
                setLocation(LocationServices.FusedLocationApi.getLastLocation());
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

    private class MyLocationListener implements LocationListener {
        @Override
        public void onLocationChanged(Location location) {
            if (mPaused) {
                return;
            }
            setLocation(location);
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
