package com.mapbox.mapboxgl.testapp;

import android.graphics.Color;
import android.graphics.PointF;
import android.os.Bundle;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.ActionBarActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.GestureDetector;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.widget.Spinner;
import android.widget.TextView;
import com.crashlytics.android.Crashlytics;
import com.mapbox.mapboxgl.annotations.Marker;
import com.mapbox.mapboxgl.annotations.MarkerOptions;
import com.mapbox.mapboxgl.annotations.Polygon;
import com.mapbox.mapboxgl.annotations.PolygonOptions;
import com.mapbox.mapboxgl.annotations.Polyline;
import com.mapbox.mapboxgl.annotations.PolylineOptions;
import com.mapbox.mapboxgl.geometry.LatLng;
import com.mapbox.mapboxgl.views.MapView;
import io.fabric.sdk.android.Fabric;
import org.json.JSONException;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;

public class MainActivity extends ActionBarActivity implements MapFragment.OnMapReadyCallback {

    private static final String TAG = "MainActivity";

    //
    // Static members
    //

    // Used for saving instance state
    private static final String STATE_IS_GPS_ON = "isGpsOn";

    //
    // Instance members
    //

    // Used for the UI
    private MapView mapView;
    private TextView mFpsTextView;
    private FrameLayout mMapFrameLayout;
    private float mDensity;
    private Spinner mClassSpinner;
    private ArrayAdapter mOutdoorsClassAdapter;
    private ArrayAdapter mSatelliteClassAdapter;

    // Used for GPS
    private MenuItem mGpsMenuItem;

    // Used for markers
    private boolean mIsMarkersOn = false;

    private Marker marker;
    private static MapFragment mMapFragment;
    private Bundle savedInstanceState;

    //
    // Lifecycle events
    //

    // Called when activity is created
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Fabric.with(this, new Crashlytics());

        mDensity = getResources().getDisplayMetrics().density;

        // Load the layout
        setContentView(R.layout.activity_main);

        if(mMapFragment == null){
            mMapFragment = new MapFragment();
        }

        this.savedInstanceState = savedInstanceState;
        mMapFragment.getMapAsync(this);

        FragmentManager fragmentManager = getSupportFragmentManager();
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        fragmentTransaction.replace(R.id.container, mMapFragment);
        fragmentTransaction.commit();


        mFpsTextView = (TextView) findViewById(R.id.view_fps);
        mFpsTextView.setText("");

