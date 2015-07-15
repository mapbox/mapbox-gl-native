package com.mapbox.mapboxgl.testapp;

import android.content.Context;
import android.graphics.Matrix;
import android.graphics.PointF;
import android.hardware.GeomagneticField;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Location;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.TextView;

import com.mapbox.mapboxgl.annotations.Marker;
import com.mapbox.mapboxgl.annotations.MarkerOptions;
import com.mapbox.mapboxgl.geometry.LatLng;
import com.mapbox.mapboxgl.views.MapView;
import com.mapzen.android.lost.api.LocationListener;
import com.mapzen.android.lost.api.LocationRequest;
import com.mapzen.android.lost.api.LocationServices;
import com.mapzen.android.lost.api.LostApiClient;

import java.util.ArrayList;

public class MainActivity extends ActionBarActivity {

    //
    // Static members
    //

    // Used for saving instance state
    private static final String STATE_IS_GPS_ON = "isGpsOn";

    //
    // Instance members
    //

    // Used for the UI
    private MapFragment mMapFragment;
    private TextView mFpsTextView;
    private ImageView mCompassView;
    private FrameLayout mMapFrameLayout;
    private float mDensity;
    private Spinner mClassSpinner;
    private ArrayAdapter mOutdoorsClassAdapter;
    private ArrayAdapter mSatelliteClassAdapter;

    // Used for GPS
    private boolean mIsGpsOn = false;
    private LostApiClient mLocationClient;
    private GpsListener mGpsListener;
    private LocationRequest mLocationRequest;
    private ImageView mGpsMarker;
    private Location mGpsLocation;
    private MenuItem mGpsMenuItem;

    // Used for compass
    private SensorManager mSensorManager;
    private Sensor mSensorAccelerometer;
    private Sensor mSensorMagneticField;
    private CompassListener mCompassListener;
    private float[] mValuesAccelerometer = new float[3];
    private float[] mValuesMagneticField = new float[3];
    private float[] mMatrixR = new float[9];
    private float[] mMatrixI = new float[9];
    private float[] mMatrixValues = new float[3];
    private float mCompassBearing;
    private boolean mCompassValid = false;

    // Used for markers
    private boolean mIsMarkersOn = false;

    private Marker marker;

    //
    // Lifecycle events
    //

    // Called when activity is created
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (savedInstanceState != null) {
            mIsGpsOn = savedInstanceState.getBoolean(STATE_IS_GPS_ON, false);
        }

        mDensity = getResources().getDisplayMetrics().density;

        // Load the layout
        setContentView(R.layout.activity_main);
        mMapFragment = (MapFragment) getSupportFragmentManager().findFragmentById(R.id.fragment_map);
        mMapFragment.getMap().setOnFpsChangedListener(new MyOnFpsChangedListener());
        mMapFragment.getMap().setOnMapChangedListener(new MyOnMapChangedListener());

        mFpsTextView = (TextView) findViewById(R.id.view_fps);
        mFpsTextView.setText("");

        mCompassView = (ImageView) findViewById(R.id.view_compass);
        mCompassView.setOnClickListener(new CompassOnClickListener());

        mGpsMarker = new ImageView(getApplicationContext());
        mGpsMarker.setVisibility(View.INVISIBLE);
        mGpsMarker.setImageResource(R.drawable.location_marker);
        mGpsMarker.setLayoutParams(new FrameLayout.LayoutParams((int) (27.0f * mDensity), (int) (27.0f * mDensity)));
        mGpsMarker.requestLayout();

        mMapFrameLayout = (FrameLayout) findViewById(R.id.layout_map);
        mMapFrameLayout.addView(mGpsMarker);
        // Add a toolbar as the action bar
        Toolbar mainToolbar = (Toolbar) findViewById(R.id.toolbar_main);
        setSupportActionBar(mainToolbar);
        getSupportActionBar().setDisplayShowTitleEnabled(false);

        // Add the spinner to select map styles
        Spinner styleSpinner = (Spinner) findViewById(R.id.spinner_style);
        ArrayAdapter styleAdapter = ArrayAdapter.createFromResource(getSupportActionBar().getThemedContext(),
                R.array.style_list, android.R.layout.simple_spinner_dropdown_item);
        styleSpinner.setAdapter(styleAdapter);
        styleSpinner.setOnItemSelectedListener(new StyleSpinnerListener());

