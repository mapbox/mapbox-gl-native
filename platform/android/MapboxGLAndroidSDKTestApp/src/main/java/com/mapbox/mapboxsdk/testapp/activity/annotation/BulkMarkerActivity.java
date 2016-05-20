package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.ProgressDialog;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.StringDef;
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
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.annotations.MarkerViewOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.GeoParseUtil;
import com.mapbox.mapboxsdk.maps.MapView;

import org.json.JSONException;

import java.io.IOException;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class BulkMarkerActivity extends AppCompatActivity implements AdapterView.OnItemSelectedListener {

    private MapboxMap mMapboxMap;
    private MapView mMapView;
    private boolean mCustomMarkerView;
    private List<LatLng> mLocations;

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

                if (actionBar != null) {
                    ArrayAdapter<CharSequence> spinnerAdapter = ArrayAdapter.createFromResource(actionBar.getThemedContext(), R.array.bulk_marker_list, android.R.layout.simple_spinner_item);
                    spinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
                    Spinner spinner = (Spinner) findViewById(R.id.spinner);
                    spinner.setAdapter(spinnerAdapter);
                    spinner.setOnItemSelectedListener(BulkMarkerActivity.this);
                }
            }
        });

        final View fab = findViewById(R.id.fab);
        if (fab != null) {
            fab.setOnClickListener(new FabClickListener());
        }
    }

    public static class TextAdapter extends MapboxMap.MarkerViewAdapter<MarkerView> {

        private LayoutInflater inflater;

        public TextAdapter(@NonNull Context context) {
            super(context);
            this.inflater = LayoutInflater.from(context);
        }

        @Nullable
        @Override
        public View getView(@NonNull MarkerView marker, @Nullable View convertView, @NonNull ViewGroup parent) {
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
        int amount = Integer.valueOf(getResources().getStringArray(R.array.bulk_marker_list)[position]);
        if (mLocations == null) {
            new LoadLocationTask(this, amount).execute();
        } else {
            showMarkers(amount);
        }
    }


    private void onLatLngListLoaded(List<LatLng> latLngs, int amount) {
        mLocations = latLngs;
        showMarkers(amount);
    }

    private void showMarkers(int amount) {
        mMapboxMap.clear();

        if (mLocations.size() < amount) {
            amount = mLocations.size();
        }

        if (mCustomMarkerView) {
            showNativeMarkers(amount);
        } else {
            showGlMarkers(amount);
        }
    }

    private void showNativeMarkers(int amount) {
        List<MarkerViewOptions> markerOptionsList = new ArrayList<>();
        DecimalFormat formatter = new DecimalFormat("#.#####");
        Random random = new Random();
        int randomIndex;

        for (int i = 0; i < amount; i++) {
            randomIndex = random.nextInt(mLocations.size());
            LatLng latLng = mLocations.get(randomIndex);
            markerOptionsList.add(new MarkerViewOptions()
                    .position(latLng)
                    .title(String.valueOf(i))
                    .snippet(formatter.format(latLng.getLatitude()) + ", " + formatter.format(latLng.getLongitude())));
        }

        mMapboxMap.addMarkerViews(markerOptionsList);
    }

    private void showGlMarkers(int amount) {
        List<MarkerOptions> markerOptionsList = new ArrayList<>();
        DecimalFormat formatter = new DecimalFormat("#.#####");
        Random random = new Random();
        int randomIndex;

        for (int i = 0; i < amount; i++) {
            randomIndex = random.nextInt(mLocations.size());
            LatLng latLng = mLocations.get(randomIndex);
            markerOptionsList.add(new MarkerOptions()
                    .position(latLng)
                    .title(String.valueOf(i))
                    .snippet(formatter.format(latLng.getLatitude()) + ", " + formatter.format(latLng.getLongitude())));
        }

        mMapboxMap.addMarkers(markerOptionsList);
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

    private class FabClickListener implements View.OnClickListener {
        @Override
        public void onClick(final View v) {
            if (mMapboxMap != null) {
                mCustomMarkerView = true;

                // remove fab
                v.animate().alpha(0).setListener(new AnimatorListenerAdapter() {
                    @Override
                    public void onAnimationEnd(Animator animation) {
                        super.onAnimationEnd(animation);
                        v.setVisibility(View.GONE);
                    }
                }).start();

                // reload markers
                Spinner spinner = (Spinner) findViewById(R.id.spinner);
                if (spinner != null) {
                    int amount = Integer.valueOf(getResources().getStringArray(R.array.bulk_marker_list)[spinner.getSelectedItemPosition()]);
                    showMarkers(amount);
                }

                // add adapter
                mMapboxMap.addMarkerViewAdapter(new TextAdapter(BulkMarkerActivity.this));

                mMapView.addOnMapChangedListener(new MapView.OnMapChangedListener() {
                    @Override
                    public void onMapChanged(@MapView.MapChange int change) {
                        if (change == MapView.REGION_IS_CHANGING || change == MapView.REGION_DID_CHANGE) {
                            if (!mMapboxMap.getMarkerViewAdapters().isEmpty()) {
                                TextView viewCountView = (TextView) findViewById(R.id.countView);
                                viewCountView.setText("ViewCache size " + (mMapView.getChildCount() - 5));
                            }
                        }
                    }
                });

                mMapboxMap.setOnMarkerViewClickListener(new MapboxMap.OnMarkerViewClickListener() {
                    @Override
                    public boolean onMarkerClick(@NonNull Marker marker, @NonNull View view, @NonNull MapboxMap.MarkerViewAdapter adapter) {
                        Toast.makeText(BulkMarkerActivity.this, "Hello " + marker.getId(), Toast.LENGTH_SHORT).show();
                        return false;
                    }
                });
            }
        }
    }

    private static class LoadLocationTask extends AsyncTask<Void, Integer, List<LatLng>> {

        private static final String TAG = "LoadLocationTask";
        private BulkMarkerActivity mActivity;
        private ProgressDialog mProgressDialog;
        private int mAmount;

        public LoadLocationTask(BulkMarkerActivity activity, int amount) {
            mAmount = amount;
            mActivity = activity;
            mProgressDialog = ProgressDialog.show(activity, "Loading", "Fetching markers", false);
        }

        @Override
        protected List<LatLng> doInBackground(Void... params) {
            try {
                String json = GeoParseUtil.loadStringFromAssets(mActivity.getApplicationContext(), "points.geojson");
                return GeoParseUtil.parseGeoJSONCoordinates(json);
            } catch (IOException | JSONException e) {
                Log.e(TAG, "Could not add markers,", e);
                return null;
            }
        }

        @Override
        protected void onPostExecute(List<LatLng> locations) {
            super.onPostExecute(locations);
            mActivity.onLatLngListLoaded(locations, mAmount);
            mProgressDialog.hide();
        }
    }
}
