package com.mapbox.mapboxsdk.testapp;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.MenuItem;
import com.mapbox.mapboxsdk.views.MapView;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class SecondMapActivity extends AppCompatActivity {

    private static final String TAG = "SecondMapActivity";

    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_second);

        Toolbar toolbar = (Toolbar) findViewById(R.id.secondToolBar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setDisplayShowHomeEnabled(true);

        mMapView = (MapView) findViewById(R.id.secondMapView);
        // Load the access token
        try {
            BufferedReader reader = new BufferedReader(new InputStreamReader(getResources().openRawResource(R.raw.token)));
            String line = reader.readLine();
            mMapView.setAccessToken(line);
        } catch (IOException e) {
            Log.e(TAG, "Error loading access token from token.txt: " + e.toString());
        }

        mMapView.onCreate(savedInstanceState);

        mMapView.setStyleUrl(MapView.StyleUrls.EMERALD);
    }

    /**
     * Dispatch onStart() to all fragments.  Ensure any created loaders are
     * now started.
     */
    @Override
    protected void onStart() {
        super.onStart();
        mMapView.onStart();
    }

    @Override
    protected void onStop() {
        super.onStop();
        mMapView.onStop();
    }

    // Called when our app goes into the background
    @Override
    public void onPause()  {
        super.onPause();

        mMapView.onPause();
    }

    // Called when our app comes into the foreground
    @Override
    public void onResume() {
        super.onResume();

        mMapView.onResume();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mMapView.onDestroy();
    }

    // Called when the system is running low on memory
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
}
