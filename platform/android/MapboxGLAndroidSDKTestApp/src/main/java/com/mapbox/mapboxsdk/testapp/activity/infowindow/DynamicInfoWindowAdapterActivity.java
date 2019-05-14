package com.mapbox.mapboxsdk.testapp.activity.infowindow;

import android.graphics.Color;
import android.os.Bundle;
import androidx.annotation.NonNull;
import androidx.core.content.res.ResourcesCompat;
import androidx.appcompat.app.AppCompatActivity;
import android.widget.TextView;

import com.mapbox.mapboxsdk.annotations.InfoWindow;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.IconUtils;

import java.util.Locale;

/**
 * Test activity showcasing how to dynamically update InfoWindow when Using an MapboxMap.InfoWindowAdapter.
 */
public class DynamicInfoWindowAdapterActivity extends AppCompatActivity implements OnMapReadyCallback {

  private static final LatLng PARIS = new LatLng(48.864716, 2.349014);

  private MapboxMap mapboxMap;
  private MapView mapView;
  private Marker marker;

  private MapboxMap.OnMapClickListener mapClickListener = new MapboxMap.OnMapClickListener() {
    @Override
    public boolean onMapClick(@NonNull LatLng point) {
      if (marker == null) {
        return false;
      }

      // Distance from click to marker
      double distanceKm = marker.getPosition().distanceTo(point) / 1000;

      // Get the info window
      final InfoWindow infoWindow = marker.getInfoWindow();

      // Get the view from the info window
      if (infoWindow != null && infoWindow.getView() != null) {
        // Set the new text on the text view in the info window
        TextView textView = (TextView) infoWindow.getView();
        textView.setText(String.format(Locale.getDefault(), "%.2fkm", distanceKm));
        // Update the info window position (as the text length changes)
        textView.post(infoWindow::update);
      }
      return true;
    }
  };

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_infowindow_adapter);

    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
  }

  @Override
  public void onMapReady(@NonNull MapboxMap map) {
    mapboxMap = map;

    map.setStyle(Style.MAPBOX_STREETS);

    // Add info window adapter
    addCustomInfoWindowAdapter(mapboxMap);

    // Keep info windows open on click
    mapboxMap.getUiSettings().setDeselectMarkersOnTap(false);

    // Add a marker
    marker = addMarker(mapboxMap);
    mapboxMap.selectMarker(marker);

    // On map click, change the info window contents
    mapboxMap.addOnMapClickListener(mapClickListener);

    // Focus on Paris
    mapboxMap.animateCamera(CameraUpdateFactory.newLatLng(PARIS));
  }

  private Marker addMarker(MapboxMap mapboxMap) {
    return mapboxMap.addMarker(
      new MarkerOptions()
        .position(PARIS)
        .icon(IconUtils.drawableToIcon(this, R.drawable.ic_location_city,
          ResourcesCompat.getColor(getResources(), R.color.mapbox_blue, getTheme()))
        ));
  }

  private void addCustomInfoWindowAdapter(final MapboxMap mapboxMap) {
    final int padding = (int) getResources().getDimension(R.dimen.attr_margin);
    mapboxMap.setInfoWindowAdapter(marker -> {
      TextView textView = new TextView(DynamicInfoWindowAdapterActivity.this);
      textView.setText(marker.getTitle());
      textView.setBackgroundColor(Color.WHITE);
      textView.setText(R.string.action_calculate_distance);
      textView.setPadding(padding, padding, padding, padding);
      return textView;
    });
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
    if (mapboxMap != null) {
      mapboxMap.removeOnMapClickListener(mapClickListener);
    }
    mapView.onDestroy();
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }
}
