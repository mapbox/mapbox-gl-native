package com.mapbox.mapboxgl.testapp;

import android.graphics.Color;
import android.graphics.PointF;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.NavigationView;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.GestureDetector;
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
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

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
    private DrawerLayout mDrawerLayout;
    private MapView mapView;
    private TextView mFpsTextView;
    private FrameLayout mMapFrameLayout;
    private float mDensity;
    private Spinner mClassSpinner;
    private ArrayAdapter mOutdoorsClassAdapter;
    private ArrayAdapter mSatelliteClassAdapter;

    // Used for GPS
    private FloatingActionButton locationFAB;

    // Used for Annotations
    private boolean mIsAnnotationsOn = false;

    private Marker marker;

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

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        final ActionBar ab = getSupportActionBar();
        ab.setHomeAsUpIndicator(R.drawable.ic_menu);
        ab.setDisplayHomeAsUpEnabled(true);

        mDrawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);

        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        if (navigationView != null) {
            setupDrawerContent(navigationView);
        }

        mapView = (MapView) findViewById(R.id.mainMapView);
        // Load the access token
        try {
            BufferedReader reader = new BufferedReader(new InputStreamReader(getResources().openRawResource(R.raw.token)));
            String line = reader.readLine();
            mapView.setAccessToken(line);
        } catch (IOException e) {
            Log.e(TAG, "Error loading access token from token.txt: " + e.toString());
        }

        mapView.onCreate(savedInstanceState);
        mapView.setOnFpsChangedListener(new MyOnFpsChangedListener());
        mapView.addOnMapChangedListener(new MyOnMapChangedListener());

        changeMapStyle(getString(R.string.styleURLMapboxStreets));
        navigationView.getMenu().findItem(R.id.actionStyleMapboxStreets).setChecked(true);

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

        mFpsTextView = (TextView) findViewById(R.id.view_fps);
        mFpsTextView.setText("");

//        mMapFrameLayout = (FrameLayout) findViewById(R.id.layout_map);

        locationFAB = (FloatingActionButton)findViewById(R.id.locationFAB);
        locationFAB.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Toggle GPS position updates
                toggleGps(!mapView.isMyLocationEnabled());
                updateMap();
//                Snackbar.make(v, "Snack time!", Snackbar.LENGTH_LONG).show();
            }
        });

