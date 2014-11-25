package com.mapbox.mapboxgl.app;

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
import android.widget.Spinner;

import com.mapbox.mapboxgl.lib.LonLatZoom;

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
    MapFragment mMapFragment;

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
        mMapFragment = (MapFragment)getSupportFragmentManager().findFragmentById(R.id.fragment_map);

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

    // Adds items to the action bar menu
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu_main, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle presses on the action bar items
        switch (item.getItemId()) {
            case R.id.action_gps:
                // Get a GPS position
                LocationManager locationManager = (LocationManager)getSystemService(getApplicationContext().LOCATION_SERVICE);
                String provider = locationManager.getBestProvider(new Criteria(), true);
                Location location = locationManager.getLastKnownLocation(provider);
                if (location != null) {
                    LonLatZoom coordinate = new LonLatZoom(location.getLongitude(), location.getLatitude(), 15);
                    mMapFragment.getMap().setCenterCoordinate(coordinate, true);
                }
                locationManager.requestSingleUpdate(provider, new MyLocationListener(), null);
                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
    }

    // This class handles location events
    private class MyLocationListener implements LocationListener {

        @Override
        public void onLocationChanged(Location location) {
            LonLatZoom coordinate = new LonLatZoom(location.getLongitude(), location.getLatitude(), 15);
            mMapFragment.getMap().setCenterCoordinate(coordinate, true);
        }

        @Override
        public void onStatusChanged(String provider, int status, Bundle extras) {
            // Do nothing
        }

        @Override
        public void onProviderEnabled(String provider) {
            // Do nothing
        }

        @Override
        public void onProviderDisabled(String provider) {
            // Do nothing
        }
    }

    // This class handles navigation events
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

                // Outdoors (Vector)
                case 2:
                    mMapFragment.getMap().setStyleUrl("asset://styles/styles/outdoors-v6.json");
                    break;

                // Outdoors (Raster)
                case 3:
                    mMapFragment.getMap().setStyleUrl("asset://styles/styles/outdoors-raster-v6.json");
                    break;

                // Satellite
                case 4:
                    mMapFragment.getMap().setStyleUrl("asset://styles/styles/satellite-v6.json");
                    break;

                // Pencil
                case 5:
                    mMapFragment.getMap().setStyleUrl("asset://styles/styles/pencil-v6.json");
                    break;

                // Empty
                case 6:
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
}
