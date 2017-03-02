package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.DrawableRes;
import android.support.v7.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.ViewToBitmapUtil;

import timber.log.Timber;

/**
 * Test activity showcasing updating a Marker image when changing zoom levels
 */
public class AddRemoveMarkerActivity extends AppCompatActivity {

  public static final double THRESHOLD = 5.0;

  private MapView mapView;
  private MapboxMap mapboxMap;
  private double lastZoom;
  private boolean isShowingHighThresholdMarker;
  private boolean isShowingLowThresholdMarker;

  private MarkerOptions lowThresholdMarker;
  private MarkerOptions highThresholdMarker;
  private Marker activeMarker;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_add_remove_marker);

    View lowThresholdView = generateView("Low", R.drawable.ic_circle);
    Bitmap lowThresholdBitmap = ViewToBitmapUtil.convertToBitmap(lowThresholdView);
    Icon lowThresholdIcon = IconFactory.getInstance(this).fromBitmap(lowThresholdBitmap);

    lowThresholdMarker = new MarkerOptions()
      .icon(lowThresholdIcon)
      .position(new LatLng(0.1, 0));

    View highThesholdView = generateView("High", R.drawable.ic_circle);
    Bitmap highThresholdBitmap = ViewToBitmapUtil.convertToBitmap(highThesholdView);
    Icon highThresholdIcon = IconFactory.getInstance(this).fromBitmap(highThresholdBitmap);

    highThresholdMarker = new MarkerOptions()
      .icon(highThresholdIcon)
      .position(new LatLng(0.1, 0));

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(MapboxMap mapboxMap) {
        AddRemoveMarkerActivity.this.mapboxMap = mapboxMap;
        updateZoom(mapboxMap.getCameraPosition().zoom);
        mapboxMap.moveCamera(CameraUpdateFactory.zoomTo(4.9f));
        mapboxMap.setOnCameraChangeListener(new MapboxMap.OnCameraChangeListener() {
          @Override
          public void onCameraChange(CameraPosition position) {
            updateZoom(position.zoom);
          }
        });
      }
    });
  }

  @SuppressLint("InflateParams")
  private View generateView(String text, @DrawableRes int drawableRes) {
    View view = LayoutInflater.from(this).inflate(R.layout.view_custom_marker, null);
    TextView textView = (TextView) view.findViewById(R.id.textView);
    textView.setText(text);
    textView.setTextColor(Color.WHITE);
    ImageView imageView = (ImageView) view.findViewById(R.id.imageView);
    imageView.setImageResource(drawableRes);
    return view;
  }

  private void updateZoom(double zoom) {
    if (lastZoom == zoom) {
      return;
    }

    lastZoom = zoom;
    if (zoom > THRESHOLD) {
      showHighThresholdMarker();
    } else {
      showLowThresholdMarker();
    }
  }

  private void showLowThresholdMarker() {
    if (isShowingLowThresholdMarker) {
      return;
    }

    isShowingLowThresholdMarker = true;
    isShowingHighThresholdMarker = false;

    if (activeMarker != null) {
      Timber.d("Remove marker with " + activeMarker.getId());
      mapboxMap.removeMarker(activeMarker);
    } else {
      Timber.e("active marker is null");
    }

    activeMarker = mapboxMap.addMarker(lowThresholdMarker);
    Timber.d("showLowThresholdMarker() " + activeMarker.getId());
  }

  private void showHighThresholdMarker() {
    if (isShowingHighThresholdMarker) {
      return;
    }

    isShowingLowThresholdMarker = false;
    isShowingHighThresholdMarker = true;

    if (activeMarker != null) {
      Timber.d("Remove marker with " + activeMarker.getId());
      mapboxMap.removeMarker(activeMarker);
    } else {
      Timber.e("active marker is null");
    }

    activeMarker = mapboxMap.addMarker(highThresholdMarker);
    Timber.d("showHighThresholdMarker() " + activeMarker.getId());
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
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
  }
}