/*
        // Add the spinner to select class styles
        mClassSpinner = (Spinner) findViewById(R.id.spinner_class);
        mOutdoorsClassAdapter = ArrayAdapter.createFromResource(getSupportActionBar().getThemedContext(),
                R.array.outdoors_class_list, android.R.layout.simple_spinner_dropdown_item);
*/

        // Set default UI state
        navigationView.getMenu().findItem(R.id.action_compass).setChecked(mapView.isCompassEnabled());
        navigationView.getMenu().findItem(R.id.action_debug).setChecked(mapView.isDebugActive());
        navigationView.getMenu().findItem(R.id.action_markers).setChecked(mIsAnnotationsOn);

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
        mapView.onStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
        mapView.onStop();
    }

    // Called when our app goes into the background
    @Override
    public void onPause()  {
        super.onPause();

        mapView.onPause();

        // Cancel GPS
        toggleGps(false);
    }

    // Called when our app comes into the foreground
    @Override
    public void onResume() {
        super.onResume();

        mapView.onResume();

        // Restart GPS
        toggleGps(true);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mapView.onDestroy();
    }

    // Called when we need to save instance state
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);

        mapView.onSaveInstanceState(outState);
        outState.putBoolean(STATE_IS_GPS_ON, mapView.isMyLocationEnabled());
    }

    // Called when the system is running low on memory
    @Override
    public void onLowMemory() {
        super.onLowMemory();

        mapView.onLowMemory();
    }

    //
    // Other events
    //

    // Called when pressing action bar items
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                mDrawerLayout.openDrawer(GravityCompat.START);
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private void setupDrawerContent(NavigationView navigationView) {
        navigationView.setNavigationItemSelectedListener(
                new NavigationView.OnNavigationItemSelectedListener() {
                    @Override
                    public boolean onNavigationItemSelected(MenuItem menuItem) {
                        mDrawerLayout.closeDrawers();

                        // Respond To Selection
                        switch (menuItem.getItemId()) {

                            case R.id.action_debug:
                                // Toggle debug mode
                                mapView.toggleDebug();

                                menuItem.setChecked(mapView.isDebugActive());

                                // Show the FPS counter
                                if (mapView.isDebugActive()) {
                                    mFpsTextView.setVisibility(View.VISIBLE);
                                    mFpsTextView.setText(getResources().getString(R.string.label_fps));
                                } else {
                                    mFpsTextView.setVisibility(View.INVISIBLE);
                                }
                                break;

                            case R.id.action_markers:
                                // Toggle markers
                                toggleAnnotations(!mIsAnnotationsOn);
                                menuItem.setChecked(mIsAnnotationsOn);
                                break;

                            case R.id.action_compass:
                                // Toggle compass
                                mapView.setCompassEnabled(!mapView.isCompassEnabled());
                                menuItem.setChecked(mapView.isCompassEnabled());
                                break;

/*
                            case R.id.followNone:
                                mapView.setUserLocationTrackingMode(MapView.UserLocationTrackingMode.NONE);
                                break;

                            case R.id.followFollow:
                                mapView.setUserLocationTrackingMode(MapView.UserLocationTrackingMode.FOLLOW);
                                break;

                            case R.id.followBearing:
                                mapView.setUserLocationTrackingMode(MapView.UserLocationTrackingMode.FOLLOW_BEARING);
                                break;
*/

                            case R.id.actionStyleMapboxStreets:
                                changeMapStyle(getString(R.string.styleURLMapboxStreets));
                                menuItem.setChecked(true);
                                break;

                            case R.id.actionStyleEmerald:
                                changeMapStyle(getString(R.string.styleURLEmerald));
                                menuItem.setChecked(true);
                                break;

                            case R.id.actionStyleLight:
                                changeMapStyle(getString(R.string.styleURLLight));
                                menuItem.setChecked(true);
                                break;

                            case R.id.actionStyleDark:
                                changeMapStyle(getString(R.string.styleURLDark));
                                menuItem.setChecked(true);
                                break;

                            case R.id.actionStyleSatellite:
                                changeMapStyle(getString(R.string.styleURLSatellite));
                                menuItem.setChecked(true);
                                break;
                        }

                        return true;
                    }
                });
    }

    private void changeMapStyle(@NonNull String styleURL) {
        mapView.setStyleUrl(styleURL);
    }

    /**
     * Enabled / Disable GPS location updates along with updating the UI
     * @param enableGps true if GPS is to be enabled, false if GPS is to be disabled
     */
    private void toggleGps(boolean enableGps) {

        if (enableGps) {
            if (!mapView.isMyLocationEnabled()) {
                mapView.setMyLocationEnabled(enableGps);
                Location location = mapView.getMyLocation();
                if (location != null) {
                    mapView.setZoomLevel(8);
                    mapView.setCenterCoordinate(new LatLng(location));
                }
                locationFAB.setColorFilter(getResources().getColor(R.color.primary));
            }
        } else {
            if (mapView.isMyLocationEnabled()) {
                mapView.setMyLocationEnabled(enableGps);
                locationFAB.setColorFilter(Color.TRANSPARENT);
            }
        }
    }

    /**
     * Enable / Disable Annotations.
     *
     * @param enableAnnotations True to display, False to hide
     */
    private void toggleAnnotations(boolean enableAnnotations) {
        if (enableAnnotations) {
            if (!mIsAnnotationsOn) {
                mIsAnnotationsOn = true;
                addMarkers();
                addPolyline();
                addPolygon();
                mapView.setZoomLevel(7);
                mapView.setCenterCoordinate(new LatLng(38.11727, -122.22839));
            }
        } else {
            if (mIsAnnotationsOn) {
                mIsAnnotationsOn = false;
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
        } catch (Exception e) {
            Log.e(TAG, "Error adding Polyline: "+ e);
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
        } catch (Exception e) {
            Log.e(TAG, "Error adding Polygon: "+ e);
            e.printStackTrace();
        }
    }

    private void removeAnnotations() {
        mapView.removeAnnotations();
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
