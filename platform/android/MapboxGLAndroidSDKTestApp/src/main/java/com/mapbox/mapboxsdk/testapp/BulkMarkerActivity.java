package com.mapbox.mapboxsdk.testapp;

import android.app.ProgressDialog;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngZoom;
import com.mapbox.mapboxsdk.testapp.utils.GeoParseUtil;
import com.mapbox.mapboxsdk.testapp.utils.TimingLogger;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;

import org.json.JSONException;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;

public class BulkMarkerActivity extends AppCompatActivity implements AdapterView.OnItemSelectedListener {

    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_marker_bulk);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayShowTitleEnabled(false);
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.setAccessToken(ApiAccess.getToken(this));
        mMapView.onCreate(savedInstanceState);
        mMapView.setCenterCoordinate(new LatLngZoom(38.87031, -77.00897, 10));
        mMapView.setCompassEnabled(false);

        ArrayAdapter<CharSequence> spinnerAdapter = ArrayAdapter.createFromResource(actionBar.getThemedContext(), R.array.bulk_marker_list, android.R.layout.simple_spinner_item);
        spinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        Spinner spinner = (Spinner) findViewById(R.id.spinner);
        spinner.setAdapter(spinnerAdapter);
        spinner.setOnItemSelectedListener(this);
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        int markersAmount = Integer.valueOf(getResources().getStringArray(R.array.bulk_marker_list)[position]);
        new LoadBulkMarkerTask(mMapView, markersAmount).execute();
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

    }

    @Override
    protected void onStart() {
        super.onStart();
        mMapView.onStart();
    }

    @Override
    public void onResume() {
        super.onResume();
        mMapView.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        mMapView.onPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
        mMapView.onStop();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mMapView.onSaveInstanceState(outState);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMapView.onDestroy();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mMapView.onLowMemory();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private static class LoadBulkMarkerTask extends AsyncTask<Void, Integer, List<MarkerOptions>> {

        private static final String TAG = "LoadBulkMarkerTask";
        private WeakReference<MapView> mMapView;
        private Context mAppContext;
        private ProgressDialog mProgressDialog;
        private int mAmount;

        public LoadBulkMarkerTask(MapView mapView, int amount) {
            Context context = mapView.getContext();
            mapView.removeAllAnnotations();
            mMapView = new WeakReference<>(mapView);
            mProgressDialog = ProgressDialog.show(context, "Loading", "Fetching markers", false);
            mAppContext = context.getApplicationContext();
            mAmount = amount;
        }

        @Override
        protected List<MarkerOptions> doInBackground(Void... params) {
            //Debug.startMethodTracing("bulk-marker");
            TimingLogger timings = new TimingLogger(TAG, "doInBackground");
            //Log.d(TAG, "isLoggable " + Log.isLoggable(TAG, Log.VERBOSE));

            List<MarkerOptions> markerOptions = new ArrayList<>(mAmount);
            timings.addSplit("create ArrayList");

            try {
                DecimalFormat formatter = new DecimalFormat("#.#####");

                String json = GeoParseUtil.loadStringFromAssets(mAppContext, "points.geojson");
                timings.addSplit("loadStringFromAssets");

                List<LatLng> locations = GeoParseUtil.parseGeoJSONCoordinates(json);

                timings.addSplit("parseGeoJSONCoordinates");

                if (locations.size() < mAmount) {
                    mAmount = locations.size();
                }

                LatLng location;
                for (int i = 0; i < mAmount; i++) {
                    location = locations.get(i);
                    Log.v(TAG, "marker " + i + " of " + mAmount + " added with " + location.getLatitude() + " " + location.getLongitude());
                    markerOptions.add(new MarkerOptions()
                            .position(location)
                            .title("Marker")
                            .snippet(formatter.format(location.getLatitude()) + ", " + formatter.format(location.getLongitude())));
                }
                timings.addSplit("create all MarkerOptions");

            } catch (IOException | JSONException e) {
                Log.e(TAG, "Could not add markers,", e);
            }

            timings.dumpToLog();
            return markerOptions;
        }

        @Override
        protected void onProgressUpdate(Integer... values) {
            super.onProgressUpdate(values);
        }

        @Override
        protected void onPostExecute(List<MarkerOptions> markerOptions) {
            super.onPostExecute(markerOptions);
            TimingLogger timings = new TimingLogger(TAG, "onPostExecute");

            MapView mapView = mMapView.get();
            if (mapView != null) {
                List<Marker> markers = mapView.addMarkers(markerOptions);
                Log.v(TAG, "Markers added " + markers.size());
            }
            timings.addSplit("addMarkers");

            timings.dumpToLog();
            mProgressDialog.hide();
            //Debug.stopMethodTracing();
        }
    }
}