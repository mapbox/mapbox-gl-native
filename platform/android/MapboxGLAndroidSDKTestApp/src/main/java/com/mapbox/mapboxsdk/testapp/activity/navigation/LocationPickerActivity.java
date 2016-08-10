package com.mapbox.mapboxsdk.testapp.activity.navigation;

import android.Manifest;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.PointF;
import android.location.Location;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.Toast;

import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.annotations.MarkerViewOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.LocationListener;
import com.mapbox.mapboxsdk.location.LocationServices;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.model.annotations.PulseMarkerView;
import com.mapbox.mapboxsdk.testapp.model.annotations.PulseMarkerViewOptions;
import com.mapbox.services.commons.ServicesException;
import com.mapbox.services.commons.models.Position;
import com.mapbox.services.geocoding.v5.GeocodingCriteria;
import com.mapbox.services.geocoding.v5.MapboxGeocoding;
import com.mapbox.services.geocoding.v5.models.CarmenFeature;
import com.mapbox.services.geocoding.v5.models.GeocodingResponse;

import java.util.List;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

/**
 * Sample Activity to show a typical location picker use case
 */
public class LocationPickerActivity extends AppCompatActivity {
    private static final String TAG = "LocationPickerActivity";
    private static final int REQUEST_PERMISSIONS = 101;

    private MapView mapView;
    private MapboxMap mapboxMap;

