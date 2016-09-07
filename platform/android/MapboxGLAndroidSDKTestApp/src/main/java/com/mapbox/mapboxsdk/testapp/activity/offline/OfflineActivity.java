package com.mapbox.mapboxsdk.testapp.activity.offline;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.text.TextUtils;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.Toast;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.offline.OfflineManager;
import com.mapbox.mapboxsdk.offline.OfflineRegion;
import com.mapbox.mapboxsdk.offline.OfflineRegionError;
import com.mapbox.mapboxsdk.offline.OfflineRegionStatus;
import com.mapbox.mapboxsdk.offline.OfflineTilePyramidRegionDefinition;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.model.other.OfflineDownloadRegionDialog;
import com.mapbox.mapboxsdk.testapp.model.other.OfflineListRegionsDialog;

import org.json.JSONObject;

import java.util.ArrayList;

public class OfflineActivity extends AppCompatActivity
        implements OfflineDownloadRegionDialog.DownloadRegionDialogListener {

    private final static String LOG_TAG = "OfflineActivity";

    // JSON encoding/decoding
    public final static String JSON_CHARSET = "UTF-8";
    public final static String JSON_FIELD_REGION_NAME = "FIELD_REGION_NAME";

    /*
     * UI elements
     */
    private MapView mMapView;
    private MapboxMap mapboxMap;
    private ProgressBar mProgressBar;
    private Button downloadRegion;
    private Button listRegions;

    private boolean isEndNotified;

    /*
     * Offline objects
     */
    private OfflineManager mOfflineManager;
    private OfflineRegion mOfflineRegion;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_offline);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        // Set up map
        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.setStyleUrl(Style.MAPBOX_STREETS);
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                Log.d(LOG_TAG, "Map is ready");
                OfflineActivity.this.mapboxMap = mapboxMap;

                // Set initial position to UNHQ in NYC
                mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(
                        new CameraPosition.Builder()
                                .target(new LatLng(40.749851, -73.967966))
                                .zoom(14)
                                .bearing(0)
                                .tilt(0)
                                .build()));
            }
        });

        // The progress bar
        mProgressBar = (ProgressBar) findViewById(R.id.progress_bar);

        // Set up button listeners
        downloadRegion = (Button) findViewById(R.id.button_download_region);
        downloadRegion.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                handleDownloadRegion();
            }
        });

        listRegions = (Button) findViewById(R.id.button_list_regions);
        listRegions.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                handleListRegions();
            }
        });

        // Set up the OfflineManager
        mOfflineManager = OfflineManager.getInstance(this);
    }

    @Override
    public void onResume() {
        super.onResume();
        mMapView.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        mMapView.onPause();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        mMapView.onSaveInstanceState(outState);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMapView.onDestroy();
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mMapView.onLowMemory();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    /*
     * Buttons logic
     */

    private void handleDownloadRegion() {
        Log.d(LOG_TAG, "handleDownloadRegion");

        // Show dialog
        OfflineDownloadRegionDialog offlineDownloadRegionDialog = new OfflineDownloadRegionDialog();
        offlineDownloadRegionDialog.show(getSupportFragmentManager(), "download");
    }

    private void handleListRegions() {
        Log.d(LOG_TAG, "handleListRegions");

        // Query the DB asynchronously
        mOfflineManager.listOfflineRegions(new OfflineManager.ListOfflineRegionsCallback() {
            @Override
            public void onList(OfflineRegion[] offlineRegions) {
                // Check result
                if (offlineRegions == null || offlineRegions.length == 0) {
                    Toast.makeText(OfflineActivity.this, "You have no regions yet.", Toast.LENGTH_SHORT).show();
                    return;
                }

                // Get regions info
                ArrayList<String> offlineRegionsNames = new ArrayList<>();
                for (OfflineRegion offlineRegion : offlineRegions) {
                    offlineRegionsNames.add(getRegionName(offlineRegion));
                }

                // Create args
                Bundle args = new Bundle();
                args.putStringArrayList(OfflineListRegionsDialog.ITEMS, offlineRegionsNames);

                // Show dialog
                OfflineListRegionsDialog offlineListRegionsDialog = new OfflineListRegionsDialog();
                offlineListRegionsDialog.setArguments(args);
                offlineListRegionsDialog.show(getSupportFragmentManager(), "list");
            }

            @Override
            public void onError(String error) {
                Log.e(LOG_TAG, "Error: " + error);
            }

            private String getRegionName(OfflineRegion offlineRegion) {
                String regionName;

                try {
                    byte[] metadata = offlineRegion.getMetadata();
                    String json = new String(metadata, JSON_CHARSET);
                    JSONObject jsonObject = new JSONObject(json);
                    regionName = jsonObject.getString(JSON_FIELD_REGION_NAME);
                } catch (Exception e) {
                    Log.e(LOG_TAG, "Failed to decode metadata: " + e.getMessage());
                    regionName = "Region " + offlineRegion.getID();
                }

                return regionName;
            }
        });
    }

    /*
     * Dialogs
     */

    @Override
    public void onDownloadRegionDialogPositiveClick(final String regionName) {
        if (TextUtils.isEmpty(regionName)) {
            Toast.makeText(OfflineActivity.this, "Region name cannot be empty.", Toast.LENGTH_SHORT).show();
            return;
        }

        // Start progress bar
        Log.d(LOG_TAG, "Download started: " + regionName);
        startProgress();

        // Definition
        String styleURL = mapboxMap.getStyleUrl();
        LatLngBounds bounds = mapboxMap.getProjection().getVisibleRegion().latLngBounds;
        double minZoom = mapboxMap.getCameraPosition().zoom;
        double maxZoom = mapboxMap.getMaxZoom();
        float pixelRatio = this.getResources().getDisplayMetrics().density;
        OfflineTilePyramidRegionDefinition definition = new OfflineTilePyramidRegionDefinition(
                styleURL, bounds, minZoom, maxZoom, pixelRatio);

        // Sample way of encoding metadata from a JSONObject
        byte[] metadata;
        try {
            JSONObject jsonObject = new JSONObject();
            jsonObject.put(JSON_FIELD_REGION_NAME, regionName);
            String json = jsonObject.toString();
            metadata = json.getBytes(JSON_CHARSET);
        } catch (Exception e) {
            Log.e(LOG_TAG, "Failed to encode metadata: " + e.getMessage());
            metadata = null;
        }

        // Create region
        mOfflineManager.createOfflineRegion(definition, metadata, new OfflineManager.CreateOfflineRegionCallback() {
            @Override
            public void onCreate(OfflineRegion offlineRegion) {
                Log.d(LOG_TAG, "Offline region created: " + regionName);
                mOfflineRegion = offlineRegion;
                launchDownload();
            }

            @Override
            public void onError(String error) {
                Log.e(LOG_TAG, "Error: " + error);
            }
        });
    }

    private void launchDownload() {
        // Set an observer
        mOfflineRegion.setObserver(new OfflineRegion.OfflineRegionObserver() {
            @Override
            public void onStatusChanged(OfflineRegionStatus status) {
                // Compute a percentage
                double percentage = status.getRequiredResourceCount() >= 0 ?
                        (100.0 * status.getCompletedResourceCount() / status.getRequiredResourceCount()) :
                        0.0;

                if (status.isComplete()) {
                    // Download complete
                    endProgress("Region downloaded successfully.");
                    return;
                } else if (status.isRequiredResourceCountPrecise()) {
                    // Switch to determinate state
                    setPercentage((int) Math.round(percentage));
                }

                // Debug
                Log.d(LOG_TAG, String.format("%s/%s resources; %s bytes downloaded.",
                        String.valueOf(status.getCompletedResourceCount()),
                        String.valueOf(status.getRequiredResourceCount()),
                        String.valueOf(status.getCompletedResourceSize())));
            }

            @Override
            public void onError(OfflineRegionError error) {
                Log.e(LOG_TAG, "onError reason: " + error.getReason());
                Log.e(LOG_TAG, "onError message: " + error.getMessage());
            }

            @Override
            public void mapboxTileCountLimitExceeded(long limit) {
                Log.e(LOG_TAG, "Mapbox tile count limit exceeded: " + limit);
            }
        });

        // Change the region state
        mOfflineRegion.setDownloadState(OfflineRegion.STATE_ACTIVE);
    }

    /*
     * Progress bar
     */

    private void startProgress() {
        // Disable buttons
        downloadRegion.setEnabled(false);
        listRegions.setEnabled(false);

        // Start and show the progress bar
        isEndNotified = false;
        mProgressBar.setIndeterminate(true);
        mProgressBar.setVisibility(View.VISIBLE);
    }

    private void setPercentage(final int percentage) {
        mProgressBar.setIndeterminate(false);
        mProgressBar.setProgress(percentage);
    }

    private void endProgress(final String message) {
        // Don't notify more than once
        if (isEndNotified) return;

        // Enable buttons
        downloadRegion.setEnabled(true);
        listRegions.setEnabled(true);

        // Stop and hide the progress bar
        isEndNotified = true;
        mProgressBar.setIndeterminate(false);
        mProgressBar.setVisibility(View.GONE);

        // Show a toast
        Toast.makeText(OfflineActivity.this, message, Toast.LENGTH_LONG).show();
    }

}
