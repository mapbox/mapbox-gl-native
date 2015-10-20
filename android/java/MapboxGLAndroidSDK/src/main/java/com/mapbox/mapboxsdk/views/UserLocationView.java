package com.mapbox.mapboxsdk.views;

import android.animation.ObjectAnimator;
import android.animation.ValueAnimator;
import android.content.Context;
import android.content.res.Resources;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.RectF;
import android.location.Location;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapzen.android.lost.api.LocationListener;
import com.mapzen.android.lost.api.LocationRequest;
import com.mapzen.android.lost.api.LocationServices;
import com.mapzen.android.lost.api.LostApiClient;

import java.lang.ref.WeakReference;

final class UserLocationView extends View {

    private WeakReference<MapView> mMapView;

    private float mDensity;
    private PointF mMarkerScreenPoint;
    private Matrix mMarkerScreenMatrix;

    private Paint mOuterRingPaint;
    private Path mOuterRingPath;
    private RectF mOuterRingBounds;

    private Paint mInnerRingPaintFill;
    private Paint mInnerRingPaintStroke;
    private Path mInnerRingPath;

    private boolean mShowDirection;
    private Paint mDirectionArrowPaint;
    private Path mDirectionArrowPath;

    private boolean mShowAccuracy;
    private Paint mAccuracyPaintFill;
    private Paint mAccuracyPaintStroke;
    private Path mAccuracyPath;
    private RectF mAccuracyBounds;

    private Rect mDirtyRect;
    private RectF mDirtyRectF;

    private boolean mShowMarker;
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

    public UserLocationView(MapView mapView, Context context) {
        super(context);
        initialize(mapView, context);
    }

    public UserLocationView(MapView mapView, Context context, AttributeSet attrs) {
        super(context, attrs);
        initialize(mapView, context);
    }

    public UserLocationView(MapView mapView, Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initialize(mapView, context);
    }

    private void initialize(MapView mapView, Context context) {
        mMapView = new WeakReference<MapView>(mapView);

        // View configuration
        setEnabled(false);

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
        mMarkerScreenPoint = new PointF();
        mMarkerScreenMatrix = new Matrix();

        mOuterRingPaint = new Paint();
        mOuterRingPaint.setAntiAlias(true);
        mOuterRingPaint.setStyle(Paint.Style.FILL);
        mOuterRingPaint.setColor(Color.BLACK);
        mOuterRingPaint.setAlpha((int) (255 * 0.5f));

        mOuterRingPath = new Path();
        mOuterRingPath.addCircle(0.0f, 0.0f, 15.0f * mDensity, Path.Direction.CW);

        mOuterRingBounds = new RectF();
        mOuterRingPath.computeBounds(mOuterRingBounds, false);

        mInnerRingPaintFill = new Paint();
        mInnerRingPaintFill.setAntiAlias(true);
        mInnerRingPaintFill.setStyle(Paint.Style.FILL);
        mInnerRingPaintFill.setColor(Color.WHITE);

        mInnerRingPaintStroke = new Paint();
        mInnerRingPaintStroke.setAntiAlias(true);
        mInnerRingPaintStroke.setStyle(Paint.Style.STROKE);
        mInnerRingPaintStroke.setStrokeWidth(1.0f * mDensity);
        mInnerRingPaintStroke.setColor(Color.BLACK);

        mInnerRingPath = new Path();
        mInnerRingPath.addCircle(0.0f, 0.0f, 7.5f * mDensity, Path.Direction.CW);

        mDirectionArrowPaint = mInnerRingPaintFill;

        mDirectionArrowPath = new Path();
        mDirectionArrowPath.moveTo(0.0f, -15.0f * mDensity);
        mDirectionArrowPath.arcTo(
                new RectF(
                        -9.0f * mDensity, -9.0f * mDensity,
                        9.0f * mDensity, 9.0f * mDensity),
                225.0f, 90.0f, false);
        mDirectionArrowPath.close();

        mAccuracyPaintFill = new Paint();
        mAccuracyPaintFill.setAntiAlias(true);
        mAccuracyPaintFill.setStyle(Paint.Style.FILL);
        mAccuracyPaintFill.setColor(Color.BLACK);
        mAccuracyPaintFill.setAlpha((int) (255 * 0.25f));

        mAccuracyPaintStroke = new Paint();
        mAccuracyPaintStroke.setAntiAlias(true);
        mAccuracyPaintStroke.setStyle(Paint.Style.STROKE);
        mAccuracyPaintStroke.setStrokeWidth(0.5f * mDensity);
        mAccuracyPaintStroke.setColor(Color.BLACK);
        mAccuracyPaintStroke.setAlpha((int) (255 * 0.5f));

        mAccuracyPath = new Path();
        mAccuracyBounds = new RectF();
    }

