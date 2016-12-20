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
import android.graphics.RectF;
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
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;

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
    private float[] projectedCoordinate = new float[2];
    private float projectedX;
    private float projectedY;

    private float contentPaddingX;
    private float contentPaddingY;

    private LatLng latLng;
    private Location location;
    private long locationUpdateTimestamp;
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
    private Drawable backgroundBearingDrawable;

    private int foregroundTintColor;
    private int backgroundTintColor;

    private final Rect foregroundBounds = new Rect();
    private final Rect foregroundBearingBounds = new Rect();
    private final Rect backgroundBounds = new Rect();
    private final Rect backgroundBearingBounds = new Rect();

    private int backgroundOffsetLeft;
    private int backgroundOffsetTop;
    private int backgroundOffsetRight;
    private int backgroundOffsetBottom;

    private Matrix matrix;
    private Camera camera;
    private PointF screenLocation;

    // camera vars
    private double tilt;
    private double bearing;
    private float magneticHeading;

    // Controls the compass update rate in milliseconds
    private static final int COMPASS_UPDATE_RATE_MS = 500;

    @MyLocationTracking.Mode
    private int myLocationTrackingMode;

    @MyBearingTracking.Mode
    private int myBearingTrackingMode;

    private GpsLocationListener userLocationListener;
    private CompassListener compassListener;
    private WindowManager mWindowManager;

    private final RectF drawRect = new RectF();

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
        if (isInEditMode()) {
            return;
        }

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
        setEnabled(false);
        mWindowManager = (WindowManager) context.getApplicationContext().getSystemService(Context.WINDOW_SERVICE);
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

        foregroundDrawable = defaultDrawable;
        foregroundBearingDrawable = bearingDrawable;

        invalidateBounds();
    }

    public final void setForegroundDrawableTint(@ColorInt int color) {
        if (color != Color.TRANSPARENT) {
            if (foregroundDrawable != null) {
                foregroundDrawable.mutate().setColorFilter(color, PorterDuff.Mode.SRC_IN);
            }
            if (foregroundBearingDrawable != null) {
                foregroundBearingDrawable.mutate().setColorFilter(color, PorterDuff.Mode.SRC_IN);
            }
        }
        invalidate();
    }

    public final void setShadowDrawable(Drawable defaultDrawable, Drawable bearingDrawable) {
        setShadowDrawable(defaultDrawable, bearingDrawable, 0, 0, 0, 0);
    }

    public final void setShadowDrawable(Drawable defaultDrawable, Drawable bearingDrawable, int left, int top, int right, int bottom) {
        backgroundDrawable = defaultDrawable;
        backgroundBearingDrawable = bearingDrawable;

        backgroundOffsetLeft = left;
        backgroundOffsetTop = top;
        backgroundOffsetRight = right;
        backgroundOffsetBottom = bottom;

        invalidateBounds();
    }

    public final void setShadowDrawableTint(@ColorInt int color) {
        if (color != Color.TRANSPARENT) {
            backgroundTintColor = color;
            if (backgroundDrawable != null) {
                backgroundDrawable.mutate().setColorFilter(color, PorterDuff.Mode.SRC_IN);
            }
            if (backgroundBearingDrawable != null) {
                backgroundBearingDrawable.mutate().setColorFilter(color, PorterDuff.Mode.SRC_IN);
            }

            if (backgroundDrawable == null) {
                return;
            }
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
        int horizontalOffset = backgroundOffsetLeft - backgroundOffsetRight;
        int verticalOffset = backgroundOffsetTop - backgroundOffsetBottom;
        computeBounds(backgroundDrawable, backgroundBounds, horizontalOffset, verticalOffset);
        computeBounds(backgroundBearingDrawable, backgroundBearingBounds, horizontalOffset, verticalOffset);
        computeBounds(foregroundDrawable, foregroundBounds);
        computeBounds(foregroundBearingDrawable, foregroundBearingBounds);

        // invoke a new draw
        invalidate();
    }

    private void computeBounds(Drawable drawable, Rect bounds) {
        computeBounds(drawable, bounds, 0, 0);
    }

    private void computeBounds(Drawable drawable, Rect bounds, int horizontalOffset, int verticalOffset) {
        if (drawable != null) {
            int halfWidth = drawable.getIntrinsicWidth() / 2;
            int halfHeight = drawable.getIntrinsicHeight() / 2;
            bounds.set(-halfWidth + horizontalOffset, -halfHeight + verticalOffset, halfWidth + horizontalOffset, halfHeight + verticalOffset);
            drawable.setBounds(bounds);
        }
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (location == null || foregroundBounds == null || foregroundBearingBounds == null || accuracyAnimator == null || screenLocation == null) {
            // Not ready yet
            return;
        }

        final PointF pointF = screenLocation;
        float metersPerPixel = (float) projection.getMetersPerPixelAtLatitude(location.getLatitude());
        float accuracyPixels = (Float) accuracyAnimator.getAnimatedValue() / metersPerPixel / 2;
        float maxRadius = getWidth() / 2;
        accuracyPixels = accuracyPixels <= maxRadius ? accuracyPixels : maxRadius;

        // reset
        matrix.reset();
        projectedCoordinate[0] = 0;
        projectedCoordinate[1] = 0;

        // put camera in position
        camera.save();
        camera.rotate((float) tilt, 0, 0);
        // map camera matrix on our matrix
        camera.getMatrix(matrix);

        if (myBearingTrackingMode != MyBearingTracking.NONE && directionAnimator != null) {
            matrix.preRotate((Float) directionAnimator.getAnimatedValue());
        }

        matrix.preTranslate(0, contentPaddingY);
        matrix.postTranslate(pointF.x, pointF.y - contentPaddingY);

        // concat our matrix on canvas
        canvas.concat(matrix);

        // calculate focal point
        matrix.mapPoints(projectedCoordinate);
        projectedX = pointF.x - projectedCoordinate[0];
        projectedY = pointF.y - projectedCoordinate[1];

        // restore orientation from camera
        camera.restore();

        // draw circle
        canvas.drawCircle(0, 0, accuracyPixels, accuracyPaint);

        if (myBearingTrackingMode == MyBearingTracking.NONE) {
            draw(canvas, foregroundDrawable, backgroundDrawable);
        } else {
            draw(canvas, foregroundBearingDrawable, backgroundBearingDrawable);
        }
    }

    public RectF getDrawRect() {
        if (myBearingTrackingMode == MyBearingTracking.NONE) {
            drawRect.set(foregroundDrawable.getBounds());
        } else {
            drawRect.set(foregroundBearingDrawable.getBounds());
        }
        drawRect.offset(screenLocation.x, screenLocation.y); //TODO manage orientation when map rotation will be activated
        return drawRect;
    }

    private void draw(Canvas canvas, Drawable foreground, Drawable background) {
        if (background != null) {
            background.draw(canvas);
        }
        if (foreground != null) {
            foreground.draw(canvas);
        }
    }

    public void setTilt(@FloatRange(from = 0, to = 60.0f) double tilt) {
        this.tilt = (float) tilt;
        if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
            mapboxMap.getUiSettings().setFocalPoint(new PointF(getCenterX(), getCenterY()));
        }
    }

    public void setBearing(double newBearing) {
        // take account of screen rotation away from its natural rotation
        int rotation = mWindowManager.getDefaultDisplay().getRotation();
        float screen_adjustment = 0;
        switch (rotation) {
            case Surface.ROTATION_0:
                screen_adjustment = 0;
                break;
            case Surface.ROTATION_90:
                screen_adjustment = 90;
                break;
            case Surface.ROTATION_180:
                screen_adjustment = 180;
                break;
            case Surface.ROTATION_270:
                screen_adjustment = 270;
                break;
        }
        bearing = (newBearing - screen_adjustment) % 360;
        setCompass(magneticHeading - bearing);
    }

    /* Mappy : redefined above without tracking notion
    public void setBearing(double bearing) {
        this.bearing = bearing;
        if (myLocationTrackingMode == MyLocationTracking.TRACKING_NONE) {
            if (myBearingTrackingMode == MyBearingTracking.GPS) {
                setCompass(location.getBearing() - bearing);
            } else if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
                setCompass(magneticHeading - bearing);
            }
        }
    } Mappy */

    public void setCameraPosition(CameraPosition position) {
        setTilt(position.tilt);
        //Mappy, the location marker is free from camara position bearing
        // setBearing(position.bearing);
    }

    public void onResume() {
        if (isEnabled()) {
            if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
                compassListener.onResume();
            }
            toggleGps(true);
        }
    }

    public void onPause() {
        compassListener.onPause();
        toggleGps(false);
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        // cleanup to prevent memory leaks
        if (locationChangeAnimator != null) {
            locationChangeAnimator.cancel();
            locationChangeAnimator = null;
        }

        if (accuracyAnimator != null) {
            accuracyAnimator.cancel();
            accuracyAnimator = null;
        }

        if (directionAnimator != null) {
            directionAnimator.cancel();
            directionAnimator = null;
        }

        if (userLocationListener != null) {
            LocationServices services = LocationServices.getLocationServices(getContext());
            services.removeLocationListener(userLocationListener);
            userLocationListener = null;
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

        if (enabled) {
            setVisibility(View.VISIBLE);
            if (myBearingTrackingMode == MyBearingTracking.COMPASS) {
                compassListener.onResume();
            } else {
                compassListener.onPause();
            }
        } else {
            setVisibility(View.INVISIBLE);
            compassListener.onPause();
        }
        toggleGps(enabled);
    }

    @Override
    protected Parcelable onSaveInstanceState() {
        Bundle bundle = new Bundle();
        bundle.putParcelable("superState", super.onSaveInstanceState());
        bundle.putDouble("tilt", tilt);
        return bundle;
    }

    @Override
    public void onRestoreInstanceState(Parcelable state) {
        if (state instanceof Bundle) {
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
            userLocationListener = null;
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
                setCompass(0);
            } else {
                myLocationBehavior.invalidate();
            }
        }
        invalidate();
    }

    public void setMyLocationTrackingMode(@MyLocationTracking.Mode int myLocationTrackingMode) {
        MyLocationBehaviorFactory factory = new MyLocationBehaviorFactory();
        myLocationBehavior = factory.getBehavioralModel(myLocationTrackingMode);

        if (location != null) {
            if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
                // center map directly
                mapboxMap.easeCamera(CameraUpdateFactory.newLatLng(new LatLng(location)), 0, false /*linear interpolator*/, false /*do not disable tracking*/, null);
            } else {
                // do not use interpolated location from tracking mode
                latLng = null;
            }
            myLocationBehavior.updateLatLng(location);
        }

        this.myLocationTrackingMode = myLocationTrackingMode;
        invalidate();
    }

    private void setCompass(double bearing) {
        float oldDir = previousDirection;
        if (directionAnimator != null) {
            oldDir = (Float) directionAnimator.getAnimatedValue();
            directionAnimator.end();
            directionAnimator = null;
        }

        float newDir = (float) bearing;
        float diff = oldDir - newDir;
        if (diff > 180.0f) {
            newDir += 360.0f;
        } else if (diff < -180.0f) {
            newDir -= 360.f;
        }
        previousDirection = newDir;

        directionAnimator = ValueAnimator.ofFloat(oldDir, newDir);
        directionAnimator.setDuration(COMPASS_UPDATE_RATE_MS);
        directionAnimator.addUpdateListener(invalidateSelfOnUpdateListener);
        directionAnimator.start();
    }

    public float getCenterX() {
        return (getX() + getMeasuredWidth()) / 2 + contentPaddingX - projectedX;
    }

    public float getCenterY() {
        return (getY() + getMeasuredHeight()) / 2 + contentPaddingY - projectedY;
    }

    public void setContentPadding(int[] padding) {
        contentPaddingX = (padding[0] - padding[2]) / 2;
        contentPaddingY = (padding[1] - padding[3]) / 2;
    }

    private static class GpsLocationListener implements LocationListener {

        private WeakReference<MyLocationView> userLocationView;

        GpsLocationListener(MyLocationView myLocationView) {
            userLocationView = new WeakReference<>(myLocationView);
        }

        /**
         * Callback method for receiving location updates from LocationServices.
         *
         * @param location The new Location data
         */
        @Override
        public void onLocationChanged(Location location) {
            MyLocationView locationView = userLocationView.get();
            if (locationView != null) {
                locationView.setLocation(location);
            }
        }
    }

    private class CompassListener implements SensorEventListener {

        private final SensorManager sensorManager;

        private Sensor rotationVectorSensor;
        float[] matrix = new float[9];
        float[] orientation = new float[3];

        // Compass data
        private long compassUpdateNextTimestamp = 0;

        CompassListener(Context context) {
            sensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
            rotationVectorSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);
        }

        public void onResume() {
            sensorManager.registerListener(this, rotationVectorSensor, SensorManager.SENSOR_DELAY_NORMAL);
        }

        public void onPause() {
            sensorManager.unregisterListener(this, rotationVectorSensor);
        }

        @Override
        public void onSensorChanged(SensorEvent event) {

            // check when the last time the compass was updated, return if too soon.
            long currentTime = SystemClock.elapsedRealtime();
            if (currentTime < compassUpdateNextTimestamp) {
                return;
            }

            if (event.sensor.getType() == Sensor.TYPE_ROTATION_VECTOR) {

                /**
                 * Mappy Fix
                 * On some Samsung devices (e.g. Galaxy Note 3 and Galaxy S4) the
                 * SensorManager.getRotationMatrixFromVector() appears to throw an
                 * exception when the passed rotation vector has length > 4.
                 * In this case
                 */
                try {
                    // calculate the rotation matrix
                    SensorManager.getRotationMatrixFromVector(matrix, event.values);
                    SensorManager.getOrientation(matrix, orientation);

                    float magneticHeading = (float) Math.toDegrees(orientation[0]);
                    setCompass(magneticHeading - bearing);
                } catch (Exception e) {
                    e.printStackTrace();
                }

                /* Mappy, the bearing of the sensor doesn't allow to rotate the map
                if (myLocationTrackingMode == MyLocationTracking.TRACKING_FOLLOW) {
                    // Change the user location view orientation to reflect the device orientation
                    rotateCamera(magneticHeading);
                    setCompass(0);
                } else {
                    // Change compass direction
                    setCompass(magneticHeading - bearing);
                }*/

                compassUpdateNextTimestamp = currentTime + COMPASS_UPDATE_RATE_MS;
            }
        }


        private void rotateCamera(float rotation) {
            CameraPosition.Builder builder = new CameraPosition.Builder();
            builder.bearing(rotation);
            mapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(builder.build()), COMPASS_UPDATE_RATE_MS, false /*linear interpolator*/, false /*do not disable tracking*/, null);
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

        MyLocationBehavior getBehavioralModel(@MyLocationTracking.Mode int mode) {
            if (mode == MyLocationTracking.TRACKING_NONE) {
                return new MyLocationShowBehavior();
            } else {
                return new MyLocationTrackingBehavior();
            }
        }
    }

    private abstract class MyLocationBehavior {

        void updateLatLng(@NonNull Location newLocation) {
            location = newLocation;
        }

        void updateLatLng(double lat, double lon) {
            if (latLng != null) {
                latLng.setLatitude(lat);
                latLng.setLongitude(lon);
            }
        }

        void updateAccuracy(@NonNull Location location) {
            if (location.getAccuracy() == accuracy) {
                // nothing change for accuracy
                return;
            }

            if (accuracyAnimator != null) {
                if (accuracyAnimator.isRunning()) {
                    // use current accuracy as a starting point
                    accuracy = (Float) accuracyAnimator.getAnimatedValue();
                }
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
            super.updateLatLng(location);
            if (latLng == null) {
                // first location fix
                latLng = new LatLng(location);
                locationUpdateTimestamp = SystemClock.elapsedRealtime();
            }

            // updateLatLng timestamp
            float previousUpdateTimeStamp = locationUpdateTimestamp;
            locationUpdateTimestamp = SystemClock.elapsedRealtime();

            // calculate animation duration
            float animationDuration;
            if (previousUpdateTimeStamp == 0) {
                animationDuration = 0;
            } else {
                animationDuration = 750L;// (locationUpdateTimestamp - previousUpdateTimeStamp) * 1.1f /*make animation slightly longer*/;
            }

            // calculate interpolated location
            latLng = new LatLng(location);
            CameraPosition.Builder builder = new CameraPosition.Builder().target(latLng);

            // add direction
            if (myBearingTrackingMode == MyBearingTracking.GPS) {
                if (location.hasBearing()) {
                    builder.bearing(location.getBearing());
                }
                setCompass(0);
            }

            // accuracy
            updateAccuracy(location);

            // ease to new camera position with a linear interpolator
            mapboxMap.easeCamera(CameraUpdateFactory.newCameraPosition(builder.build()), (int) animationDuration, false /*linear interpolator*/, false /*do not disable tracking*/, null);
        }

        @Override
        void invalidate() {
            int[] mapPadding = mapboxMap.getPadding();
            float x = (getWidth() + mapPadding[0] - mapPadding[2]) / 2 + contentPaddingX;
            float y = (getHeight() - mapPadding[3] + mapPadding[1]) / 2 + contentPaddingY;
            screenLocation = new PointF(x, y);
            MyLocationView.this.invalidate();
        }
    }

    private class MyLocationShowBehavior extends MyLocationBehavior {

        @Override
        void updateLatLng(@NonNull final Location location) {
            super.updateLatLng(location);
            if (latLng == null) {
                // first location update
                latLng = new LatLng(location);
                locationUpdateTimestamp = SystemClock.elapsedRealtime();
            }

            // update LatLng location
            LatLng newLocation = new LatLng(location);

            // update LatLng accuracy
            updateAccuracy(location);

            // calculate updateLatLng time + add some extra offset to improve animation
            long previousUpdateTimeStamp = locationUpdateTimestamp;
            locationUpdateTimestamp = SystemClock.elapsedRealtime();
            long locationUpdateDuration = 750L; //(long) ((locationUpdateTimestamp - previousUpdateTimeStamp) * 1.2f);

            // animate changes
            if (locationChangeAnimator != null) {
                locationChangeAnimator.end();
                locationChangeAnimator = null;
            }

            locationChangeAnimator = ValueAnimator.ofFloat(0.0f, 1.0f);
            locationChangeAnimator.setDuration(locationUpdateDuration);
            locationChangeAnimator.addUpdateListener(new MarkerCoordinateAnimatorListener(this,
                    latLng, newLocation
            ));
            locationChangeAnimator.start();
            latLng = newLocation;
        }

        @Override
        void invalidate() {
            if (latLng != null) {
                screenLocation = projection.toScreenLocation(latLng);
            }
            MyLocationView.this.invalidate();
        }
    }
}
