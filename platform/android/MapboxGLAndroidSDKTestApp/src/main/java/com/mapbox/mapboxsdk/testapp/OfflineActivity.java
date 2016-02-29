package com.mapbox.mapboxsdk.testapp;

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
import com.mapbox.mapboxsdk.offline.OfflineTilePyramidRegionDefinition;
import com.mapbox.mapboxsdk.offline.OfflineRegionError;
import com.mapbox.mapboxsdk.offline.OfflineRegionStatus;
import com.mapbox.mapboxsdk.testapp.offline.DownloadRegionDialog;
import com.mapbox.mapboxsdk.testapp.offline.CustomMetadata;
import com.mapbox.mapboxsdk.testapp.offline.ListRegionsDialog;
import com.mapbox.mapboxsdk.utils.ApiAccess;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;

public class OfflineActivity extends AppCompatActivity
        implements DownloadRegionDialog.DownloadRegionDialogListener {

    private final static String LOG_TAG = "OfflineActivity";

    /*
     * UI elements
     */
    private MapView mMapView;
    private MapboxMap mMapboxMap;
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
        mMapView.setAccessToken(ApiAccess.getToken(this));
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                Log.d(LOG_TAG, "Map is ready");
                mMapboxMap = mapboxMap;

                // Set style
                mapboxMap.setStyle(Style.MAPBOX_STREETS);

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
        mOfflineManager.setAccessToken(ApiAccess.getToken(this));
    }

    @Override
    protected void onStart() {
        super.onStart();
        mMapView.onStart();
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
    protected void onStop() {
        super.onStop();
        mMapView.onStop();
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
        DownloadRegionDialog downloadRegionDialog = new DownloadRegionDialog();
        downloadRegionDialog.show(getSupportFragmentManager(), "download");
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
                args.putStringArrayList(ListRegionsDialog.ITEMS, offlineRegionsNames);

                // Show dialog
                ListRegionsDialog listRegionsDialog = new ListRegionsDialog();
                listRegionsDialog.setArguments(args);
                listRegionsDialog.show(getSupportFragmentManager(), "list");
            }

            @Override
            public void onError(String error) {
                Log.e(LOG_TAG, "Error: " + error);
            }

            private String getRegionName(OfflineRegion offlineRegion) {
                String regionName;

                try {
                    CustomMetadata customMetadata = CustomMetadata.decode(offlineRegion.getMetadata());
                    regionName = customMetadata.getRegionName();
                } catch (UnsupportedEncodingException e) {
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
        String styleURL = mMapboxMap.getStyleUrl();
        LatLngBounds bounds = mMapboxMap.getProjection().getVisibleRegion().latLngBounds;
        double minZoom = mMapboxMap.getCameraPosition().zoom;
        double maxZoom = mMapboxMap.getUiSettings().getMaxZoom();
        float pixelRatio = this.getResources().getDisplayMetrics().density;
        OfflineTilePyramidRegionDefinition definition = new OfflineTilePyramidRegionDefinition(
                styleURL, bounds, minZoom, maxZoom, pixelRatio);

        // Sample way of encoding metadata
        byte[] metadata;
        try {
            CustomMetadata customMetadata = new CustomMetadata(regionName);
            metadata = customMetadata.encode();
        } catch (UnsupportedEncodingException e) {
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

                if (status.isComplete() || percentage >= 98.0 /* Known issue */) {
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
