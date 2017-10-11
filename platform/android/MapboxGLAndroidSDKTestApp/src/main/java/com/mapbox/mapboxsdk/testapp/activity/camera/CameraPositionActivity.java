package com.mapbox.mapboxsdk.testapp.activity.camera;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.annotation.IdRes;
import android.support.annotation.NonNull;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import timber.log.Timber;

/**
 * Test activity showcasing how to listen to camera change events.
 */
public class CameraPositionActivity extends AppCompatActivity implements OnMapReadyCallback {

  private MapView mapView;
  private MapboxMap mapboxMap;
  private FloatingActionButton fab;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_camera_position);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
  }

  @Override
  public void onMapReady(@NonNull final MapboxMap map) {
    mapboxMap = map;

    mapboxMap.setOnCameraIdleListener(new MapboxMap.OnCameraIdleListener() {
      @Override
      public void onCameraIdle() {
        Timber.e("OnCameraIdle");
        fab.setColorFilter(ContextCompat.getColor(CameraPositionActivity.this, android.R.color.holo_green_dark));
      }
    });

    mapboxMap.setOnCameraMoveCancelListener(new MapboxMap.OnCameraMoveCanceledListener() {
      @Override
      public void onCameraMoveCanceled() {
        Timber.e("OnCameraMoveCanceled");
      }
    });

    mapboxMap.setOnCameraMoveListener(new MapboxMap.OnCameraMoveListener() {
      @Override
      public void onCameraMove() {
        Timber.e("OnCameraMove");
        fab.setColorFilter(ContextCompat.getColor(CameraPositionActivity.this, android.R.color.holo_orange_dark));
      }
    });

    mapboxMap.setOnCameraMoveStartedListener(new MapboxMap.OnCameraMoveStartedListener() {

      private final String[] REASONS = {"REASON_API_GESTURE", "REASON_DEVELOPER_ANIMATION", "REASON_API_ANIMATION"};

      @Override
      public void onCameraMoveStarted(int reason) {
        // reason ranges from 1 <-> 3
        fab.setColorFilter(ContextCompat.getColor(CameraPositionActivity.this, android.R.color.holo_red_dark));
        Timber.e("OnCameraMoveStarted: %s", REASONS[reason - 1]);
      }
    });

    // add a listener to FAB
    fab = (FloatingActionButton) findViewById(R.id.fab);
    fab.setColorFilter(ContextCompat.getColor(CameraPositionActivity.this, R.color.primary));
    fab.setOnClickListener(new View.OnClickListener() {
      @SuppressLint("InflateParams")
      @Override
      public void onClick(View view) {
        Context context = view.getContext();
        final View dialogContent = LayoutInflater.from(context).inflate(R.layout.dialog_camera_position, null);
        AlertDialog.Builder builder = new AlertDialog.Builder(
          context, com.mapbox.mapboxsdk.R.style.mapbox_AlertDialogStyle);
        builder.setTitle(R.string.dialog_camera_position);
        builder.setView(onInflateDialogContent(dialogContent));
        builder.setPositiveButton("Animate", new DialogInterface.OnClickListener() {
          @Override
          public void onClick(DialogInterface dialog, int which) {
            double latitude = Double.parseDouble(
              ((TextView) dialogContent.findViewById(R.id.value_lat)).getText().toString());
            double longitude = Double.parseDouble(
              ((TextView) dialogContent.findViewById(R.id.value_lon)).getText().toString());
            double zoom = Double.parseDouble(
              ((TextView) dialogContent.findViewById(R.id.value_zoom)).getText().toString());
            double bearing = Double.parseDouble(
              ((TextView) dialogContent.findViewById(R.id.value_bearing)).getText().toString());
            double tilt = Double.parseDouble(
              ((TextView) dialogContent.findViewById(R.id.value_tilt)).getText().toString());

            CameraPosition cameraPosition = new CameraPosition.Builder()
              .target(new LatLng(latitude, longitude))
              .zoom(zoom)
              .bearing(bearing)
              .tilt(tilt)
              .build();

            mapboxMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition), 5000,
              new MapboxMap.CancelableCallback() {
                @Override
                public void onCancel() {
                  Timber.v("OnCancel called");
                }

                @Override
                public void onFinish() {
                  Timber.v("OnFinish called");
                }
              });
            Timber.v(cameraPosition.toString());
          }
        });
        builder.setNegativeButton("Cancel", null);
        builder.setCancelable(false);
        builder.show();
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
  protected void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }

  private View onInflateDialogContent(View view) {
    linkTextView(view, R.id.value_lat, R.id.seekbar_lat, new LatLngChangeListener(), 180 + 38);
    linkTextView(view, R.id.value_lon, R.id.seekbar_lon, new LatLngChangeListener(), 180 - 77);
    linkTextView(view, R.id.value_zoom, R.id.seekbar_zoom, new ValueChangeListener(), 6);
    linkTextView(view, R.id.value_bearing, R.id.seekbar_bearing, new ValueChangeListener(), 90);
    linkTextView(view, R.id.value_tilt, R.id.seekbar_tilt, new ValueChangeListener(), 40);
    return view;
  }

  private void linkTextView(
    View view, @IdRes int textViewRes, @IdRes int seekBarRes, ValueChangeListener listener, int defaultValue) {
    final TextView value = (TextView) view.findViewById(textViewRes);
    SeekBar seekBar = (SeekBar) view.findViewById(seekBarRes);
    listener.setLinkedValueView(value);
    seekBar.setOnSeekBarChangeListener(listener);
    seekBar.setProgress(defaultValue);
  }

  private class ValueChangeListener implements SeekBar.OnSeekBarChangeListener {

    protected TextView textView;

    public void setLinkedValueView(TextView textView) {
      this.textView = textView;
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
      textView.setText(String.valueOf(progress));
    }
  }

  private class LatLngChangeListener extends ValueChangeListener {

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
      super.onProgressChanged(seekBar, progress - 180, fromUser);
    }
  }
}