        // Add the spinner to select class styles
        mClassSpinner = (Spinner) findViewById(R.id.spinner_class);
        mOutdoorsClassAdapter = ArrayAdapter.createFromResource(getSupportActionBar().getThemedContext(),
                R.array.outdoors_class_list, android.R.layout.simple_spinner_dropdown_item);

        // Prepare for GPS
        mLocationClient = new LostApiClient.Builder(this).build();
        mLocationRequest = LocationRequest.create()
                .setInterval(1000)
                .setSmallestDisplacement(1)
                .setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY);
        mGpsListener = new GpsListener();

        mSensorManager = (SensorManager) getApplicationContext().getSystemService(Context.SENSOR_SERVICE);
        mSensorAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mSensorMagneticField = mSensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
        mCompassListener = new CompassListener();
    }

    // Called when our app goes into the background
    @Override
    public void onPause()  {
        super.onPause();

        // Cancel GPS
        toggleGps(false);
    }

    // Called when our app comes into the foreground
    @Override
    public void onResume() {
        super.onResume();
        // Restart GPS
        toggleGps(true);
    }

    // Called when we need to save instance state
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);

        outState.putBoolean(STATE_IS_GPS_ON, mIsGpsOn);
    }


    //
    // Other events
    //

    // Adds items to the action bar menu
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu_main, menu);
        mGpsMenuItem = menu.findItem(R.id.action_gps);
        if (mIsGpsOn) {
            mGpsMenuItem.setIcon(R.drawable.ic_action_location_found);
        } else {
            mGpsMenuItem.setIcon(R.drawable.ic_action_location_searching);
        }
        return super.onCreateOptionsMenu(menu);
    }

    // Called when pressing action bar items
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_gps:
                // Toggle GPS position updates
                toggleGps(!mIsGpsOn);
                updateMap();
                return true;

            case R.id.action_debug:
                // Toggle debug mode
                mMapFragment.getMap().toggleDebug();

                // Show the FPS counter
                if (mMapFragment.getMap().isDebugActive()) {
                    mFpsTextView.setVisibility(View.VISIBLE);
                    mFpsTextView.setText(getResources().getString(R.string.label_fps));
                } else {
                    mFpsTextView.setVisibility(View.INVISIBLE);
                }
                return true;

            case R.id.action_markers:
                // Toggle markers
                toggleMarkers(!mIsMarkersOn);

            default:
                return super.onOptionsItemSelected(item);
        }
    }

    /**
     * Enabled / Disable GPS location updates along with updating the UI
     * @param enableGps true if GPS is to be enabled, false if GPS is to be disabled
     */
    private void toggleGps(boolean enableGps) {

        if (enableGps) {
            if (!mIsGpsOn) {
                mIsGpsOn = true;
                if (mGpsMenuItem != null) {
                    mGpsMenuItem.setIcon(R.drawable.ic_action_location_found);
                }
                mGpsLocation = null;
                mLocationClient.connect();
                updateLocation(LocationServices.FusedLocationApi.getLastLocation());
                LocationServices.FusedLocationApi.requestLocationUpdates(mLocationRequest, new GpsListener());
                mSensorManager.registerListener(mCompassListener, mSensorAccelerometer, SensorManager.SENSOR_DELAY_UI);
                mSensorManager.registerListener(mCompassListener, mSensorMagneticField, SensorManager.SENSOR_DELAY_UI);
            }
        } else {
            if (mIsGpsOn) {
                mIsGpsOn = false;
                if (mGpsMenuItem != null) {
                    mGpsMenuItem.setIcon(R.drawable.ic_action_location_searching);
                }
                LocationServices.FusedLocationApi.removeLocationUpdates(mGpsListener);
                mLocationClient.disconnect();
                mSensorManager.unregisterListener(mCompassListener, mSensorAccelerometer);
                mSensorManager.unregisterListener(mCompassListener, mSensorMagneticField);
                mGpsLocation = null;
            }
        }
    }

    /**
     * Enable / Disable markers.
     *
     * @param enableMarkers
     */
    private void toggleMarkers(boolean enableMarkers) {
        if (enableMarkers) {
            if (!mIsMarkersOn) {
                mIsMarkersOn = true;
                addMarkers();
            }
        } else {
            if (mIsMarkersOn) {
                mIsMarkersOn = false;
                removeMarkers();
            }
        }
    }

    private void addMarkers() {
        LatLng backLot = new LatLng(38.649441, -121.369064);
        MapView map = mMapFragment.getMap();
        marker = map.addMarker(new MarkerOptions()
            .position(backLot)
            .title("Back Lot")
            .snippet("The back lot behind my house"));
    }

    private void removeMarkers() {
        marker.remove();
    }

    // This class forwards location updates to updateLocation()
    private class GpsListener implements LocationListener {

        @Override
        public void onLocationChanged(Location location) {
            updateLocation(location);
        }
    }

    // This class handles sensor updates to calculate compass bearing
    private class CompassListener implements SensorEventListener {

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

            boolean valid = SensorManager.getRotationMatrix(mMatrixR, mMatrixI,
                    mValuesAccelerometer,
                    mValuesMagneticField);

            if (valid) {
                SensorManager.getOrientation(mMatrixR, mMatrixValues);
                //mAzimuthRadians.putValue(mMatrixValues[0]);
                //mAzimuth = Math.toDegrees(mAzimuthRadians.getAverage());

                if (mGpsLocation != null) {
                    GeomagneticField geomagneticField = new GeomagneticField(
                            (float) mGpsLocation.getLatitude(),
                            (float) mGpsLocation.getLongitude(),
                            (float) mGpsLocation.getAltitude(),
                            System.currentTimeMillis());
                    mCompassBearing = (float) Math.toDegrees(mMatrixValues[0]) + geomagneticField.getDeclination();
                    mCompassValid = true;
                }
            }

            updateMap();
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {
            // TODO: ignore unreliable stuff
        }
    }

    // Handles location updates from GPS
    private void updateLocation(Location location) {
        if (location != null) {
            mGpsLocation = location;
        }

        updateMap();
    }

    // This class handles style change events
    private class StyleSpinnerListener implements AdapterView.OnItemSelectedListener {

        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            switch (position) {

                // Mapbox Streets
                case 0:
                    mMapFragment.getMap().setStyleUrl("asset://styles/mapbox-streets-v7.json");
                    mMapFragment.getMap().removeAllClasses();
                    mClassSpinner.setVisibility(View.INVISIBLE);
                    mClassSpinner.setAdapter(null);
                    mClassSpinner.setOnItemSelectedListener(null);
                    break;

                // Emerald
                case 1:
                    mMapFragment.getMap().setStyleUrl("asset://styles/emerald-v7.json");
                    mMapFragment.getMap().removeAllClasses();
                    mClassSpinner.setVisibility(View.INVISIBLE);
                    mClassSpinner.setAdapter(null);
                    mClassSpinner.setOnItemSelectedListener(null);
                    break;

                // Light
                case 2:
                    mMapFragment.getMap().setStyleUrl("asset://styles/light-v7.json");
                    mMapFragment.getMap().removeAllClasses();
                    mClassSpinner.setVisibility(View.INVISIBLE);
                    mClassSpinner.setAdapter(null);
                    mClassSpinner.setOnItemSelectedListener(null);
                    break;

                // Dark
                case 3:
                    mMapFragment.getMap().setStyleUrl("asset://styles/dark-v7.json");
                    mMapFragment.getMap().removeAllClasses();
                    mClassSpinner.setVisibility(View.INVISIBLE);
                    mClassSpinner.setAdapter(null);
                    mClassSpinner.setOnItemSelectedListener(null);
                    break;

                // Outdoors
                case 4:
                    mMapFragment.getMap().setStyleUrl("asset://styles/outdoors-v7.json");
                    mMapFragment.getMap().removeAllClasses();
                    mClassSpinner.setVisibility(View.VISIBLE);
                    mClassSpinner.setAdapter(mOutdoorsClassAdapter);
                    mClassSpinner.setOnItemSelectedListener(new OutdoorClassSpinnerListener());
                    break;

                default:
                    onNothingSelected(parent);
                    break;
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> parent) {
            mMapFragment.getMap().setStyleUrl("");
        }
    }

    // This class handles outdoor class change events
    private class OutdoorClassSpinnerListener implements AdapterView.OnItemSelectedListener {

        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            ArrayList<String> classes = new ArrayList<>(1);

            switch (position) {
                // Day
                case 0:
                    classes.add("day");
                    mMapFragment.getMap().setClasses(classes);
                    break;

                // Night
                case 1:
                    classes.add("night");
                    mMapFragment.getMap().setClasses(classes);
                    break;

                default:
                    onNothingSelected(parent);
                    break;
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> parent) {
            mMapFragment.getMap().removeAllClasses();
        }
    }

    // This class handles satellite class change events
    private class SatelliteClassSpinnerListener implements AdapterView.OnItemSelectedListener {

        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            ArrayList<String> classes = new ArrayList<>(2);

            switch (position) {
                // Labels + Contours
                case 0:
                    classes.add("labels");
                    classes.add("contours");
                    mMapFragment.getMap().setClasses(classes);
                    break;

                // Labels Only
                case 1:
                    classes.add("labels");
                    mMapFragment.getMap().setClasses(classes);
                    break;

                // No Labels
                case 2:
                    mMapFragment.getMap().setClasses(classes);
                    break;

                default:
                    onNothingSelected(parent);
                    break;
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> parent) {
            mMapFragment.getMap().removeAllClasses();
        }
    }

    // Called when FPS changes
    private class MyOnFpsChangedListener implements MapView.OnFpsChangedListener {

        @Override
        public void onFpsChanged(double fps) {
            mFpsTextView.setText(getResources().getString(R.string.label_fps) + String.format(" %4.2f", fps));
        }
    }

    // Rotates an ImageView - does not work if the ImageView has padding, use margins
    private void rotateImageView(ImageView imageView, float angle) {
        Matrix matrix = new Matrix();
        matrix.setScale((float) imageView.getWidth() / (float) imageView.getDrawable().getIntrinsicWidth(), (float) imageView.getHeight() / (float) imageView.getDrawable().getIntrinsicHeight());
        matrix.postRotate(angle, (float) imageView.getWidth() / 2.0f, (float) imageView.getHeight() / 2.0f);
        imageView.setImageMatrix(matrix);
        imageView.setScaleType(ImageView.ScaleType.MATRIX);
    }

    // Updates the UI to match the current map's position
    private void updateMap() {
        rotateImageView(mCompassView, (float) mMapFragment.getMap().getDirection());

        if (mGpsLocation != null) {
            mGpsMarker.setVisibility(View.VISIBLE);
            LatLng coordinate = new LatLng(mGpsLocation);
            PointF screenLocation = mMapFragment.getMap().toScreenLocation(coordinate);

            if (mGpsLocation.hasBearing() || mCompassValid) {
                mGpsMarker.setImageResource(R.drawable.direction_arrow);
                FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams((int) (54.0f * mDensity), (int) (54.0f * mDensity));
                lp.leftMargin = (int) ((screenLocation.x - 54.0f / 2.0f) * mDensity);
                lp.topMargin = mMapFrameLayout.getHeight() - (int) ((screenLocation.y + 54.0f / 2.0f) * mDensity);
                mGpsMarker.setLayoutParams(lp);
                float bearing = mGpsLocation.hasBearing() ? mGpsLocation.getBearing() : mCompassBearing;
                rotateImageView(mGpsMarker, bearing);
                mGpsMarker.requestLayout();
            } else {
                mGpsMarker.setImageResource(R.drawable.location_marker);
                FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams((int) (27.0f * mDensity), (int) (27.0f * mDensity));
                lp.leftMargin = (int) ((screenLocation.x - 27.0f / 2.0f) * mDensity);
                lp.topMargin = mMapFrameLayout.getHeight() - (int) ((screenLocation.y + 27.0f / 2.0f) * mDensity);
                mGpsMarker.setLayoutParams(lp);
                rotateImageView(mGpsMarker, 0.0f);
                mGpsMarker.requestLayout();
            }
        } else {
            mGpsMarker.setVisibility(View.INVISIBLE);
        }
    }

    // Called when map state changes
    private class MyOnMapChangedListener implements MapView.OnMapChangedListener {

        @Override
        public void onMapChanged() {
            updateMap();
        }
    }

    // Called when someone presses the compass
    private class CompassOnClickListener implements View.OnClickListener {

        @Override
        public void onClick(View view) {
            mMapFragment.getMap().resetNorth();
        }
    }
}
