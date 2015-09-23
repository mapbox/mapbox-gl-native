package com.mapbox.mapboxgl.views.widget;

import android.content.Context;
import android.hardware.GeomagneticField;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Location;
import android.support.v4.content.ContextCompat;
import android.util.AttributeSet;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.mapbox.mapboxgl.views.MapView;
import com.mapbox.mapboxgl.views.R;

public class CompassView extends ImageView implements SensorEventListener {

    private CompassDelegate mCompassDelegate;

    // Sensor model
    private SensorManager mSensorManager;
    private Sensor mSensorAccelerometer;
    private Sensor mSensorMagneticField;

    // Sensor data
    private boolean mSensorValid;
    private float[] mValuesAccelerometer = new float[3];
    private float[] mValuesMagneticField = new float[3];
    private float[] mMatrixR = new float[9];
    private float[] mMatrixI = new float[9];
    private float[] mMatrixValues = new float[3];

    // Location data
    private GeomagneticField mGeomagneticField;
    private Location mGpsLocation;

    // Compass values
    private float mCompassBearing;
    private boolean mCompassValid = false;

    public CompassView(Context context) {
        super(context);
        initialize(context);
    }

    public CompassView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initialize(context);
    }

    public CompassView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initialize(context);
    }

    private void initialize(Context context) {
        // Sensor initialisation
        mSensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
        mSensorAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mSensorMagneticField = mSensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);

        // View configuration
        setImageDrawable(ContextCompat.getDrawable(getContext(), R.drawable.compass));
        setContentDescription(getResources().getString(R.string.compassContentDescription));

        // Layout params
        float mScreenDensity = context.getResources().getDisplayMetrics().density;
        FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams((int) (48 * mScreenDensity), (int) (48 * mScreenDensity));
        setLayoutParams(lp);
    }

    public boolean isValid() {
        return mCompassValid;
    }

    public float getBearing() {
        return mCompassBearing;
    }

    public void registerListeners(CompassDelegate compassDelegate) {
        mSensorManager.registerListener(this, mSensorAccelerometer, SensorManager.SENSOR_DELAY_UI);
        mSensorManager.registerListener(this, mSensorMagneticField, SensorManager.SENSOR_DELAY_UI);
        mCompassDelegate = compassDelegate;
    }

    public void unRegisterListeners() {
        mCompassDelegate = null;
        mSensorManager.unregisterListener(this, mSensorMagneticField);
        mSensorManager.unregisterListener(this, mSensorAccelerometer);
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        switch (event.sensor.getType()) {
            case Sensor.TYPE_ACCELEROMETER:
                System.arraycopy(event.values, 0, mValuesAccelerometer, 0, 3);
                break;
            case Sensor.TYPE_MAGNETIC_FIELD:
                System.arraycopy(event.values, 0, mValuesMagneticField, 0, 3);
                break;
        }

        mSensorValid = SensorManager.getRotationMatrix(mMatrixR, mMatrixI,
                mValuesAccelerometer,
                mValuesMagneticField);

        if (mSensorValid && mCompassDelegate != null) {
            SensorManager.getOrientation(mMatrixR, mMatrixValues);
            //mAzimuthRadians.putValue(mMatrixValues[0]);
            //mAzimuth = Math.toDegrees(mAzimuthRadians.getAverage());

            mGpsLocation = mCompassDelegate.getMyLocation();
            if (mGpsLocation != null) {
                mGeomagneticField = new GeomagneticField(
                        (float) mGpsLocation.getLatitude(),
                        (float) mGpsLocation.getLongitude(),
                        (float) mGpsLocation.getAltitude(),
                        System.currentTimeMillis());
                mCompassBearing = (float) Math.toDegrees(mMatrixValues[0]) + mGeomagneticField.getDeclination();
                mCompassValid = true;
            }
        }

        if(mCompassDelegate!=null){
            mCompassDelegate.updateMap(MapView.MapChange.MapChangeNullChange);
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // TODO: ignore unreliable stuff
    }

}