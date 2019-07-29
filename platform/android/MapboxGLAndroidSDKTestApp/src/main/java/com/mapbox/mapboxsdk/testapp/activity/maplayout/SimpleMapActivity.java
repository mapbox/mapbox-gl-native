package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.MenuItem;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.mapbox.mapboxsdk.log.Logger;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.Mapbox;
import com.mapbox.mapboxsdk.offline.OfflineRegion;
import com.mapbox.mapboxsdk.offline.OfflineManager;//.CreateOfflineRegionCallback;
import com.mapbox.mapboxsdk.offline.OfflineRegionError;
import com.mapbox.mapboxsdk.offline.OfflineRegionStatus;
import com.mapbox.mapboxsdk.offline.OfflineTilePyramidRegionDefinition;
import com.mapbox.mapboxsdk.testapp.utils.OfflineUtils;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.NavUtils;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;

/**
 * Test activity showcasing a simple MapView without any MapboxMap interaction.
 */
public class SimpleMapActivity extends AppCompatActivity {

  private MapView mapView;
  private OfflineRegion offlineRegion;
  private TextView tv;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    Mapbox.setConnected(false);
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_simple);
    mapView = findViewById(R.id.mapView);
    mapView.onCreate(savedInstanceState);

    tv = new TextView(this);
    ((RelativeLayout) mapView.getParent()).addView(tv);

    OfflineManager offlineManager = OfflineManager.getInstance(this);
    offlineManager.listOfflineRegions(new OfflineManager.ListOfflineRegionsCallback() {
      @Override
      public void onList(OfflineRegion[] offlineRegions) {
        if (offlineRegions.length > 0) {
          tv.setText("region present");
          Logger.d("OfflineTest", "region present");
          mapView.getMapAsync(mapboxMap -> mapboxMap.setOfflineRegionDefinition(offlineRegions[0].getDefinition()));
        } else {
          LatLngBounds bounds = new LatLngBounds.Builder()
            .include(new LatLng(26.644858, 118.17))
            .include(new LatLng(26.63, 118.181632))
            .build();
          double minZoom = 14;
          double maxZoom = 17;

          float pixelRatio = SimpleMapActivity.this.getResources().getDisplayMetrics().density;
          OfflineTilePyramidRegionDefinition definition = new OfflineTilePyramidRegionDefinition(
            Style.MAPBOX_STREETS, bounds, minZoom, maxZoom, pixelRatio, false);

          byte[] metadata = OfflineUtils.convertRegionName("test_region");
          offlineManager.createOfflineRegion(definition, metadata, new OfflineManager.CreateOfflineRegionCallback() {
            @Override
            public void onCreate(OfflineRegion offlineRegion) {
              Logger.d("OfflineTest", "starting download");
              SimpleMapActivity.this.offlineRegion = offlineRegion;
              Mapbox.setConnected(null);
              launchDownload();
            }

            @Override
            public void onError(String error) {
            }
          });
        }
      }

      @Override
      public void onError(String error) {
      }
    });
  }

  private void launchDownload() {
    offlineRegion.setObserver(new OfflineRegion.OfflineRegionObserver() {
      @Override
      public void onStatusChanged(OfflineRegionStatus status) {
        tv.setText(status.getCompletedResourceCount() + "/" + status.getRequiredResourceCount());
        Logger.d("OfflineTest", status.getCompletedResourceCount() + "/" + status.getRequiredResourceCount());
        if (status.isComplete()) {
          offlineRegion.setDownloadState(OfflineRegion.STATE_INACTIVE);
          offlineRegion.setObserver(null);
          Mapbox.setConnected(false);
          Toast.makeText(SimpleMapActivity.this, "Region downloaded", Toast.LENGTH_SHORT).show();
          mapView.getMapAsync(mapboxMap -> mapboxMap.setOfflineRegionDefinition(offlineRegion.getDefinition()));
        }
      }

      @Override
      public void onError(OfflineRegionError error) {

      }

      @Override
      public void mapboxTileCountLimitExceeded(long limit) {
      }
    });

    offlineRegion.setDownloadState(OfflineRegion.STATE_ACTIVE);
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
    Mapbox.setConnected(true);
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }
}
