package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.content.Context;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.mapbox.mapboxsdk.annotations.BaseMarkerOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.model.annotations.CountryMarker;
import com.mapbox.mapboxsdk.testapp.model.annotations.CountryMarkerOptions;

import java.util.ArrayList;
import java.util.List;

public class ViewMarkerActivity extends AppCompatActivity implements OnMapReadyCallback {

    private MapView mMapView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_marker_view);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(this);
    }

    @Override
    public void onMapReady(MapboxMap mapboxMap) {
        List<BaseMarkerOptions> countries = new ArrayList<>();
        countries.add(new CountryMarkerOptions().title("China").abbrevName("ch").flagRes(R.drawable.ic_china).position(new LatLng(31.230416, 121.473701)));
        countries.add(new CountryMarkerOptions().title("United States").abbrevName("us").flagRes(R.drawable.ic_us).position(new LatLng(38.907192, -77.036871)));
        countries.add(new CountryMarkerOptions().title("Brazil").abbrevName("br").flagRes(R.drawable.ic_brazil).position(new LatLng(-15.798200, -47.922363)));
        countries.add(new CountryMarkerOptions().title("Germany").abbrevName("de").flagRes(R.drawable.ic_germany).position(new LatLng(52.520007, 13.404954)));
        mapboxMap.addMarkers(countries);

        mapboxMap.setMarkerViewAdapter(new CountryAdapter(this));
    }

    private static class CountryAdapter implements MapboxMap.MarkerViewAdapter<CountryMarker> {

        private LayoutInflater inflater;

        public CountryAdapter(@NonNull Context context) {
            this.inflater = LayoutInflater.from(context);
        }

        @Nullable
        @Override
        public View getView(@NonNull CountryMarker marker, @Nullable View convertView, @NonNull ViewGroup parent) {
            ViewHolder viewHolder;
            if (convertView == null) {
                viewHolder = new ViewHolder();
                convertView = inflater.inflate(R.layout.view_custom_marker, parent, false);
                viewHolder.flag = (ImageView) convertView.findViewById(R.id.imageView);
                viewHolder.abbrev = (TextView) convertView.findViewById(R.id.textView);
                convertView.setTag(viewHolder);
            } else {
                viewHolder = (ViewHolder) convertView.getTag();
            }
            viewHolder.flag.setImageResource(marker.getFlagRes());
            viewHolder.abbrev.setText(marker.getAbbrevName());
            return convertView;
        }

        private static class ViewHolder {
            ImageView flag;
            TextView abbrev;
        }
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
}