package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import timber.log.Timber;

/**
 * Test Activity showcasing the different debug modes and allows to cycle between the default map styles.
 */
public class DebugModeActivity extends AppCompatActivity {

  private MapView mapView;
  private MapboxMap mapboxMap;

  private int currentStyleIndex = 0;

  private static final String[] STYLES = new String[] {
    Style.MAPBOX_STREETS,
    Style.OUTDOORS,
    Style.LIGHT,
    Style.DARK,
    Style.SATELLITE,
    Style.SATELLITE_STREETS
  };

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_debug_mode);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.setTag(true);
    mapView.setStyleUrl(STYLES[currentStyleIndex]);
    mapView.onCreate(savedInstanceState);

    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(@NonNull MapboxMap map) {
        mapboxMap = map;

        mapboxMap.getUiSettings().setZoomControlsEnabled(true);

        // show current zoom level on screen
        final TextView textView = (TextView) findViewById(R.id.textZoom);
        mapboxMap.setOnCameraChangeListener(new MapboxMap.OnCameraChangeListener() {
          @Override
          public void onCameraChange(CameraPosition position) {
            textView.setText(String.format(getString(R.string.debug_zoom), position.zoom));
          }
        });
      }
    });


    FloatingActionButton fabDebug = (FloatingActionButton) findViewById(R.id.fabDebug);
    fabDebug.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view) {
        if (mapboxMap != null) {
          Timber.d("Debug FAB: isDebug Active? " + mapboxMap.isDebugActive());
          mapboxMap.cycleDebugOptions();
        }
      }
    });

    FloatingActionButton fabStyles = (FloatingActionButton) findViewById(R.id.fabStyles);
    fabStyles.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view) {
        if (mapboxMap != null) {
          currentStyleIndex++;
          if (currentStyleIndex == STYLES.length) {
            currentStyleIndex = 0;
          }
          mapboxMap.setStyleUrl(STYLES[currentStyleIndex], new MapboxMap.OnStyleLoadedListener() {
            @Override
            public void onStyleLoaded(String style) {
              Timber.d("Style loaded %s", style);
            }
          });
        }
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
