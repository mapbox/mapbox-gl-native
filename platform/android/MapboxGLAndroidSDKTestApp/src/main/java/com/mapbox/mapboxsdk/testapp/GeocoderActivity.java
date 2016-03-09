package com.mapbox.mapboxsdk.testapp;

import android.graphics.PointF;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Gravity;
import android.view.MenuItem;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import com.mapbox.geocoder.GeocoderCriteria;
import com.mapbox.geocoder.MapboxGeocoder;
import com.mapbox.geocoder.service.models.GeocoderFeature;
import com.mapbox.geocoder.service.models.GeocoderResponse;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.Projection;
import com.mapbox.mapboxsdk.testapp.utils.ApiAccess;
import com.mapbox.mapboxsdk.maps.MapView;

import java.util.List;

import retrofit.Callback;
import retrofit.Response;
import retrofit.Retrofit;

public class GeocoderActivity extends AppCompatActivity {

    private static final String LOG_TAG = "GeocoderActivity";
    private static final LatLng DC_DUPONT_CIRCLE = new LatLng(38.90962, -77.04341);

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

        mapView = (MapView) findViewById(R.id.mapView);
        mapView.setAccessToken(ApiAccess.getToken(this));
        mapView.setStyle(Style.MAPBOX_STREETS);
        mapView.onCreate(savedInstanceState);

        final ImageView dropPinView = new ImageView(this);
        dropPinView.setImageResource(R.drawable.ic_droppin_24dp);
        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT, Gravity.CENTER);
        dropPinView.setLayoutParams(params);
        mapView.addView(dropPinView);

        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull final MapboxMap mapboxMap) {
                final Projection projection = mapboxMap.getProjection();
                final int width = mapView.getMeasuredWidth();
                final int height = mapView.getMeasuredHeight();

                // Camera position
                mapboxMap.setCameraPosition(
                        new CameraPosition.Builder()
                                .target(DC_DUPONT_CIRCLE)
                                .zoom(15)
                                .bearing(0)
                                .tilt(0)
                                .build()
                );

                // Click listener
                mapboxMap.setOnMapClickListener(new MapboxMap.OnMapClickListener() {
                    @Override
                    public void onMapClick(@NonNull LatLng point) {
                        PointF centerPoint = new PointF(width / 2, (height + dropPinView.getHeight()) / 2);
                        LatLng centerLatLng = new LatLng(projection.fromScreenLocation(centerPoint));

                        setMessage("Geocoding...");

                        mapboxMap.removeAnnotations();
                        mapboxMap.addMarker(new MarkerOptions().position(centerLatLng));

                        geocode(centerLatLng);
                    }
                });
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
    public void onPause() {
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

    private void geocode(final LatLng point) {
        new AsyncTask<Void, Void, Void>() {

            @Override
            protected Void doInBackground(Void... params) {
                MapboxGeocoder client = new MapboxGeocoder.Builder()
                        .setAccessToken(ApiAccess.getToken(GeocoderActivity.this))
                        .setCoordinates(point.getLongitude(), point.getLatitude())
                        .setType(GeocoderCriteria.TYPE_POI)
                        .build();

                client.enqueue(new Callback<GeocoderResponse>()

                               {
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
                               }

                );
                return null;
            }
        }.execute();
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
}
