package com.mapbox.mapboxgl.testapp;

import android.graphics.Color;
import android.graphics.PointF;
import android.location.Location;
import android.os.Bundle;
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
import android.widget.TextView;
import com.crashlytics.android.Crashlytics;
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
    private static final String STATE_IS_ANNOTATIONS_ON = "isAnnotationsOn";
    private static final String STATE_SELECTED_STYLE = "selectedStyle";

    //
    // Instance members
    //

    // Used for the UI
    private DrawerLayout mDrawerLayout;
    private MapView mMapView;
    private TextView mFpsTextView;
    private int mSelectedStyle = R.id.actionStyleMapboxStreets;
    NavigationView mNavigationView;

    // Used for GPS
    private FloatingActionButton mLocationFAB;

    // Used for Annotations
    private boolean mIsAnnotationsOn = false;

    //
    // Lifecycle events
    //

    // Called when activity is created
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Fabric.with(this, new Crashlytics());

        // Load the layout
        setContentView(R.layout.activity_main);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        final ActionBar ab = getSupportActionBar();
        ab.setHomeAsUpIndicator(R.drawable.ic_menu);
        ab.setDisplayHomeAsUpEnabled(true);

        mDrawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);

        mNavigationView = (NavigationView) findViewById(R.id.nav_view);
        if (mNavigationView != null) {
            setupDrawerContent(mNavigationView);
        }

        mMapView = (MapView) findViewById(R.id.mainMapView);
        // Load the access token
        try {
            BufferedReader reader = new BufferedReader(new InputStreamReader(getResources().openRawResource(R.raw.token)));
            String line = reader.readLine();
            mMapView.setAccessToken(line);
        } catch (IOException e) {
            Log.e(TAG, "Error loading access token from token.txt: " + e.toString());
        }

        mMapView.onCreate(savedInstanceState);
        mMapView.setOnFpsChangedListener(new MyOnFpsChangedListener());

        final GestureDetector gestureDetector = new GestureDetector(this, new GestureDetector.SimpleOnGestureListener() {
            public void onLongPress(final MotionEvent e) {
                float x = e.getX();
                float y = e.getY();

                // flip y direction vertically to match core GL
                y = mMapView.getHeight() - y;

                LatLng position = mMapView.fromScreenLocation(new PointF(x, y));

                mMapView.addMarker(new MarkerOptions()
                        .position(position)
                        .title("Dropped Pin")
                        .sprite("default_marker"));
            }
        });

        mMapView.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View v, MotionEvent event) {
                return gestureDetector.onTouchEvent(event);
            }
        });

        mFpsTextView = (TextView) findViewById(R.id.view_fps);
        mFpsTextView.setText("");

        mLocationFAB = (FloatingActionButton)findViewById(R.id.locationFAB);
        mLocationFAB.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Toggle GPS position updates
                toggleGps(!mMapView.isMyLocationEnabled());
                if (mMapView.isMyLocationEnabled()) {
                    Location location = mMapView.getMyLocation();
                    if (location != null) {
                        mMapView.setZoomLevel(8);
                        mMapView.setCenterCoordinate(new LatLng(location));
                    }
                }
            }
        });

        // Restore saved state
        if (savedInstanceState != null) {
            mIsAnnotationsOn = savedInstanceState.getBoolean(STATE_IS_ANNOTATIONS_ON);
            mSelectedStyle = savedInstanceState.getInt(STATE_SELECTED_STYLE);
        }

        // Set default UI state
        mNavigationView.getMenu().findItem(R.id.action_compass).setChecked(mMapView.isCompassEnabled());
        mNavigationView.getMenu().findItem(R.id.action_debug).setChecked(mMapView.isDebugActive());
        mNavigationView.getMenu().findItem(R.id.action_markers).setChecked(mIsAnnotationsOn);
        changeMapStyle(mSelectedStyle);
        toggleGps(mMapView.isMyLocationEnabled());
    }

    /**
     * Dispatch onStart() to all fragments.  Ensure any created loaders are
     * now started.
     */
    @Override
    protected void onStart() {
        super.onStart();
        mMapView.onStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
        mMapView.onStop();
    }

    // Called when our app goes into the background
    @Override
    public void onPause()  {
        super.onPause();

        mMapView.onPause();
    }

    // Called when our app comes into the foreground
    @Override
    public void onResume() {
        super.onResume();

        mMapView.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMapView.onDestroy();
    }

    // Called when we need to save instance state
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);

        mMapView.onSaveInstanceState(outState);
        outState.putBoolean(STATE_IS_ANNOTATIONS_ON, mIsAnnotationsOn);
        outState.putInt(STATE_SELECTED_STYLE, mSelectedStyle);
    }

    // Called when the system is running low on memory
    @Override
    public void onLowMemory() {
        super.onLowMemory();

        mMapView.onLowMemory();
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
                                mMapView.toggleDebug();
                                toggleFpsCounter(mMapView.isDebugActive());
                                menuItem.setChecked(mMapView.isDebugActive());
                                return true;

                            case R.id.action_markers:
                                // Toggle markers
                                toggleAnnotations(!mIsAnnotationsOn);
                                menuItem.setChecked(mIsAnnotationsOn);
                                return true;

                            case R.id.action_compass:
                                // Toggle compass
                                mMapView.setCompassEnabled(!mMapView.isCompassEnabled());
                                menuItem.setChecked(mMapView.isCompassEnabled());
                                return true;

/*
                            case R.id.followNone:
                                mMapView.setUserLocationTrackingMode(MapView.UserLocationTrackingMode.NONE);
                                return true;

                            case R.id.followFollow:
                                mMapView.setUserLocationTrackingMode(MapView.UserLocationTrackingMode.FOLLOW);
                                return true;

                            case R.id.followBearing:
                                mMapView.setUserLocationTrackingMode(MapView.UserLocationTrackingMode.FOLLOW_BEARING);
                                return true;
*/
                            default:
                                return changeMapStyle(menuItem.getItemId());
                        }
                    }
                });
    }

    private void toggleFpsCounter(boolean enableFps) {
        // Show the FPS counter
        if (enableFps) {
            mFpsTextView.setVisibility(View.VISIBLE);
            mFpsTextView.setText(getResources().getString(R.string.label_fps));
        } else {
            mFpsTextView.setVisibility(View.INVISIBLE);
        }
    }

    private boolean changeMapStyle(int id) {
        switch (id) {
            case R.id.actionStyleMapboxStreets:
                mMapView.setStyleUrl(getString(R.string.styleURLMapboxStreets));
                mNavigationView.getMenu().findItem(id).setChecked(true);
                mSelectedStyle = id;
                return true;

            case R.id.actionStyleEmerald:
                mMapView.setStyleUrl(getString(R.string.styleURLEmerald));
                mNavigationView.getMenu().findItem(id).setChecked(true);
                mSelectedStyle = id;
                return true;

            case R.id.actionStyleLight:
                mMapView.setStyleUrl(getString(R.string.styleURLLight));
                mNavigationView.getMenu().findItem(id).setChecked(true);
                mSelectedStyle = id;
                return true;

            case R.id.actionStyleDark:
                mMapView.setStyleUrl(getString(R.string.styleURLDark));
                mNavigationView.getMenu().findItem(id).setChecked(true);
                mSelectedStyle = id;
                return true;

            case R.id.actionStyleSatellite:
                mMapView.setStyleUrl(getString(R.string.styleURLSatellite));
                mNavigationView.getMenu().findItem(id).setChecked(true);
                mSelectedStyle = id;
                return true;

            default:
                return false;
        }
    }

    /**
     * Enabled / Disable GPS location updates along with updating the UI
     * @param enableGps true if GPS is to be enabled, false if GPS is to be disabled
     */
    private void toggleGps(boolean enableGps) {
        if (enableGps) {
            mMapView.setMyLocationEnabled(enableGps);
            mLocationFAB.setColorFilter(getResources().getColor(R.color.primary));
        } else {
            mMapView.setMyLocationEnabled(enableGps);
            mLocationFAB.setColorFilter(Color.TRANSPARENT);
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
                mMapView.setZoomLevel(7);
                mMapView.setCenterCoordinate(new LatLng(38.11727, -122.22839));
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
        MapView map = mMapView;
        mMapView.addMarker(new MarkerOptions()
            .position(backLot)
            .title("Back Lot")
            .snippet("The back lot behind my house"));

        LatLng cheeseRoom = new LatLng(38.531577,-122.010646);
        mMapView.addMarker(new MarkerOptions()
            .position(cheeseRoom)
            .sprite("dog-park-15")
            .title("Cheese Room")
            .snippet("The only air conditioned room on the property!"));
    }

    private void addPolyline() {
        try {
            String geojsonStr = Util.loadStringFromAssets(this, "small_line.geojson");
            LatLng[] latLngs = Util.parseGeoJSONCoordinates(geojsonStr);
            MapView map = mMapView;
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
            MapView map = mMapView;
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
        mMapView.removeAnnotations();
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
                    mMapView.setStyleClasses(styleClasses);
                    break;

                // Night
                case 1:
                    styleClasses.add("night");
                    mMapView.setStyleClasses(styleClasses);
                    break;

                default:
                    onNothingSelected(parent);
                    break;
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> parent) {
            mMapView.removeAllStyleClasses();
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
                    mMapView.setStyleClasses(styleClasses);
                    break;

                // Labels Only
                case 1:
                    styleClasses.add("labels");
                    mMapView.setStyleClasses(styleClasses);
                    break;

                // No Labels
                case 2:
                    mMapView.setStyleClasses(styleClasses);
                    break;

                default:
                    onNothingSelected(parent);
                    break;
            }
        }

        @Override
        public void onNothingSelected(AdapterView<?> parent) {
            mMapView.removeAllStyleClasses();
        }
    }

    // Called when FPS changes
    private class MyOnFpsChangedListener implements MapView.OnFpsChangedListener {

        @Override
        public void onFpsChanged(double fps) {
            mFpsTextView.setText(getResources().getString(R.string.label_fps) + String.format(" %4.2f", fps));
        }
    }

}