    @Override
    public void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (!mShowMarker) {
            return;
        }

        canvas.concat(mMarkerScreenMatrix);

        if (!canvas.quickReject(mOuterRingPath, Canvas.EdgeType.AA)) {
            if (mShowAccuracy) {
                canvas.drawPath(mAccuracyPath, mAccuracyPaintFill);
                canvas.drawPath(mAccuracyPath, mAccuracyPaintStroke);
            }
            canvas.drawPath(mOuterRingPath, mOuterRingPaint);
            canvas.drawPath(mInnerRingPath, mInnerRingPaintFill);
            canvas.drawPath(mInnerRingPath, mInnerRingPaintStroke);
            if (mShowDirection) {
                canvas.drawPath(mDirectionArrowPath, mDirectionArrowPaint);
            }
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

            // compute new marker position
            // TODO add JNI method that takes existing pointf
            mMarkerScreenPoint = mMapView.get().toScreenLocation(mMarkerCoordinate);
            mMarkerScreenMatrix.reset();
            mMarkerScreenMatrix.setTranslate(
                    mMarkerScreenPoint.x,
                    mMarkerScreenPoint.y);

            // rotate so arrow in points to bearing
            if (mShowDirection) {
                mMarkerScreenMatrix.preRotate(mMarkerDirection +
                        (float) mMapView.get().getDirection());
            }

            // adjust accuracy circle
            if (mShowAccuracy) {
                mAccuracyPath.reset();
                mAccuracyPath.addCircle(0.0f, 0.0f,
                        (float) (mMarkerAccuracy / mMapView.get().getMetersPerPixelAtLatitude(
                                mMarkerCoordinate.getLatitude())),
                        Path.Direction.CW);

                mAccuracyPath.computeBounds(mAccuracyBounds, false);
            }

            // invalidate changed pixels
            if (mDirtyRect == null) {
                mDirtyRect = new Rect();
                mDirtyRectF = new RectF();
            } else {
                // the old marker location
                // TODO need to take into account accuracy stroke
                //invalidate(mDirtyRect);
                invalidate();
            }

            RectF largerBounds;
            if (mAccuracyBounds.contains(mOuterRingBounds)) {
                largerBounds = mAccuracyBounds;
            } else {
                largerBounds = mOuterRingBounds;
            }
            mMarkerScreenMatrix.mapRect(mDirtyRectF, largerBounds);
            mDirtyRectF.roundOut(mDirtyRect);
            // TODO need to take into account accuracy stroke
            //invalidate(mDirtyRect); // the new marker location
            invalidate();
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
            mMarkerCoordinate = null;
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

        mMarkerCoordinateAnimator = ValueAnimator.ofFloat(0.0f, 1.0f);
        mMarkerCoordinateAnimator.setDuration(1000);
        mMarkerCoordinateAnimator.addUpdateListener(new MarkerCoordinateAnimatorListener(
                previousCoordinate, new LatLng(location)
        ));
        mMarkerCoordinateAnimator.start();

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
        mMapView.get().update();
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
