package com.mapbox.mapboxsdk.testapp;

import android.content.Context;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngZoom;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;

import org.json.JSONException;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;

public class BulkMarkerActivity extends AppCompatActivity {

    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_infowindow);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.setAccessToken(ApiAccess.getToken(this));
        mMapView.onCreate(savedInstanceState);
        mMapView.setCenterCoordinate(new LatLngZoom(38.87031, -77.00897, 10));

        new LoadBulkMarkerTask(mMapView, 1000).execute();
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

    private static class LoadBulkMarkerTask extends AsyncTask<Void, Void, List<MarkerOptions>> {

        private static final String TAG = "LoadBulkMarkerTask";
        private WeakReference<MapView> mMapView;
        private Context mAppContext;
        private int mAmount;

        public LoadBulkMarkerTask(MapView mapView, int amount) {
            mMapView = new WeakReference<>(mapView);
            mAppContext = mapView.getContext().getApplicationContext();
            mAmount = amount;
        }

        @Override
        protected List<MarkerOptions> doInBackground(Void... params) {
            List<MarkerOptions> markerOptions = new ArrayList<>();
            try {
                DecimalFormat formatter = new DecimalFormat("#.#####");
                String json = Util.loadStringFromAssets(mAppContext, "points.geojson");
                List<LatLng> locations = Util.parseGeoJSONCoordinates(json);

                LatLng location;
                for (int i = 0; i < mAmount; i++) {
                    location = locations.get(i);
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
        protected void onPostExecute(List<MarkerOptions> markerOptions) {
            super.onPostExecute(markerOptions);
            MapView mapView = mMapView.get();
            if (mapView != null) {
                List<Marker> markers = mapView.addMarkers(markerOptions);
                Log.v(TAG, "Markers added " + markers.size());
            }
        }
    }
}