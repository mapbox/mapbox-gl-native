package com.mapbox.mapboxsdk.testapp;

import android.app.ProgressDialog;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;

import com.mapbox.mapboxsdk.MapFragment;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngZoom;
import com.mapbox.mapboxsdk.testapp.utils.GeoParseUtil;
import com.mapbox.mapboxsdk.views.MapboxMap;
import com.mapbox.mapboxsdk.views.OnMapReadyCallback;

import org.json.JSONException;

import java.io.IOException;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;

public class BulkMarkerActivity extends AppCompatActivity implements OnMapReadyCallback,AdapterView.OnItemSelectedListener {

    private MapboxMap mMapboxMap;
    private ActionBar mActionBar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_marker_bulk);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        mActionBar = getSupportActionBar();
        if (mActionBar != null) {
            mActionBar.setDisplayShowTitleEnabled(false);
            mActionBar.setDisplayHomeAsUpEnabled(true);
            mActionBar.setDisplayShowHomeEnabled(true);
        }

        MapFragment mapFragment = (MapFragment) getSupportFragmentManager().findFragmentById(R.id.mapFragment);
        mapFragment.getMapAsync(this);
    }

    @Override
    public void onMapReady(@NonNull MapboxMap mapboxMap) {
        mMapboxMap = mapboxMap;
        mMapboxMap.setStyle(Style.LIGHT);
        mMapboxMap.setLatLng(new LatLngZoom(38.87031, -77.00897, 10));
        mMapboxMap.setCompassEnabled(true);

        ArrayAdapter<CharSequence> spinnerAdapter = ArrayAdapter.createFromResource(mActionBar.getThemedContext(), R.array.bulk_marker_list, android.R.layout.simple_spinner_item);
        spinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        Spinner spinner = (Spinner) findViewById(R.id.spinner);
        spinner.setAdapter(spinnerAdapter);
        spinner.setOnItemSelectedListener(BulkMarkerActivity.this);
    }

    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        int markersAmount = Integer.valueOf(getResources().getStringArray(R.array.bulk_marker_list)[position]);
        new LoadBulkMarkerTask(this, mMapboxMap, markersAmount).execute();
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {

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
        private MapboxMap mMapboxMap;
        private Context mAppContext;
        private ProgressDialog mProgressDialog;
        private int mAmount;

        public LoadBulkMarkerTask(Context context, MapboxMap mapboxMap, int amount) {
            mAppContext = context.getApplicationContext();
            mapboxMap.removeAllAnnotations();
            mMapboxMap = mapboxMap;
            mProgressDialog = ProgressDialog.show(context, "Loading", "Fetching markers", false);
            mAmount = amount;
        }

        @Override
        protected List<MarkerOptions> doInBackground(Void... params) {
            List<MarkerOptions> markerOptions = new ArrayList<>(mAmount);
            try {
                DecimalFormat formatter = new DecimalFormat("#.#####");
                String json = GeoParseUtil.loadStringFromAssets(mAppContext, "points.geojson");
                List<LatLng> locations = GeoParseUtil.parseGeoJSONCoordinates(json);

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

            } catch (IOException | JSONException e) {
                Log.e(TAG, "Could not add markers,", e);
            }

            return markerOptions;
        }

        @Override
        protected void onProgressUpdate(Integer... values) {
            super.onProgressUpdate(values);
        }

        @Override
        protected void onPostExecute(List<MarkerOptions> markerOptions) {
            super.onPostExecute(markerOptions);
            mMapboxMap.addMarkers(markerOptions);
            mProgressDialog.hide();
        }
    }
}