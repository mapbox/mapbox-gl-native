package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.app.ProgressDialog;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.TextView;

import com.mapbox.mapboxsdk.annotations.BaseMarkerOptions;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.model.annotations.CountryMarker;
import com.mapbox.mapboxsdk.testapp.utils.GeoParseUtil;
import com.mapbox.mapboxsdk.maps.MapView;

import org.json.JSONException;

import java.io.IOException;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;

public class BulkMarkerActivity extends AppCompatActivity implements AdapterView.OnItemSelectedListener {

    private MapboxMap mMapboxMap;
    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_marker_bulk);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        final ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayShowTitleEnabled(false);
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                mMapboxMap = mapboxMap;

                mMapboxMap.setMarkerViewAdapter(new TextAdapter(BulkMarkerActivity.this));

                if (actionBar != null) {
                    ArrayAdapter<CharSequence> spinnerAdapter = ArrayAdapter.createFromResource(actionBar.getThemedContext(), R.array.bulk_marker_list, android.R.layout.simple_spinner_item);
                    spinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
                    Spinner spinner = (Spinner) findViewById(R.id.spinner);
                    spinner.setAdapter(spinnerAdapter);
                    spinner.setOnItemSelectedListener(BulkMarkerActivity.this);
                }
            }
        });
    }

    private static class TextAdapter implements MapboxMap.MarkerViewAdapter<Marker> {

        private LayoutInflater inflater;

        public TextAdapter(@NonNull Context context) {
            this.inflater = LayoutInflater.from(context);
        }

        @Nullable
        @Override
        public View getView(@NonNull Marker marker, @Nullable View convertView, @NonNull ViewGroup parent) {
            ViewHolder viewHolder;
            if (convertView == null) {
                viewHolder = new ViewHolder();
                convertView = inflater.inflate(R.layout.view_text_marker, parent, false);
                viewHolder.title = (TextView) convertView.findViewById(R.id.textView);
                convertView.setTag(viewHolder);
            } else {
                viewHolder = (ViewHolder) convertView.getTag();
            }
            viewHolder.title.setText(marker.getTitle());
            return convertView;
        }

        private static class ViewHolder {
            TextView title;
        }
    }


    @Override
    public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
        int markersAmount = Integer.valueOf(getResources().getStringArray(R.array.bulk_marker_list)[position]);
        new LoadBulkMarkerTask(this, mMapboxMap, markersAmount).execute();
    }

    @Override
    public void onNothingSelected(AdapterView<?> parent) {
        // nothing selected, nothing to do!
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

    private static class LoadBulkMarkerTask extends AsyncTask<Void, Integer, List<BaseMarkerOptions>> {

        private static final String TAG = "LoadBulkMarkerTask";
        private MapboxMap mMapboxMap;
        private Context mAppContext;
        private ProgressDialog mProgressDialog;
        private int mAmount;

        public LoadBulkMarkerTask(Context context, MapboxMap mapboxMap, int amount) {
            mMapboxMap = mapboxMap;
            mapboxMap.removeAnnotations();
            mProgressDialog = ProgressDialog.show(context, "Loading", "Fetching markers", false);
            mAppContext = context.getApplicationContext();
            mAmount = amount;
        }

        @Override
        protected List<BaseMarkerOptions> doInBackground(Void... params) {
            List<BaseMarkerOptions> markerOptions = new ArrayList<>(mAmount);
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
                    markerOptions.add(new MarkerOptions()
                            .position(location)
                            .title(String.valueOf(i))
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
        protected void onPostExecute(List<BaseMarkerOptions> markerOptions) {
            super.onPostExecute(markerOptions);
            mMapboxMap.addMarkers(markerOptions);
            mProgressDialog.hide();
        }
    }
}
