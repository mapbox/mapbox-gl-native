package com.mapbox.mapboxgl.app;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
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

import com.mapbox.mapboxgl.lib.LonLatZoom;
import com.mapbox.mapboxgl.lib.MapView;

public class MainActivity extends ActionBarActivity {

    //
    // Static members
    //

    // Tag used for logging
    private static final String TAG = "MainActivity";

    // Used for GPS
    private static final String SINGLE_LOCATION_UPDATE_ACTION = "com.mapbox.mapboxgl.app.SINGLE_LOCATION_UPDATE_ACTION";

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
    private LocationManager mLocationManager;
    private PendingIntent mLocationPendingIntent;
    private LocationReceiver mLocationReceiver;

    //
    // Lifecycle events
    //

    // Called when activity is created
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.v(TAG, "onCreate");

        Intent locationIntent = new Intent(SINGLE_LOCATION_UPDATE_ACTION);
        mLocationManager = (LocationManager)getSystemService(getApplicationContext().LOCATION_SERVICE);
        mLocationPendingIntent = PendingIntent.getBroadcast(getApplicationContext(), 0, locationIntent, PendingIntent.FLAG_UPDATE_CURRENT);

        // Load the layout
        setContentView(R.layout.activity_main);
        mMapFragment = (MapFragment)getSupportFragmentManager().findFragmentById(R.id.fragment_map);
        mMapFragment.getMap().setOnFpsChangedListener(new MyOnFpsChangedListener());
        mMapFragment.getMap().setOnMapChangedListener(new MyOnMapChangedListener());

        mFpsTextView = (TextView)findViewById(R.id.view_fps);
        mFpsTextView.setText("");

        mCompassView = (ImageView)findViewById(R.id.view_compass);
        mCompassView.setOnClickListener(new CompassOnClickListener());

        // Add a toolbar as the action bar
        Toolbar mainToolbar = (Toolbar)findViewById(R.id.toolbar_main);
        setSupportActionBar(mainToolbar);
        getSupportActionBar().setDisplayShowTitleEnabled(false);

        // TODO move to layout file and save state
        // Add the spinner to select map styles
        Spinner styleSpinner = (Spinner)findViewById(R.id.spinner_style);
        ArrayAdapter styleAdapter = ArrayAdapter.createFromResource(getSupportActionBar().getThemedContext(),
                R.array.style_list, android.R.layout.simple_spinner_dropdown_item);
        styleSpinner.setAdapter(styleAdapter);
        styleSpinner.setOnItemSelectedListener(new StyleSpinnerListener());
    }

    // Called when our app goes into the background
    @Override
    public void onPause()  {
        super.onPause();
        Log.v(TAG, "onPause");

        // Cancel any outstanding GPS
        if (mLocationReceiver != null) {
            getApplicationContext().unregisterReceiver(mLocationReceiver);
            mLocationReceiver = null;
        }

        mLocationManager.removeUpdates(mLocationPendingIntent);
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
                // Get a GPS position
                Criteria criteria = new Criteria();
                criteria.setAccuracy(Criteria.ACCURACY_LOW);
                String provider = mLocationManager.getBestProvider(criteria, true);
                Location location = mLocationManager.getLastKnownLocation(provider);
                if (location != null) {
                    LonLatZoom coordinate = new LonLatZoom(location.getLongitude(), location.getLatitude(), 15);
                    mMapFragment.getMap().setCenterCoordinate(coordinate, true);
                }
                IntentFilter locationIntentFilter = new IntentFilter(SINGLE_LOCATION_UPDATE_ACTION);
                mLocationReceiver = new LocationReceiver();
                getApplicationContext().registerReceiver(mLocationReceiver, locationIntentFilter);
                mLocationManager.requestSingleUpdate(provider, mLocationPendingIntent);
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

    // This class handles location events
    private class LocationReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            context.unregisterReceiver(this);

            Location location = (Location)intent.getExtras().get(LocationManager.KEY_LOCATION_CHANGED);
            if (location != null) {
                LonLatZoom coordinate = new LonLatZoom(location.getLongitude(), location.getLatitude(), 15);
                mMapFragment.getMap().setCenterCoordinate(coordinate, true);
            }

            mLocationManager.removeUpdates(mLocationPendingIntent);
            mLocationReceiver = null;
        }
    }

    // This class handles style change events
    private class StyleSpinnerListener implements AdapterView.OnItemSelectedListener {

        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            switch(position) {
                // Bright
                case 0:
                    mMapFragment.getMap().setStyleUrl("asset://styles/styles/bright-v6.json");
                    break;

                // Basic
                case 1:
                    mMapFragment.getMap().setStyleUrl("asset://styles/styles/basic-v6.json");
                    break;

                // Outdoors
                case 2:
                    mMapFragment.getMap().setStyleUrl("asset://styles/styles/outdoors-v6.json");
                    break;

                // Satellite
                case 3:
                    mMapFragment.getMap().setStyleUrl("asset://styles/styles/satellite-v6.json");
                    break;

                // Pencil
                case 4:
                    mMapFragment.getMap().setStyleUrl("asset://styles/styles/pencil-v6.json");
                    break;

                // Empty
                case 5:
                    mMapFragment.getMap().setStyleUrl("asset://styles/styles/empty-v6.json");
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
            mCompassView.setRotation((float)mMapFragment.getMap().getDirection());
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
