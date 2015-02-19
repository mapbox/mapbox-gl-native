package com.mapbox.mapboxgl.app;

import android.location.Location;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.TextView;

import com.mapbox.mapboxgl.lib.LatLngZoom;
import com.mapbox.mapboxgl.lib.MapView;
import com.mapzen.android.lost.LocationClient;
import com.mapzen.android.lost.LocationListener;
import com.mapzen.android.lost.LocationRequest;

import java.util.ArrayList;

public class MainActivity extends ActionBarActivity {

    //
    // Static members
    //

    // Tag used for logging
    private static final String TAG = "MainActivity";

    //
    // Instance members
    //

    // Holds the MapFragment
    private MapFragment mMapFragment;

    // The FPS label
    private TextView mFpsTextView;

    // The compass
    private ImageView mCompassView;

    // Used for GPS
    private boolean mIsGpsOn = false;
    private LocationClient mLocationClient;
    private LocationListener mLocationListener;
    private LocationRequest mLocationRequest;

    // Used for the class spinner
    Spinner mClassSpinner;
    ArrayAdapter mOutdoorsClassAdapter;
    ArrayAdapter mSatelliteClassAdapter;

    //
    // Lifecycle events
    //

    // Called when activity is created
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.v(TAG, "onCreate");

        // Load the layout
        setContentView(R.layout.activity_main);
        mMapFragment = (MapFragment) getSupportFragmentManager().findFragmentById(R.id.fragment_map);
        mMapFragment.getMap().setOnFpsChangedListener(new MyOnFpsChangedListener());
        mMapFragment.getMap().setOnMapChangedListener(new MyOnMapChangedListener());

        mFpsTextView = (TextView) findViewById(R.id.view_fps);
        mFpsTextView.setText("");

        mCompassView = (ImageView) findViewById(R.id.view_compass);
        mCompassView.setOnClickListener(new CompassOnClickListener());

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
        mSatelliteClassAdapter = ArrayAdapter.createFromResource(getSupportActionBar().getThemedContext(),
                R.array.satellite_class_list, android.R.layout.simple_spinner_dropdown_item);

        // Prepare for GPS
        mLocationRequest = LocationRequest.create();
        mLocationRequest.setInterval(0);
        mLocationRequest.setSmallestDisplacement(0);

