package com.mapbox.mapboxgl.app;

import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.ActionBarActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;

import com.mapbox.mapboxgl.lib.MapView;

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

        // TODO move to layout file and save state
        // Add the spinner to select map styles
        Spinner styleSpinner = (Spinner)findViewById(R.id.spinner_style);
        ArrayAdapter styleAdapter = ArrayAdapter.createFromResource(this,
                R.array.style_list, android.R.layout.simple_spinner_dropdown_item);
        styleSpinner.setAdapter(styleAdapter);
        styleSpinner.setOnItemSelectedListener(new StyleSpinnerListener());

        // Add a toolbar as the action bar
        Toolbar mainToolbar = (Toolbar)findViewById(R.id.toolbar_main);
        setSupportActionBar(mainToolbar);
        getSupportActionBar().setDisplayShowTitleEnabled(false);
    }

    // This class handles navigation events
    private class StyleSpinnerListener implements AdapterView.OnItemSelectedListener {

        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            switch(position) {
                // Basic
                case 0:
                    mMapFragment.getMap().setStyleUrl("asset://styles/styles/basic-v6.json");
                    break;

                // Bright
                case 1:
                    mMapFragment.getMap().setStyleUrl("asset://styles/styles/bright-v6.json");
                    break;

                // Outdoors (Vector)
                case 2:
                    mMapFragment.getMap().setStyleUrl("asset://styles/styles/outdoors-v6.json");
                    break;

                // Outdoors (Raster)
                case 3:
                    mMapFragment.getMap().setStyleUrl("asset://styles/styles/outdoors-raster-v6.json");
                    break;

                // Pencil
                case 4:
                    mMapFragment.getMap().setStyleUrl("asset://styles/styles/pencil-v6.json");
                    break;

                // Satellite
                case 5:
                    mMapFragment.getMap().setStyleUrl("asset://styles/styles/satellite-v6.json");
                    break;
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> parent) {
            mMapFragment.getMap().setStyleUrl("");
        }
    }
}
