package com.mapbox.mapboxsdk.testapp.activity.infowindow;

import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;

import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.model.annotations.CityStateMarker;
import com.mapbox.mapboxsdk.testapp.model.annotations.CityStateMarkerOptions;
import com.mapbox.mapboxsdk.testapp.utils.IconUtils;

/**
 * Test activity showcasing using an InfoWindowAdapter to provide a custom InfoWindow content.
 */
public class InfoWindowAdapterActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_infowindow_adapter);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(map -> {
      mapboxMap = map;
      addMarkers();
      addCustomInfoWindowAdapter();
    });
  }

  private void addMarkers() {
    mapboxMap.addMarker(generateCityStateMarker("Andorra", 42.505777, 1.52529, "#F44336"));
    mapboxMap.addMarker(generateCityStateMarker("Luxembourg", 49.815273, 6.129583, "#3F51B5"));
    mapboxMap.addMarker(generateCityStateMarker("Monaco", 43.738418, 7.424616, "#673AB7"));
    mapboxMap.addMarker(generateCityStateMarker("Vatican City", 41.902916, 12.453389, "#009688"));
    mapboxMap.addMarker(generateCityStateMarker("San Marino", 43.942360, 12.457777, "#795548"));
    mapboxMap.addMarker(generateCityStateMarker("Liechtenstein", 47.166000, 9.555373, "#FF5722"));
  }

  private CityStateMarkerOptions generateCityStateMarker(String title, double lat, double lng, String color) {
    CityStateMarkerOptions marker = new CityStateMarkerOptions();
    marker.title(title);
    marker.position(new LatLng(lat, lng));
    marker.infoWindowBackground(color);

    Icon icon = IconUtils.drawableToIcon(this, R.drawable.ic_location_city, Color.parseColor(color));
    marker.icon(icon);
    return marker;
  }

  private void addCustomInfoWindowAdapter() {
    mapboxMap.setInfoWindowAdapter(new MapboxMap.InfoWindowAdapter() {

      private int tenDp = (int) getResources().getDimension(R.dimen.attr_margin);

      @Override
      public View getInfoWindow(@NonNull Marker marker) {
        TextView textView = new TextView(InfoWindowAdapterActivity.this);
        textView.setText(marker.getTitle());
        textView.setTextColor(Color.WHITE);

        if (marker instanceof CityStateMarker) {
          CityStateMarker cityStateMarker = (CityStateMarker) marker;
          textView.setBackgroundColor(Color.parseColor(cityStateMarker.getInfoWindowBackgroundColor()));
        }

        textView.setPadding(tenDp, tenDp, tenDp, tenDp);
        return textView;
      }
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
    mapView.onDestroy();
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }
}
