package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.app.ProgressDialog;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v4.view.MenuItemCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.GeoParseUtil;
import timber.log.Timber;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;

/**
 * Test activity showcasing adding a large amount of Markers.
 */
public class BulkMarkerActivity extends AppCompatActivity implements AdapterView.OnItemSelectedListener {

  private MapboxMap mapboxMap;
  private MapView mapView;
  private List<LatLng> locations;
  private ProgressDialog progressDialog;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_marker_bulk);

    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this::initMap);
  }

  private void initMap(MapboxMap mapboxMap) {
    this.mapboxMap =  mapboxMap;
    mapboxMap.setStyle(Style.MAPBOX_STREETS);
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    ArrayAdapter<CharSequence> spinnerAdapter = ArrayAdapter.createFromResource(
      this, R.array.bulk_marker_list, android.R.layout.simple_spinner_item);
    spinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
    getMenuInflater().inflate(R.menu.menu_bulk_marker, menu);
    MenuItem item = menu.findItem(R.id.spinner);
    Spinner spinner = (Spinner) MenuItemCompat.getActionView(item);
    spinner.setAdapter(spinnerAdapter);
    spinner.setOnItemSelectedListener(BulkMarkerActivity.this);
    return true;
  }

  @Override
  public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
    int amount = Integer.valueOf(getResources().getStringArray(R.array.bulk_marker_list)[position]);
    if (locations == null) {
      progressDialog = ProgressDialog.show(this, "Loading", "Fetching markers", false);
      new LoadLocationTask(this, amount).execute();
    } else {
      showMarkers(amount);
    }
  }

  private void onLatLngListLoaded(List<LatLng> latLngs, int amount) {
    progressDialog.hide();
    locations = latLngs;
    showMarkers(amount);
  }

  private void showMarkers(int amount) {
    if (mapboxMap == null || locations == null || mapView.isDestroyed()) {
      return;
    }

    mapboxMap.clear();
    if (locations.size() < amount) {
      amount = locations.size();
    }

    showGlMarkers(amount);
  }

  private void showGlMarkers(int amount) {
    List<MarkerOptions> markerOptionsList = new ArrayList<>();
    DecimalFormat formatter = new DecimalFormat("#.#####");
    Random random = new Random();
    int randomIndex;

    for (int i = 0; i < amount; i++) {
      randomIndex = random.nextInt(locations.size());
      LatLng latLng = locations.get(randomIndex);
      markerOptionsList.add(new MarkerOptions()
        .position(latLng)
        .title(String.valueOf(i))
        .snippet(formatter.format(latLng.getLatitude()) + ", " + formatter.format(latLng.getLongitude())));
    }

    mapboxMap.addMarkers(markerOptionsList);
  }

  @Override
  public void onNothingSelected(AdapterView<?> parent) {
    // nothing selected, nothing to do!
  }

  @Override
  protected void onStart() {
    super.onStart();
    mapView.onStart();
  }

  @Override
  protected void onResume() {
    super.onResume();
    mapView.onResume();
  }

  @Override
  protected void onPause() {
    super.onPause();
    mapView.onPause();
  }

  @Override
  protected void onStop() {
    super.onStop();
    mapView.onStop();
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    if (progressDialog != null) {
      progressDialog.dismiss();
    }
    mapView.onDestroy();
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  private static class LoadLocationTask extends AsyncTask<Void, Integer, List<LatLng>> {

    private WeakReference<BulkMarkerActivity> activity;
    private int amount;

    private LoadLocationTask(BulkMarkerActivity activity, int amount) {
      this.amount = amount;
      this.activity = new WeakReference<>(activity);
    }

    @Override
    protected List<LatLng> doInBackground(Void... params) {
      BulkMarkerActivity activity = this.activity.get();
      if (activity != null) {
        String json = null;
        try {
          json = GeoParseUtil.loadStringFromAssets(activity.getApplicationContext(), "points.geojson");
        } catch (IOException exception) {
          Timber.e(exception, "Could not add markers");
        }

        if (json != null) {
          return GeoParseUtil.parseGeoJsonCoordinates(json);
        }
      }
      return null;
    }

    @Override
    protected void onPostExecute(List<LatLng> locations) {
      super.onPostExecute(locations);
      BulkMarkerActivity activity = this.activity.get();
      if (activity != null) {
        activity.onLatLngListLoaded(locations, amount);
      }
    }
  }
}
