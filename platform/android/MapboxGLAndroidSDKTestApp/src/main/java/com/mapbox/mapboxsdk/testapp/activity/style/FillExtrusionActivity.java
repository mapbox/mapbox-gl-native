package com.mapbox.mapboxsdk.testapp.activity.style;

import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.style.layers.FillExtrusionLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.services.commons.geojson.Polygon;

import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillExtrusionColor;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillExtrusionHeight;
import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.fillExtrusionOpacity;

/**
 * Test activity showcasing fill extrusions
 */
public class FillExtrusionActivity extends AppCompatActivity {

  private MapboxMap mapboxMap;
  private MapView mapView;

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_fill_extrusion_layer);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(@NonNull
                             final MapboxMap map) {
        mapboxMap = map;
        Polygon domTower = Polygon.fromCoordinates(new double[][][] {
          new double[][] {
            new double[] {
              5.12112557888031,
              52.09071040847704
            },
            new double[] {
              5.121227502822875,
              52.09053901776669
            },
            new double[] {
              5.121484994888306,
              52.090601641371805
            },
            new double[] {
              5.1213884353637695,
              52.090766439912635
            },
            new double[] {
              5.12112557888031,
              52.09071040847704
            }
          }
        });

        GeoJsonSource source = new GeoJsonSource("extrusion-source", domTower);
        mapboxMap.addSource(source);

        mapboxMap.addLayer(
          new FillExtrusionLayer("extrusion-layer", source.getId())
            .withProperties(
              fillExtrusionHeight(40f),
              fillExtrusionOpacity(0.5f),
              fillExtrusionColor(Color.RED)
            )
        );

//        map.animateCamera(
//          CameraUpdateFactory.newCameraPosition(
//            new CameraPosition.Builder()
//              .target(new LatLng(52.09071040847704, 5.12112557888031))
//              .tilt(45.0)
//              .zoom(18)
//              .build()
//          ),
//          10000
//        );
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
  public void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  @Override
  public void onDestroy() {
    super.onDestroy();
    mapView.onDestroy();
  }

}