        mLocationClient = new LocationClient(getApplicationContext(), new LocationClient.ConnectionCallbacks() {
            @Override
            public void onConnected(Bundle connectionHint) {
                updateLocation(mLocationClient.getLastLocation());

                mLocationListener = new LocationListener() {
                    @Override
                    public void onLocationChanged(Location location) {
                        //Toast.makeText(getApplicationContext(), "Location: " + location.toString(), Toast.LENGTH_SHORT).show();
                        updateLocation(location);
                    }
                };

                mLocationClient.requestLocationUpdates(mLocationRequest, mLocationListener);
            }

            @Override
            public void onDisconnected() {
                mLocationListener = null;
            }
        });
    }

    // Called when our app goes into the background
    @Override
    public void onPause()  {
        super.onPause();
        Log.v(TAG, "onPause");

        // Cancel GPS
        if (mIsGpsOn) {
            mLocationClient.disconnect();
        }
    }

    // Called when our app comes into the foreground
    @Override
    public void onResume() {
        super.onResume();
        Log.v(TAG, "onResume");

        // Restart GPS
        // Cancel any outstanding GPS
        if (mIsGpsOn) {
            mLocationClient.connect();
        }
    }

    //
    // Other events
    //

    // Adds items to the action bar menu
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu_main, menu);
        return super.onCreateOptionsMenu(menu);
    }

    // Called when pressing action bar items
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_gps:
                // Toggle GPS position updates
                if (mIsGpsOn) {
                    mIsGpsOn = false;
                    item.setIcon(R.drawable.ic_action_location_searching);
                    mLocationClient.disconnect();

                } else {
                    mIsGpsOn = true;
                    item.setIcon(R.drawable.ic_action_location_found);
                    mLocationClient.connect();
                }
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

            default:
                return super.onOptionsItemSelected(item);
        }
    }

    // Handles location updates from GPS
    private void updateLocation(Location location) {
        if (location != null) {
            LatLngZoom coordinate = new LatLngZoom(location.getLatitude(), location.getLongitude(), 16);
            mMapFragment.getMap().setCenterCoordinate(coordinate, true);
        }
    }

    // This class handles style change events
    private class StyleSpinnerListener implements AdapterView.OnItemSelectedListener {

        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            switch(position) {
                // Bright
                case 0:
                    mMapFragment.getMap().setStyleUrl("asset://styles/bright-v7.json");
                    mMapFragment.getMap().removeAllClasses();
                    mClassSpinner.setVisibility(View.INVISIBLE);
                    mClassSpinner.setAdapter(null);
                    mClassSpinner.setOnItemSelectedListener(null);
                    break;

                // Basic
                case 1:
                    mMapFragment.getMap().setStyleUrl("asset://styles/basic-v7.json");
                    mMapFragment.getMap().removeAllClasses();
                    mClassSpinner.setVisibility(View.INVISIBLE);
                    mClassSpinner.setAdapter(null);
                    mClassSpinner.setOnItemSelectedListener(null);
                    break;

                // Outdoors
                case 2:
                    mMapFragment.getMap().setStyleUrl("asset://styles/outdoors-v7.json");
                    mMapFragment.getMap().removeAllClasses();
                    mClassSpinner.setVisibility(View.VISIBLE);
                    mClassSpinner.setAdapter(mOutdoorsClassAdapter);
                    mClassSpinner.setOnItemSelectedListener(new OutdoorClassSpinnerListener());
                    break;

                // Satellite
                case 3:
                    mMapFragment.getMap().setStyleUrl("asset://styles/satellite-v7.json");
                    mMapFragment.getMap().removeAllClasses();
                    mClassSpinner.setVisibility(View.VISIBLE);
                    mClassSpinner.setAdapter(mSatelliteClassAdapter);
                    mClassSpinner.setOnItemSelectedListener(new SatelliteClassSpinnerListener());
                    break;

                // Pencil
                case 4:
                    mMapFragment.getMap().setStyleUrl("asset://styles/pencil-v7.json");
                    mMapFragment.getMap().removeAllClasses();
                    mClassSpinner.setVisibility(View.INVISIBLE);
                    mClassSpinner.setAdapter(null);
                    mClassSpinner.setOnItemSelectedListener(null);
                    break;

                // Empty
                case 5:
                    mMapFragment.getMap().setStyleUrl("asset://styles/empty-v7.json");
                    mMapFragment.getMap().removeAllClasses();
                    mClassSpinner.setVisibility(View.INVISIBLE);
                    mClassSpinner.setAdapter(null);
                    mClassSpinner.setOnItemSelectedListener(null);
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
            ArrayList<String> classes = new ArrayList<String>(1);

            switch(position) {
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
            ArrayList<String> classes = new ArrayList<String>(2);

            switch(position) {
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
    public class MyOnFpsChangedListener implements MapView.OnFpsChangedListener {

        @Override
        public void onFpsChanged(double fps) {
            mFpsTextView.setText(getResources().getString(R.string.label_fps) + String.format(" %4.2f", fps));
        }
    }

    // Called when map state changes
    public class MyOnMapChangedListener implements MapView.OnMapChangedListener {

        @Override
        public void onMapChanged() {
            mCompassView.setRotation((float) mMapFragment.getMap().getDirection());
        }
    }

    // Called when someone presses the compass
    public class CompassOnClickListener implements View.OnClickListener {

        @Override
        public void onClick(View view) {
            mMapFragment.getMap().resetNorth();
        }
    }
}
