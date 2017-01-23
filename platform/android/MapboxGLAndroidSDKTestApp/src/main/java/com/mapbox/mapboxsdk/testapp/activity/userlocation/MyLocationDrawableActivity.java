package com.mapbox.mapboxsdk.testapp.activity.userlocation;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.location.Location;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.view.ViewGroup;

import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.location.LocationListener;
import com.mapbox.mapboxsdk.location.LocationServices;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

public class MyLocationDrawableActivity extends AppCompatActivity implements LocationListener {

  private static final int PERMISSIONS_LOCATION = 0;

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(@Nullable Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_my_location_customization);

    findViewById(R.id.progress).setVisibility(View.GONE);

    MapboxMapOptions mapboxMapOptions = new MapboxMapOptions();
    mapboxMapOptions.styleUrl(Style.MAPBOX_STREETS);

    // configure MyLocationView drawables
    mapboxMapOptions.myLocationForegroundDrawable(ContextCompat.getDrawable(this, R.drawable.ic_chelsea));
    mapboxMapOptions.myLocationBackgroundDrawable(ContextCompat.getDrawable(this, R.drawable.ic_arsenal));
    mapboxMapOptions.myLocationForegroundTintColor(Color.GREEN);
    mapboxMapOptions.myLocationBackgroundTintColor(Color.YELLOW);
    mapboxMapOptions.myLocationBackgroundPadding(new int[] {0, 0,
      (int) getResources().getDimension(R.dimen.locationview_background_drawable_padding),
      (int) getResources().getDimension(R.dimen.locationview_background_drawable_padding)});

    mapboxMapOptions.myLocationAccuracyTint(Color.RED);
    mapboxMapOptions.myLocationAccuracyAlpha(155);

    mapView = new MapView(this, mapboxMapOptions);
    mapView.setId(R.id.mapView);
    ViewGroup parent = (ViewGroup) findViewById(R.id.container);
    parent.addView(mapView);

    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(MapboxMap map) {
        mapboxMap = map;
        toggleGps(true);
      }
    });
  }

  public void toggleGps(boolean enableGps) {
    if (enableGps) {
      if ((ContextCompat.checkSelfPermission(this,
        Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED)
        || (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION)
        != PackageManager.PERMISSION_GRANTED)) {
        ActivityCompat.requestPermissions(this, new String[] {
          Manifest.permission.ACCESS_COARSE_LOCATION,
          Manifest.permission.ACCESS_FINE_LOCATION}, PERMISSIONS_LOCATION);
      } else {
        enableLocation(true);
      }
    } else {
      enableLocation(false);
    }
  }

  private void enableLocation(boolean enabled) {
    if (enabled) {
      mapboxMap.setMyLocationEnabled(true);
      Location location = mapboxMap.getMyLocation();
      if (location != null) {
        onLocationChanged(location);
      } else {
        LocationServices.getLocationServices(this).addLocationListener(this);
      }
    } else {
      mapboxMap.setMyLocationEnabled(false);
    }
  }

  @Override
  public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
    if (requestCode == PERMISSIONS_LOCATION) {
      if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
        enableLocation(true);
      }
    }
  }

  @Override
  public void onLocationChanged(Location location) {
    if (mapboxMap != null) {
      mapboxMap.moveCamera(CameraUpdateFactory.newLatLngZoom(new LatLng(location), 14));
    }
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
    mapView.onDestroy();
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }
}