        mMapFrameLayout = (FrameLayout) findViewById(R.id.layout_map);
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

    }

    @Override
    public void onMapReady(MapView mapView) {
        this.mapView = mapView;

        initMapView(savedInstanceState);
    }


    private void initMapView(Bundle savedInstanceState) {

        mapView.onCreate(savedInstanceState);
        mapView.setOnFpsChangedListener(new MyOnFpsChangedListener());
        mapView.addOnMapChangedListener(new MyOnMapChangedListener());

        final GestureDetector gestureDetector = new GestureDetector(this, new GestureDetector.SimpleOnGestureListener() {
            public void onLongPress(final MotionEvent e) {
                float x = e.getX();
                float y = e.getY();

                // flip y direction vertically to match core GL
                y = mapView.getHeight() - y;

                LatLng position = mapView.fromScreenLocation(new PointF(x, y));

                mapView.addMarker(new MarkerOptions()
                        .position(position)
                        .title("Dropped Pin")
                        .sprite("default_marker"));
            }
        });

        mapView.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View v, MotionEvent event) {
                return gestureDetector.onTouchEvent(event);
            }
        });

        if (savedInstanceState != null) {
            mapView.setMyLocationEnabled(savedInstanceState.getBoolean(STATE_IS_GPS_ON, false));
        }
    }


    /**
     * Dispatch onStart() to all fragments.  Ensure any created loaders are
     * now started.
     */

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
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

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    // Called when we need to save instance state
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);

        //mapView.onSaveInstanceState(outState);
        //outState.putBoolean(STATE_IS_GPS_ON, mapView.isMyLocationEnabled());
    }

    // Called when the system is running low on memory
    @Override
    public void onLowMemory() {
        super.onLowMemory();

        //mapView.onLowMemory();
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
        if (mapView.isMyLocationEnabled()) {
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
                toggleGps(!mapView.isMyLocationEnabled());
                updateMap();
                return true;

            case R.id.action_debug:
                // Toggle debug mode
                mapView.toggleDebug();

                // Show the FPS counter
                if (mapView.isDebugActive()) {
                    mFpsTextView.setVisibility(View.VISIBLE);
                    mFpsTextView.setText(getResources().getString(R.string.label_fps));
                } else {
                    mFpsTextView.setVisibility(View.INVISIBLE);
                }
                return true;

            case R.id.action_markers:
                // Toggle markers
                toggleMarkers(!mIsMarkersOn);
                return true;

            case R.id.action_compass:
                // Toggle compass
                mapView.setCompassEnabled(!mapView.isCompassEnabled());
                return true;

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
            if (!mapView.isMyLocationEnabled()) {
                mapView.setMyLocationEnabled(enableGps);
                if (mGpsMenuItem != null) {
                    mGpsMenuItem.setIcon(R.drawable.ic_action_location_found);
                }
            }
        } else {
            if (mapView.isMyLocationEnabled()) {
                mapView.setMyLocationEnabled(enableGps);
                if (mGpsMenuItem != null) {
                    mGpsMenuItem.setIcon(R.drawable.ic_action_location_searching);
                }
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
                addPolyline();
                addPolygon();
            }
        } else {
            if (mIsMarkersOn) {
                mIsMarkersOn = false;
                removeAnnotations();
            }
        }
    }

    private void addMarkers() {
        LatLng backLot = new LatLng(38.649441, -121.369064);
        MapView map = mapView;
        marker = map.addMarker(new MarkerOptions()
            .position(backLot)
            .title("Back Lot")
            .snippet("The back lot behind my house"));

        LatLng cheeseRoom = new LatLng(38.531577,-122.010646);
        map.addMarker(new MarkerOptions()
                .position(cheeseRoom)
                .sprite("dog-park-15")
                .title("Cheese Room")
                .snippet("The only air conditioned room on the property!"));
    }

    private void addPolyline() {
        try {
            String geojsonStr = Util.loadStringFromAssets(this, "small_line.geojson");
            LatLng[] latLngs = Util.parseGeoJSONCoordinates(geojsonStr);
            MapView map = mapView;
            Polyline line = map.addPolyline(new PolylineOptions()
                    .add(latLngs)
                    .width(2)
                    .color(Color.RED));
        } catch (IOException e) {
            e.printStackTrace();
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private void addPolygon() {
        String geojsonStr = null;
        try {
            geojsonStr = Util.loadStringFromAssets(this, "small_poly.geojson");
            LatLng[] latLngs = Util.parseGeoJSONCoordinates(geojsonStr);
            MapView map = mapView;
            ArrayList<PolygonOptions> opts = new ArrayList<PolygonOptions>();
            opts.add(new PolygonOptions()
                        .add(latLngs)
                        .strokeColor(Color.MAGENTA)
                        .fillColor(Color.BLUE));
            Polygon polygon = map.addPolygons(opts).get(0);
        } catch (IOException e) {
            e.printStackTrace();
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    private void removeAnnotations() {
        mapView.removeAnnotations();
    }

    // This class handles style change events
    private class StyleSpinnerListener implements AdapterView.OnItemSelectedListener {

        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            switch (position) {

                // Mapbox Streets
                case 0:
                    mapView.setStyleUrl("asset://styles/mapbox-streets-v7.json");
                    mapView.removeAllStyleClasses();
                    mClassSpinner.setVisibility(View.INVISIBLE);
                    mClassSpinner.setAdapter(null);
                    mClassSpinner.setOnItemSelectedListener(null);
                    break;

                // Emerald
                case 1:
                    mapView.setStyleUrl("asset://styles/emerald-v7.json");
                    mapView.removeAllStyleClasses();
                    mClassSpinner.setVisibility(View.INVISIBLE);
                    mClassSpinner.setAdapter(null);
                    mClassSpinner.setOnItemSelectedListener(null);
                    break;

                // Light
                case 2:
                    mapView.setStyleUrl("asset://styles/light-v7.json");
                    mapView.removeAllStyleClasses();
                    mClassSpinner.setVisibility(View.INVISIBLE);
                    mClassSpinner.setAdapter(null);
                    mClassSpinner.setOnItemSelectedListener(null);
                    break;

                // Dark
                case 3:
                    mapView.setStyleUrl("asset://styles/dark-v7.json");
                    mapView.removeAllStyleClasses();
                    mClassSpinner.setVisibility(View.INVISIBLE);
                    mClassSpinner.setAdapter(null);
                    mClassSpinner.setOnItemSelectedListener(null);
                    break;

                // Outdoors
                case 4:
                    mapView.setStyleUrl("asset://styles/outdoors-v7.json");
                    mapView.removeAllStyleClasses();
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
            mapView.setStyleUrl("");
        }
    }

    // This class handles outdoor class change events
    private class OutdoorClassSpinnerListener implements AdapterView.OnItemSelectedListener {

        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            ArrayList<String> styleClasses = new ArrayList<>(1);

            switch (position) {
                // Day
                case 0:
                    styleClasses.add("day");
                    mapView.setStyleClasses(styleClasses);
                    break;

                // Night
                case 1:
                    styleClasses.add("night");
                    mapView.setStyleClasses(styleClasses);
                    break;

                default:
                    onNothingSelected(parent);
                    break;
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> parent) {
            mapView.removeAllStyleClasses();
        }
    }

    // This class handles satellite class change events
    private class SatelliteClassSpinnerListener implements AdapterView.OnItemSelectedListener {

        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            ArrayList<String> styleClasses = new ArrayList<>(2);

            switch (position) {
                // Labels + Contours
                case 0:
                    styleClasses.add("labels");
                    styleClasses.add("contours");
                    mapView.setStyleClasses(styleClasses);
                    break;

                // Labels Only
                case 1:
                    styleClasses.add("labels");
                    mapView.setStyleClasses(styleClasses);
                    break;

                // No Labels
                case 2:
                    mapView.setStyleClasses(styleClasses);
                    break;

                default:
                    onNothingSelected(parent);
                    break;
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> parent) {
            mapView.removeAllStyleClasses();
        }
    }

    // Called when FPS changes
    private class MyOnFpsChangedListener implements MapView.OnFpsChangedListener {

        @Override
        public void onFpsChanged(double fps) {
            mFpsTextView.setText(getResources().getString(R.string.label_fps) + String.format(" %4.2f", fps));
        }
    }


    // Updates the UI to match the current map's position
    private void updateMap() {
//        rotateImageView(mCompassView, (float) mapView.getDirection());

/*
        if (mGpsLocation != null) {
            mGpsMarker.setVisibility(View.VISIBLE);
            LatLng coordinate = new LatLng(mGpsLocation);
            PointF screenLocation = mapView.toScreenLocation(coordinate);

            if (mGpsLocation.hasBearing() || mCompassValid) {
                mGpsMarker.setImageResource(R.drawable.direction_arrow);
                float iconSize = 54.0f * mDensity;
                FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams((int) iconSize, (int) iconSize);
                lp.leftMargin = (int) (screenLocation.x - iconSize / 2.0f);
                lp.topMargin = mMapFrameLayout.getHeight() - (int) (screenLocation.y + iconSize / 2.0f);
                mGpsMarker.setLayoutParams(lp);
                float bearing = mGpsLocation.hasBearing() ? mGpsLocation.getBearing() : mCompassBearing;
                rotateImageView(mGpsMarker, bearing);
                mGpsMarker.requestLayout();
            } else {
                mGpsMarker.setImageResource(R.drawable.location_marker);
                float iconSize = 27.0f * mDensity;
                FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams((int) iconSize, (int) iconSize);
                lp.leftMargin = (int) (screenLocation.x - iconSize / 2.0f);
                lp.topMargin = mMapFrameLayout.getHeight() - (int) (screenLocation.y + iconSize / 2.0f);
                mGpsMarker.setLayoutParams(lp);
                rotateImageView(mGpsMarker, 0.0f);
                mGpsMarker.requestLayout();
            }
        } else {
            mGpsMarker.setVisibility(View.INVISIBLE);
        }
*/
    }

    // Called when map state changes
    private class MyOnMapChangedListener implements MapView.OnMapChangedListener {

        @Override
        public void onMapChanged() {
            updateMap();
        }
    }

}
