package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.geojson.Point;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.widgets.CompassView;
import com.mapbox.mapboxsdk.style.expressions.Expression;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.style.layers.PropertyFactory;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;
import java.util.List;

/**
 * Test activity showcasing a simple MapView without any MapboxMap interaction.
 */
public class SimpleMapActivity extends AppCompatActivity {

  private MapView mapView;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_simple);

    mapView = (MapView) findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(new OnMapReadyCallback() {
      @Override
      public void onMapReady(MapboxMap mapboxMap) {
        List<Feature> features1 = new ArrayList<>();
        features1.add(Feature.fromGeometry(Point.fromLngLat(10, 50)));

        List<Feature> features2 = new ArrayList<>();
        features2.add(Feature.fromGeometry(Point.fromLngLat(11, 50)));

        FeatureCollection featureCollection1 = FeatureCollection.fromFeatures(features1);
        FeatureCollection featureCollection2 = FeatureCollection.fromFeatures(features2);

        GeoJsonSource source1 = new GeoJsonSource("source1", featureCollection1);
        GeoJsonSource source2 = new GeoJsonSource("source2", featureCollection2);
        mapboxMap.addSource(source1);
        mapboxMap.addSource(source2);

        Layer layer1 = new SymbolLayer("layer1", "source1").withProperties(PropertyFactory.iconImage("restaurant-15"), PropertyFactory.iconAllowOverlap(true), PropertyFactory.iconAnchor(Expression.literal(Property.ICON_ANCHOR_CENTER)));
        Layer layer2 = new SymbolLayer("layer2", "source2").withProperties(PropertyFactory.iconImage("restaurant-15"), PropertyFactory.iconAllowOverlap(true), PropertyFactory.iconAnchor(Expression.literal(Property.ICON_ANCHOR_CENTER)));
        mapboxMap.addLayer(layer1);
        mapboxMap.addLayer(layer2);

        new Handler().postDelayed(new Runnable() {
          @Override
          public void run() {
            featureCollection2.features().addAll(featureCollection1.features());
            featureCollection1.features().clear();
            source1.setGeoJson(featureCollection1);
            source2.setGeoJson(featureCollection2);
          }
        }, 5000);
        /*Expression.get("imageAttribute");
        mapboxMap.addOnCameraIdleListener(() ->
          Timber.e("Camera IDLE: %s, zoom: %f", mapboxMap.getCameraPosition().target, mapboxMap.getCameraPosition().zoom));

        //crossing IDL
        LatLngBounds latLngBounds = LatLngBounds.from(16.5, -172.8, -35.127709, 172.6);
        LatLng ne = new LatLng(16.5, -172.8);
        LatLng sw = new LatLng(-35.127709, 172.6);

        if (ne.getLongitude() < sw.getLongitude()) {
          if (ne.getLongitude() < 0 && sw.getLongitude() > 0) {
            sw.setLongitude(sw.getLongitude() - 360);
          } else if (ne.getLongitude() > 0 && sw.getLongitude() < 0) {
            sw.setLongitude(sw.getLongitude() + 360);
          }
        }

        LatLngBounds betterBounds = new LatLngBounds.Builder().include(ne).include(sw).build();
        CameraPosition cameraPosition = mapboxMap.getCameraForLatLngBounds(betterBounds, new int[] {0, 0, 0, 0});
        mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
        //mapboxMap.setLatLngBoundsForCameraTarget(latLngBounds);

        //not crossing IDL
        new Handler().postDelayed(() -> {
          LatLngBounds latLngBounds1 = LatLngBounds.from(16.5, -162.8, -35.127709, -177.4);
//          mapboxMap.setLatLngBoundsForCameraTarget(latLngBounds1);
          CameraPosition cameraPosition1 = mapboxMap.getCameraForLatLngBounds(latLngBounds1, new int[] {0, 0, 0, 0});
          //mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(cameraPosition1));
        }, 2500);*/
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
}
