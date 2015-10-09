package com.mapbox.mapboxsdk.testapp;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.NavigationView;
import android.support.design.widget.Snackbar;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;
import com.crashlytics.android.Crashlytics;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.annotations.PolygonOptions;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;
import io.fabric.sdk.android.Fabric;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    //
    // Static members
    //

    // Used for saving instance state
    private static final String STATE_IS_ANNOTATIONS_ON = "isAnnotationsOn";
    private static final String STATE_SELECTED_STYLE = "selectedStyle";

    // Used for permissions requests
    private static final int PERMISSIONS_LOCATION = 0;

    //
    // Instance members
    //

    // Used for the UI
    private DrawerLayout mDrawerLayout;
    private MapView mMapView;
    private TextView mFpsTextView;
    private int mSelectedStyle = R.id.actionStyleMapboxStreets;
    private NavigationView mNavigationView;

    // Used for GPS
    private FloatingActionButton mLocationFAB;

    // Used for Annotations
    private boolean mIsAnnotationsOn = false;


    private static final DecimalFormat latLngFormatter = new DecimalFormat("#.#####");

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
        if (ab != null) {
            ab.setHomeAsUpIndicator(R.drawable.ic_menu);
            ab.setDisplayHomeAsUpEnabled(true);
        }

        mDrawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);

        mNavigationView = (NavigationView) findViewById(R.id.nav_view);
        if (mNavigationView != null) {
            setupDrawerContent(mNavigationView);
        }

        mMapView = (MapView) findViewById(R.id.mainMapView);
        mMapView.setAccessToken(ApiAccess.getToken(this));
        mMapView.onCreate(savedInstanceState);

        mMapView.setOnFpsChangedListener(new MyOnFpsChangedListener());

        mMapView.setOnMapLongClickListener(new MapView.OnMapLongClickListener() {
            @Override
            public void onMapLongClick(LatLng point) {
                mMapView.addMarker(new MarkerOptions()
                        .position(point)
                        .title("Dropped Pin")
                        .snippet(latLngFormatter.format(point.getLatitude()) + ", " +
                                latLngFormatter.format(point.getLongitude()))
                        .sprite("default_marker"));
            }
        });

        mMapView.setOnMapClickListener(new MapView.OnMapClickListener() {
            @Override
            public void onMapClick(LatLng point) {
                String location = latLngFormatter.format(point.getLatitude()) + ", " +
                        latLngFormatter.format(point.getLongitude());
                Snackbar.make(findViewById(android.R.id.content), "Map Click Listener " + location, Snackbar.LENGTH_SHORT).show();
            }
        });

        mMapView.setOnMarkerClickListener(new MapView.OnMarkerClickListener() {
            @Override
            public boolean onMarkerClick(Marker marker) {
                Snackbar.make(findViewById(android.R.id.content), "Custom Marker Click Listener", Snackbar.LENGTH_SHORT).show();
                return false;
            }
        });

        mFpsTextView = (TextView) findViewById(R.id.view_fps);
        mFpsTextView.setText("");

        mLocationFAB = (FloatingActionButton) findViewById(R.id.locationFAB);
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
    public void onPause() {
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

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String permissions[], @NonNull int[] grantResults) {
        switch (requestCode) {
            case PERMISSIONS_LOCATION: {
                if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    enableGps();
                }
            }
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
                                return true;

                            case R.id.action_markers:
                                // Toggle markers
                                toggleAnnotations(!mIsAnnotationsOn);
                                return true;

                            case R.id.action_compass:
                                // Toggle compass
                                mMapView.setCompassEnabled(!mMapView.isCompassEnabled());
                                return true;

                            case R.id.action_info_window_adapter:
                                startActivity(new Intent(getApplicationContext(), InfoWindowAdapterActivity.class));
                                return true;

                            case R.id.action_map_fragment:
                                startActivity(new Intent(getApplicationContext(), MapFragmentActivity.class));
                                return true;

                            case R.id.action_press_for_marker:
                                startActivity(new Intent(getApplicationContext(), PressForMarkerActivity.class));
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
                mMapView.setStyleUrl(MapView.StyleUrls.MAPBOX_STREETS);
                mSelectedStyle = id;
                return true;

            case R.id.actionStyleEmerald:
                mMapView.setStyleUrl(MapView.StyleUrls.EMERALD);
                mSelectedStyle = id;
                return true;

            case R.id.actionStyleLight:
                mMapView.setStyleUrl(MapView.StyleUrls.LIGHT);
                mSelectedStyle = id;
                return true;

            case R.id.actionStyleDark:
                mMapView.setStyleUrl(MapView.StyleUrls.DARK);
                mSelectedStyle = id;
                return true;

            case R.id.actionStyleSatellite:
                mMapView.setStyleUrl(MapView.StyleUrls.SATELLITE);
                mSelectedStyle = id;
                return true;

            default:
                return false;
        }
    }

    /**
     * Enabled / Disable GPS location updates along with updating the UI
     *
     * @param enableGps true if GPS is to be enabled, false if GPS is to be disabled
     */
    private void toggleGps(boolean enableGps) {
        if (enableGps) {
            if ((ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) ||
                    (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED)) {
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_COARSE_LOCATION, Manifest.permission.ACCESS_FINE_LOCATION}, PERMISSIONS_LOCATION);
            } else {
                enableGps();
            }
        } else {
            mMapView.setMyLocationEnabled(false);
            mLocationFAB.setColorFilter(Color.TRANSPARENT);
        }
    }

    private void enableGps() {
        mMapView.setMyLocationEnabled(true);
        mLocationFAB.setColorFilter(ContextCompat.getColor(this, R.color.primary));
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
        List<MarkerOptions> markerOptionsList = new ArrayList<>();

        final MarkerOptions backLot = generateMarker("Back Lot", "The back lot behind my house", null, 38.649441, -121.369064);
        markerOptionsList.add(backLot);

        final MarkerOptions cheeseRoom = generateMarker("Cheese Room", "The only air conditioned room on the property", "dog-park-15", 38.531577, -122.010646);
        markerOptionsList.add(cheeseRoom);

       List<Marker> markers = mMapView.addMarkers(markerOptionsList);

        // need to call this after adding markers to map, click event hook into InfoWindow needs refactoring
        final Marker backLotMarker = markers.get(0);
        backLotMarker.setInfoWindowOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                Toast.makeText(getApplicationContext(), "Custom Info Touch Listener!!", Toast.LENGTH_SHORT).show();
                backLotMarker.hideInfoWindow();
                return true;
            }
        });
    }

    private MarkerOptions generateMarker(String title, String snippet, String sprite, double lat, double lng){
        return new MarkerOptions()
                .position(new LatLng(lat, lng))
                .title(title)
                .sprite(sprite)
                .snippet(snippet);
    }

    private void addPolyline() {
        try {
            String geojsonStr = Util.loadStringFromAssets(this, "small_line.geojson");
            LatLng[] latLngs = Util.parseGeoJSONCoordinates(geojsonStr);
            MapView map = mMapView;
            map.addPolyline(new PolylineOptions()
                    .add(latLngs)
                    .width(2)
                    .color(Color.RED));
        } catch (Exception e) {
            Log.e(TAG, "Error adding Polyline: " + e);
            e.printStackTrace();
        }
    }

    private void addPolygon() {
        try {
            String geojsonStr = Util.loadStringFromAssets(this, "small_poly.geojson");
            LatLng[] latLngs = Util.parseGeoJSONCoordinates(geojsonStr);
            MapView map = mMapView;
            ArrayList<PolygonOptions> opts = new ArrayList<>();
            opts.add(new PolygonOptions()
                    .add(latLngs)
                    .strokeColor(Color.MAGENTA)
                    .fillColor(Color.BLUE));
            map.addPolygons(opts).get(0);
        } catch (Exception e) {
            Log.e(TAG, "Error adding Polygon: " + e);
            e.printStackTrace();
        }
    }

    private void removeAnnotations() {
        mMapView.removeAllAnnotations();
    }

    // Called when FPS changes
    private class MyOnFpsChangedListener implements MapView.OnFpsChangedListener {

        @Override
        public void onFpsChanged(double fps) {
            mFpsTextView.setText(getResources().getString(R.string.label_fps) + String.format(" %4.2f", fps));
        }
    }

}
