package com.mapbox.mapboxsdk.testapp.activity.camera;

import android.graphics.Typeface;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.ColorInt;
import android.support.annotation.IntDef;
import android.support.annotation.Nullable;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.mapbox.android.gestures.AndroidGesturesManager;
import com.mapbox.android.gestures.MoveGestureDetector;
import com.mapbox.android.gestures.RotateGestureDetector;
import com.mapbox.android.gestures.ShoveGestureDetector;
import com.mapbox.android.gestures.StandardScaleGestureDetector;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.FontCache;
import com.mapbox.mapboxsdk.testapp.utils.ResourceUtils;

import java.lang.annotation.Retention;
import java.util.ArrayList;
import java.util.List;

import static java.lang.annotation.RetentionPolicy.SOURCE;

/**
 * Test activity showcasing APIs around gestures implementation.
 */
public class GestureDetectorActivity extends AppCompatActivity {

  private static final int MAX_NUMBER_OF_ALERTS = 30;

  private MapView mapView;
  private MapboxMap mapboxMap;
  private RecyclerView recyclerView;
  private GestureAlertsAdapter gestureAlertsAdapter;

  private AndroidGesturesManager gesturesManager;

  @Nullable
  private Marker marker;
  @Nullable
  private LatLng focalPointLatLng;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_gesture_detector);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(MapboxMap mapboxMap) {
        GestureDetectorActivity.this.mapboxMap = mapboxMap;
        initializeMap();
      }
    });

    recyclerView = (RecyclerView) findViewById(R.id.alerts_recycler);
    recyclerView.setLayoutManager(new LinearLayoutManager(this));

    gestureAlertsAdapter = new GestureAlertsAdapter();
    recyclerView.setAdapter(gestureAlertsAdapter);
  }

  @Override
  protected void onResume() {
    super.onResume();
    mapView.onResume();
  }

  @Override
  protected void onPause() {
    super.onPause();
    gestureAlertsAdapter.cancelUpdates();
    mapView.onPause();
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
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
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

  private void initializeMap() {
    gesturesManager = mapboxMap.getGesturesManager();

    RelativeLayout.LayoutParams layoutParams = (RelativeLayout.LayoutParams) recyclerView.getLayoutParams();
    layoutParams.height = (int) (mapView.getHeight() / 1.75);
    layoutParams.width = (mapView.getWidth() / 3);
    recyclerView.setLayoutParams(layoutParams);

    attachListeners();
  }

  public void attachListeners() {
    mapboxMap.addOnMoveListener(new MapboxMap.OnMoveListener() {
      @Override
      public void onMoveBegin(MoveGestureDetector detector) {
        gestureAlertsAdapter.addAlert(new GestureAlert(GestureAlert.TYPE_START, "MOVE START"));
      }

      @Override
      public void onMove(MoveGestureDetector detector) {
        gestureAlertsAdapter.addAlert(new GestureAlert(GestureAlert.TYPE_PROGRESS, "MOVE PROGRESS"));
      }

      @Override
      public void onMoveEnd(MoveGestureDetector detector) {
        gestureAlertsAdapter.addAlert(new GestureAlert(GestureAlert.TYPE_END, "MOVE END"));
      }
    });

    mapboxMap.addOnRotateListener(new MapboxMap.OnRotateListener() {
      @Override
      public void onRotateBegin(RotateGestureDetector detector) {
        gestureAlertsAdapter.addAlert(new GestureAlert(GestureAlert.TYPE_START, "ROTATE START"));
      }

      @Override
      public void onRotate(RotateGestureDetector detector) {
        gestureAlertsAdapter.addAlert(new GestureAlert(GestureAlert.TYPE_PROGRESS, "ROTATE PROGRESS"));
        recalculateFocalPoint();
      }

      @Override
      public void onRotateEnd(RotateGestureDetector detector) {
        gestureAlertsAdapter.addAlert(new GestureAlert(GestureAlert.TYPE_END, "ROTATE END"));
      }
    });

    mapboxMap.addOnScaleListener(new MapboxMap.OnScaleListener() {
      @Override
      public void onScaleBegin(StandardScaleGestureDetector detector) {
        gestureAlertsAdapter.addAlert(new GestureAlert(GestureAlert.TYPE_START, "SCALE START"));
        if (focalPointLatLng != null) {
          gestureAlertsAdapter.addAlert(new GestureAlert(GestureAlert.TYPE_OTHER, "INCREASING MOVE THRESHOLD"));
          gesturesManager.getMoveGestureDetector().setMoveThreshold(
            ResourceUtils.convertDpToPx(GestureDetectorActivity.this, 175));

          gestureAlertsAdapter.addAlert(new GestureAlert(GestureAlert.TYPE_OTHER, "MANUALLY INTERRUPTING MOVE"));
          gesturesManager.getMoveGestureDetector().interrupt();
        }
        recalculateFocalPoint();
      }

      @Override
      public void onScale(StandardScaleGestureDetector detector) {
        gestureAlertsAdapter.addAlert(new GestureAlert(GestureAlert.TYPE_PROGRESS, "SCALE PROGRESS"));
      }

      @Override
      public void onScaleEnd(StandardScaleGestureDetector detector) {
        gestureAlertsAdapter.addAlert(new GestureAlert(GestureAlert.TYPE_END, "SCALE END"));

        if (focalPointLatLng != null) {
          gestureAlertsAdapter.addAlert(new GestureAlert(GestureAlert.TYPE_OTHER, "REVERTING MOVE THRESHOLD"));
          gesturesManager.getMoveGestureDetector().setMoveThreshold(0f);
        }
      }
    });

    mapboxMap.addOnShoveListener(new MapboxMap.OnShoveListener() {
      @Override
      public void onShoveBegin(ShoveGestureDetector detector) {
        gestureAlertsAdapter.addAlert(new GestureAlert(GestureAlert.TYPE_START, "SHOVE START"));
      }

      @Override
      public void onShove(ShoveGestureDetector detector) {
        gestureAlertsAdapter.addAlert(new GestureAlert(GestureAlert.TYPE_PROGRESS, "SHOVE PROGRESS"));
      }

      @Override
      public void onShoveEnd(ShoveGestureDetector detector) {
        gestureAlertsAdapter.addAlert(new GestureAlert(GestureAlert.TYPE_END, "SHOVE END"));
      }
    });
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_gestures, menu);
    return true;
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    resetModes();
    switch (item.getItemId()) {
      case R.id.menu_gesture_none:
        return true;
      case R.id.menu_gesture_focus_point:
        focalPointLatLng = new LatLng(51.50325, -0.12968);
        marker = mapboxMap.addMarker(new MarkerOptions().position(focalPointLatLng));
        mapboxMap.easeCamera(CameraUpdateFactory.newLatLngZoom(focalPointLatLng, 16));
        mapboxMap.getUiSettings().setFocalPoint(mapboxMap.getProjection().toScreenLocation(focalPointLatLng));
        return true;
      case R.id.menu_gesture_animation:
        mapboxMap.getUiSettings().setAllVelocityAnimationsEnabled(false);
    }
    return super.onOptionsItemSelected(item);
  }

  private void resetModes() {
    focalPointLatLng = null;
    mapboxMap.getUiSettings().setFocalPoint(null);
    gesturesManager.getMoveGestureDetector().setMoveThreshold(0f);
    mapboxMap.getUiSettings().setAllVelocityAnimationsEnabled(true);

    if (marker != null) {
      mapboxMap.removeMarker(marker);
      marker = null;
    }
  }

  private void recalculateFocalPoint() {
    if (focalPointLatLng != null) {
      mapboxMap.getUiSettings().setFocalPoint(
        mapboxMap.getProjection().toScreenLocation(focalPointLatLng)
      );
    }
  }

  private static class GestureAlertsAdapter extends RecyclerView.Adapter<GestureAlertsAdapter.ViewHolder> {

    private boolean isUpdating;
    private final Handler updateHandler = new Handler();
    private final List<GestureAlert> alerts = new ArrayList<>();

    public static class ViewHolder extends RecyclerView.ViewHolder {

      TextView alertMessageTv;

      @ColorInt
      public int textColor;

      ViewHolder(View view) {
        super(view);
        Typeface typeface = FontCache.get("Roboto-Regular.ttf", view.getContext());
        alertMessageTv = (TextView) view.findViewById(R.id.alert_message);
        alertMessageTv.setTypeface(typeface);
      }
    }


    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
      View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.item_gesture_alert, parent, false);
      return new ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
      GestureAlert alert = alerts.get(position);
      holder.alertMessageTv.setText(alert.getMessage());
      holder.alertMessageTv.setTextColor(
        ContextCompat.getColor(holder.alertMessageTv.getContext(), alert.getColor()));
    }

    @Override
    public int getItemCount() {
      return alerts.size();
    }

    void addAlert(GestureAlert alert) {
      for (GestureAlert gestureAlert : alerts) {
        if (gestureAlert.getAlertType() != GestureAlert.TYPE_PROGRESS) {
          break;
        }

        if (alert.getAlertType() == GestureAlert.TYPE_PROGRESS && gestureAlert.equals(alert)) {
          return;
        }
      }

      if (getItemCount() >= MAX_NUMBER_OF_ALERTS) {
        alerts.remove(getItemCount() - 1);
      }

      alerts.add(0, alert);
      if (!isUpdating) {
        isUpdating = true;
        updateHandler.postDelayed(updateRunnable, 250);
      }
    }

    private Runnable updateRunnable = new Runnable() {
      @Override
      public void run() {
        notifyDataSetChanged();
        isUpdating = false;
      }
    };

    void cancelUpdates() {
      updateHandler.removeCallbacksAndMessages(null);
    }
  }

  private static class GestureAlert {
    @Retention(SOURCE)
    @IntDef( {TYPE_NONE, TYPE_START, TYPE_PROGRESS, TYPE_END, TYPE_OTHER})
    @interface Type {
    }

    static final int TYPE_NONE = 0;
    static final int TYPE_START = 1;
    static final int TYPE_END = 2;
    static final int TYPE_PROGRESS = 3;
    static final int TYPE_OTHER = 4;

    @Type
    private int alertType;

    private String message;

    @ColorInt
    private int color;

    GestureAlert(@Type int alertType, String message) {
      this.alertType = alertType;
      this.message = message;

      switch (alertType) {
        case TYPE_NONE:
          color = android.R.color.black;
          break;
        case TYPE_END:
          color = android.R.color.holo_red_dark;
          break;
        case TYPE_OTHER:
          color = android.R.color.holo_purple;
          break;
        case TYPE_PROGRESS:
          color = android.R.color.holo_orange_dark;
          break;
        case TYPE_START:
          color = android.R.color.holo_green_dark;
          break;
      }
    }

    int getAlertType() {
      return alertType;
    }

    String getMessage() {
      return message;
    }

    int getColor() {
      return color;
    }

    @Override
    public boolean equals(Object o) {
      if (this == o) {
        return true;
      }
      if (o == null || getClass() != o.getClass()) {
        return false;
      }

      GestureAlert that = (GestureAlert) o;

      if (alertType != that.alertType) {
        return false;
      }
      return message != null ? message.equals(that.message) : that.message == null;
    }

    @Override
    public int hashCode() {
      int result = alertType;
      result = 31 * result + (message != null ? message.hashCode() : 0);
      return result;
    }
  }
}
