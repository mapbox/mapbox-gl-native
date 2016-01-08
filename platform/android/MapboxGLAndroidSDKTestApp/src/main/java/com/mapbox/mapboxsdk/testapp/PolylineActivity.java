package com.mapbox.mapboxsdk.testapp;

import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import com.mapbox.mapboxsdk.MapFragment;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngZoom;
import com.mapbox.mapboxsdk.utils.ApiAccess;
import com.mapbox.mapboxsdk.views.MapView;
import com.mapbox.mapboxsdk.views.MapboxMap;
import com.mapbox.mapboxsdk.views.OnMapReadyCallback;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class PolylineActivity extends AppCompatActivity implements OnMapReadyCallback, View.OnClickListener{

    private static final String STATE_POLYLINE_OPTIONS = "polylineOptions";

    private MapboxMap mMapboxMap;
    private List<Polyline> mPolylines;
    private List<PolylineOptions> mPolylineOptions = new ArrayList<>();

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

        findViewById(R.id.fab).setOnClickListener(this);

        if (savedInstanceState != null) {
            mPolylineOptions = savedInstanceState.getParcelableArrayList(STATE_POLYLINE_OPTIONS);
        } else {
            mPolylineOptions.addAll(PolylineProvider.getAll());
        }

        MapFragment mapFragment = (MapFragment) getSupportFragmentManager().findFragmentById(R.id.mapFragment);
        mapFragment.getMapAsync(this);
    }

    @Override
    public void onMapReady(@NonNull MapboxMap mapboxMap) {
        mMapboxMap = mapboxMap;
        mMapboxMap.setLatLng(new LatLngZoom(47.798202,7.573781,4));
        mMapboxMap.setStyle(Style.EMERALD);
        mPolylines = mapboxMap.addPolylines(mPolylineOptions);
    }

    @Override
    public void onClick(View v) {
        if(mMapboxMap!=null) {
            if (mPolylines != null && mPolylines.size() > 0) {
                if (mPolylines.size() == 1) {
                    // test for removing annotation
                    mMapboxMap.removeAnnotation(mPolylines.get(0));
                } else {
                    // test for removing annotations
                    mMapboxMap.removeAnnotations(mPolylines);
                }
            }
            mPolylineOptions.clear();
            mPolylineOptions.addAll(PolylineProvider.getRandomLine());
            mPolylines = mMapboxMap.addPolylines(mPolylineOptions);
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_id_remove:
                // test to remove all annotations
                mPolylineOptions.clear();
                mMapboxMap.removeAllAnnotations();
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
