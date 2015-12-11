package com.mapbox.mapboxsdk.testapp;

import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class PolylineActivity extends AppCompatActivity {

    private static final String STATE_POLYLINE_OPTIONS = "polylineOptions";

    private List<Polyline> mPolylines;
    private ArrayList<PolylineOptions> mPolylineOptions = new ArrayList<>();
    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_polyline);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.setAccessToken(ApiAccess.getToken(this));
        mMapView.onCreate(savedInstanceState);

        if (savedInstanceState != null) {
            mPolylineOptions = savedInstanceState.getParcelableArrayList(STATE_POLYLINE_OPTIONS);
        } else {
            mPolylineOptions.addAll(PolylineProvider.getAll());
        }
        mPolylines = mMapView.addPolylines(mPolylineOptions);

        findViewById(R.id.fab).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mPolylines != null && mPolylines.size() > 0) {
                    if (mPolylines.size() == 1) {
                        // test for removing annotation
                        mMapView.removeAnnotation(mPolylines.get(0));
                    } else {
                        // test for removing annotations
                        mMapView.removeAnnotations(mPolylines);
                    }
                }
                mPolylineOptions.clear();
                mPolylineOptions.addAll(PolylineProvider.getRandomLine());
                mPolylines = mMapView.addPolylines(mPolylineOptions);
            }
        });
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
        outState.putParcelableArrayList(STATE_POLYLINE_OPTIONS, mPolylineOptions);
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
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_polyline, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_id_remove:
                // test to remove all annotations
                mPolylineOptions.clear();
                mMapView.removeAllAnnotations();
                return true;

            case android.R.id.home:
                onBackPressed();
                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private static class PolylineProvider {
        private static final LatLng ANDORRA = new LatLng(42.505777, 1.52529);
        private static final LatLng LUXEMBOURG = new LatLng(49.815273, 6.129583);
        private static final LatLng MONACO = new LatLng(43.738418, 7.424616);
        private static final LatLng VATICAN_CITY = new LatLng(41.902916, 12.453389);
        private static final LatLng SAN_MARINO = new LatLng(43.942360, 12.457777);
        private static final LatLng LIECHTENSTEIN = new LatLng(47.166000, 9.555373);

        private static final List<PolylineOptions> POLYLINES = new ArrayList<PolylineOptions>() {{
            add(generatePolyline(ANDORRA, LUXEMBOURG, "#F44336"));
            add(generatePolyline(ANDORRA, MONACO, "#FF5722"));
            add(generatePolyline(MONACO, VATICAN_CITY, "#673AB7"));
            add(generatePolyline(VATICAN_CITY, SAN_MARINO, "#009688"));
            add(generatePolyline(SAN_MARINO, LIECHTENSTEIN, "#795548"));
            add(generatePolyline(LIECHTENSTEIN, LUXEMBOURG, "#3F51B5"));
        }};

        private static PolylineOptions generatePolyline(LatLng start, LatLng end, String color) {
            PolylineOptions line = new PolylineOptions();
            line.add(start);
            line.add(end);
            line.color(Color.parseColor(color));
            return line;
        }

        public static List<PolylineOptions> getAll() {
            return POLYLINES;
        }

        public static List<PolylineOptions> getRandomLine() {
            List<PolylineOptions> randomLines = new ArrayList<>();
            Collections.shuffle(POLYLINES);
            randomLines.add(POLYLINES.get(0));
            return randomLines;
        }

    }

}
