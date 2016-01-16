package com.mapbox.mapboxsdk.testapp;

import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.widget.TextView;

import com.mapbox.geocoder.GeocoderCriteria;
import com.mapbox.geocoder.MapboxGeocoder;
import com.mapbox.geocoder.service.models.GeocoderFeature;
import com.mapbox.geocoder.service.models.GeocoderResponse;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;

import java.util.List;

import retrofit.Callback;
import retrofit.Response;
import retrofit.Retrofit;

public class GeocoderActivity extends AppCompatActivity {

    private static final String LOG_TAG = "GeocoderActivity";

    private MapView mapView;
    private TextView textView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_geocoder);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        textView = (TextView) findViewById(R.id.message);
        setMessage("Tap the map to trigger the geocoder.");

        LatLng dupontCircle = new LatLng(38.90962, -77.04341);

        mapView = (MapView) findViewById(R.id.mapView);
        mapView.setAccessToken(ApiAccess.getToken(this));
        mapView.setStyleUrl(Style.MAPBOX_STREETS);
        mapView.setCenterCoordinate(dupontCircle);
        mapView.setZoomLevel(15);
        mapView.onCreate(savedInstanceState);

        mapView.setOnMapClickListener(new MapView.OnMapClickListener() {
            @Override
            public void onMapClick(LatLng point) {
                setMessage("Geocoding...");
                mapView.removeAllAnnotations();
                mapView.addMarker(new MarkerOptions()
                        .position(point)
                        .title("Your finger is here"));
                geocode(point);
            }
        });
    }

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

    @Override
    public void onPause()  {
        super.onPause();
        mapView.onPause();
    }

    @Override
    public void onResume() {
        super.onResume();
        mapView.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mapView.onDestroy();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mapView.onSaveInstanceState(outState);
    }

    /*
     * Forward geocoding
     */

    private void geocode(LatLng point) {
        MapboxGeocoder client = new MapboxGeocoder.Builder()
                .setAccessToken(ApiAccess.getToken(this))
                .setCoordinates(point.getLongitude(), point.getLatitude())
                .setType(GeocoderCriteria.TYPE_POI)
                .build();

        client.enqueue(new Callback<GeocoderResponse>() {
            @Override
            public void onResponse(Response<GeocoderResponse> response, Retrofit retrofit) {
                List<GeocoderFeature> results = response.body().getFeatures();
                if (results.size() > 0) {
                    String placeName = results.get(0).getPlaceName();
                    setSuccess(placeName);
                } else {
                    setMessage("No results.");
                }
            }

            @Override
            public void onFailure(Throwable t) {
                setError(t.getMessage());
            }
        });

    }

    /*
     * Update text view
     */

    private void setMessage(String message) {
        Log.d(LOG_TAG, "Message: " + message);
        textView.setText(message);
    }

    private void setSuccess(String placeName) {
        Log.d(LOG_TAG, "Place name: " + placeName);
        textView.setText(placeName);
    }

    private void setError(String message) {
        Log.e(LOG_TAG, "Error: " + message);
        textView.setText("Error: " + message);
    }

}