    private ImageView dropPinView;
    private Marker addressPin;
    private ImageButton clearDisplayViewButton;
    private MarkerView userMarker;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_location_picker);

        setupActionBar();

        //Initialize map as normal
        mapView = (MapView) findViewById(R.id.mapView);
        mapView.onCreate(savedInstanceState);

        //Create ui elements
        createDropPin();
        createSelectLocationButton();
        createClearSelectionButton();

        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(MapboxMap map) {
                //Store for later
                mapboxMap = map;

                //Add user marker
                mapboxMap.getMarkerViewManager().addMarkerViewAdapter(new PulseMarkerViewAdapter(LocationPickerActivity.this));
                userMarker = createCustomUserMarker(new LatLng(0, 0));

                //Fix the focal point to the center of the map
                PointF focalPoint = new PointF((mapView.getX() + mapView.getWidth() / 2), (mapView.getY() + mapView.getHeight() / 2));
                mapboxMap.getUiSettings().setFocalPoint(focalPoint);

                //Track camera updates to animate the user location views
                trackUserLocationView(userMarker);
            }
        });
    }

    @Override
    public void onResume() {
        super.onResume();
        mapView.onResume();

        //Check permissions
        if (arePermissionsGranted()) {
            mapView.getMapAsync(new OnMapReadyCallback() {

                @Override
                public void onMapReady(final MapboxMap mapboxMap) {
                    //Get the user's location
                    final LocationServices locationServices = LocationServices.getLocationServices(getApplicationContext());

                    Location location = locationServices.getLastLocation();
                    if (location != null) {
                        zoomInOn(location);
                        userMarker.setPosition(new LatLng(location));
                    } else {
                        final ProgressDialog loadingDialog = ProgressDialog.show(LocationPickerActivity.this, "Loading", "Getting user location", false);
                        locationServices.addLocationListener(new LocationListener() {
                            @Override
                            public void onLocationChanged(@Nullable Location location) {
                                //Move the camera to the user
                                if (location != null) {
                                    zoomInOn(location);
                                    userMarker.setPosition(new LatLng(location));
                                    locationServices.removeLocationListener(this);
                                    loadingDialog.hide();
                                }
                            }
                        });
                    }

                    locationServices.toggleGPS(true);
                }
            });
        }
    }

    private void zoomInOn(Location location) {
        //Move the camera to the user
        if (location != null) {
            mapboxMap.setCameraPosition(new CameraPosition.Builder()
                    .target(new LatLng(location))
                    .zoom(16)
                    .bearing(0)
                    .tilt(0)
                    .build());
        }
    }


    /**
     * Tracks the camera to animate the marker when overlapping with the picker.
     * Makes sure the marker actually points to the user's position by tracking it.
     */
    private void trackUserLocationView(final MarkerView markerView) {
        final float circleDiameterSize = getResources().getDimension(R.dimen.circle_size);

        //Track camera changes to check for overlap
        mapboxMap.setOnCameraChangeListener(new MapboxMap.OnCameraChangeListener() {

            private Animation pulseAnimation;

            @Override
            public void onCameraChange(CameraPosition position) {
                if (markerView == null) {
                    return;
                }

                //Make drop pin visible, if it wasn't already
                showDropPin();

                //Get the distance from the tip of the location picker to the MarkerView
                double distance = getLocationPickerLocation().distanceTo(markerView.getPosition());

                //If closeby, animate, otherwise, stop animation
                View view = mapboxMap.getMarkerViewManager().getView(markerView);
                if (view != null) {
                    View backgroundView = view.findViewById(R.id.background_imageview);
                    if (pulseAnimation == null && distance < 0.5f * circleDiameterSize) {
                        pulseAnimation = AnimationUtils.loadAnimation(LocationPickerActivity.this, R.anim.pulse);
                        pulseAnimation.setRepeatCount(Animation.INFINITE);
                        pulseAnimation.setRepeatMode(Animation.RESTART);
                        backgroundView.startAnimation(pulseAnimation);
                    } else if (pulseAnimation != null && distance >= 0.5f * circleDiameterSize) {
                        backgroundView.clearAnimation();
                        pulseAnimation = null;
                    }
                }
            }
        });

        //Track location updates to move the user marker
        LocationServices.getLocationServices(getApplicationContext()).addLocationListener(new LocationListener() {
            @Override
            public void onLocationChanged(Location location) {
                if (location != null && markerView != null) {
                    markerView.setPosition(new LatLng(location));
                }
            }
        });
    }

    private MarkerView createCustomUserMarker(LatLng markerPosition) {
        return mapboxMap.addMarker(new PulseMarkerViewOptions()
                .icon(IconFactory.getInstance(getApplicationContext()).fromResource(R.drawable.ic_my_location_24dp))
                .position(markerPosition)
        );
    }

    private void createClearSelectionButton() {
        clearDisplayViewButton = (ImageButton) findViewById(R.id.clearDisplayViewButton);
        clearDisplayViewButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                removeAddressPin();
                hide(clearDisplayViewButton);
                showDropPin();
            }
        });
    }

    private void createSelectLocationButton() {
        Button selectLocationButton = (Button) findViewById(R.id.selectLocationButton);
        //noinspection ConstantConditions
        selectLocationButton.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        Log.i(TAG, "Location Selected!");
                        if (mapboxMap != null) {
                            //Control button's state
                            clearDisplayViewButton.setVisibility(View.VISIBLE);
                            dropPinView.setVisibility(View.INVISIBLE);

                            //Get position for the drop pin
                            LatLng position = getLocationPickerLocation();

                            //Show the address pin (result)
                            showAddressPin(position);

                            //Get the address for that location and update the marker
                            geocode(position, new GeocodeCallbacks() {
                                @Override
                                public void onResult(String result) {
                                    updateAddressPin(result);
                                }

                                @Override
                                public void onFailure(Throwable failure) {
                                    showFeedbackMessage("Could not retrieve address: " + failure.getMessage());
                                }
                            });
                        }
                    }
                }
        );
    }

    private void createDropPin() {
        float density = getResources().getDisplayMetrics().density;

        dropPinView = new ImageView(this);
        dropPinView.setImageResource(R.drawable.ic_droppin_24dp);
        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT, Gravity.CENTER);
        params.bottomMargin = (int) (12 * density);
        dropPinView.setLayoutParams(params);

        mapView.addView(dropPinView);
    }

    private void showDropPin() {
        if (dropPinView != null && dropPinView.getVisibility() != View.VISIBLE) {
            dropPinView.setVisibility(View.VISIBLE);
        }
    }

    private void hide(View view) {
        if (view != null) {
            view.setVisibility(View.INVISIBLE);
        }
    }

    private void setupActionBar() {
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }
    }

    /**
     * Get address for the given location
     */
    private void geocode(LatLng point, final GeocodeCallbacks callbacks) {
        try {
            //Create Geocoding client
            MapboxGeocoding client = new MapboxGeocoding.Builder()
                    .setAccessToken(getString(R.string.mapbox_access_token))
                    .setCoordinates(Position.fromCoordinates(point.getLongitude(), point.getLatitude()))
                    .setGeocodingType(GeocodingCriteria.TYPE_ADDRESS)
                    .build();

            //Place the request
            client.enqueueCall(new Callback<GeocodingResponse>() {
                @Override
                public void onResponse(Call<GeocodingResponse> call, Response<GeocodingResponse> response) {

                    List<CarmenFeature> results = response.body().getFeatures();
                    String address = null;
                    if (results.size() > 0) {
                        CarmenFeature feature = results.get(0);
                        address = feature.getAddress() + " " + feature.getText();
                        Log.i(TAG, "address " + address);
                    } else {
                        showFeedbackMessage("No results for search.");
                    }

                    callbacks.onResult(address);
                }

                @Override
                public void onFailure(Call<GeocodingResponse> call, Throwable t) {
                    Log.e(TAG, "Geocoding Failure: " + t.getMessage());
                    callbacks.onFailure(t);
                }
            });
        } catch (ServicesException e) {
            Log.e(TAG, "Error geocoding: " + e.toString());
            callbacks.onFailure(e);
        }
    }

    private LatLng getLocationPickerLocation() {
        return mapboxMap.getProjection().fromScreenLocation(
                new PointF(dropPinView.getLeft() + (dropPinView.getWidth() / 2), dropPinView.getBottom())
        );
    }

    private Marker showAddressPin(LatLng position) {
        if (addressPin != null) {
            //Remove previous pin
            removeAddressPin();
        }

        //Create new one
        addressPin = mapboxMap.addMarker(new MarkerViewOptions().title("Loading address...").position(position));
        mapboxMap.selectMarker(addressPin);
        return addressPin;
    }

    private void removeAddressPin() {
        if (mapboxMap != null && addressPin != null) {
            mapboxMap.removeMarker(addressPin);
        }
    }

    private void updateAddressPin(@Nullable String address) {
        if (addressPin != null) {
            addressPin.setTitle(address == null ? "No address found" : address);
        }
    }

    private void showFeedbackMessage(String message) {
        Toast.makeText(getApplicationContext(), message, Toast.LENGTH_SHORT).show();
    }

    private boolean arePermissionsGranted() {
        if (Build.VERSION.SDK_INT >= 23 &&
                checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED &&
                checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            Log.i(TAG, "Requesting permissions");
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, REQUEST_PERMISSIONS);
            return false;
        }
        Log.i(TAG, "Permissions already granted");
        return true;
    }

    @Override
    public void onPause() {
        super.onPause();
        mapView.onPause();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mapView.onSaveInstanceState(outState);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mapView.onDestroy();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mapView.onLowMemory();
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

    /**
     * Custom MarkerViewAdapter for the pulsing marker
     */
    private static class PulseMarkerViewAdapter extends MapboxMap.MarkerViewAdapter<PulseMarkerView> {

        private LayoutInflater inflater;

        public PulseMarkerViewAdapter(@NonNull Context context) {
            super(context);
            this.inflater = LayoutInflater.from(context);
        }

        @Nullable
        @Override
        public View getView(@NonNull PulseMarkerView marker, @Nullable View convertView, @NonNull ViewGroup parent) {
            ViewHolder viewHolder;
            if (convertView == null) {
                viewHolder = new ViewHolder();
                convertView = inflater.inflate(R.layout.view_pulse_marker, parent, false);
                viewHolder.foregroundImageView = (ImageView) convertView.findViewById(R.id.foreground_imageView);
                viewHolder.backgroundImageView = (ImageView) convertView.findViewById(R.id.background_imageview);
                convertView.setTag(viewHolder);
            }
            return convertView;
        }

        private static class ViewHolder {
            ImageView foregroundImageView;
            ImageView backgroundImageView;
        }
    }

    private interface GeocodeCallbacks {
        void onResult(String result);

        void onFailure(Throwable failure);
    }
}
