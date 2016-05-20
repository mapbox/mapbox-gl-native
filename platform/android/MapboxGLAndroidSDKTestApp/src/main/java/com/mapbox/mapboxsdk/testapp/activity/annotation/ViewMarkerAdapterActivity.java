package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.content.Context;
import android.graphics.Point;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.annotations.MarkerViewOptions;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.model.annotations.CountryMarkerOptions;
import com.mapbox.mapboxsdk.testapp.model.annotations.CountryMarkerView;
import com.mapbox.mapboxsdk.testapp.model.annotations.CountryMarkerViewOptions;

public class ViewMarkerAdapterActivity extends AppCompatActivity {

    private MapboxMap mMapboxMap;
    private MapView mMapView;

    private final static LatLng[] LAT_LNGS = new LatLng[]{
            new LatLng(38.907327, -77.041293),
            new LatLng(38.909698, -77.029642),
            new LatLng(38.907227, -77.036530),
            new LatLng(38.905607, -77.031916),
            new LatLng(38.897424, -77.036508),
            new LatLng(38.897642, -77.041980),
            new LatLng(38.889876, -77.008849),
            new LatLng(38.889441, -77.050134)
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_marker_view);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        final ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        final TextView viewCountView = (TextView) findViewById(R.id.countView);
        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                mMapboxMap = mapboxMap;

                // add text markers
                for (int i = 0; i < LAT_LNGS.length; i++) {
                    mMapboxMap.addMarkerView(new MarkerViewOptions()
                            .position(LAT_LNGS[i])
                            .title(String.valueOf(i))
                            .infoWindowOffset(new Point(0, 86))
                            .selectAnimatorResource(R.animator.scale_up)
                            .deselectAnimatorResource(R.animator.scale_down)
                    );
                }

                // add flag marker
                CountryMarkerViewOptions options = new CountryMarkerViewOptions();
                options.title("United States");
                options.abbrevName("us");
                options.flagRes(R.drawable.ic_us);
                options.position(new LatLng(38.899774, -77.023237));
                options.selectAnimatorResource(R.animator.rotate_360);
                options.deselectAnimatorResource(R.animator.rotate_360);
                options.infoWindowOffset(new Point(0, 64));
                options.flat(true);

                mapboxMap.addMarkerView(options);

                // default GL marker
                mMapboxMap.addMarker(new CountryMarkerOptions()
                        .title("United States")
                        .position(new LatLng(38.902580, -77.050102))
                );

                // set adapters
                mMapboxMap.addMarkerViewAdapter(new TextAdapter(ViewMarkerAdapterActivity.this));
                mMapboxMap.addMarkerViewAdapter(new CountryAdapter(ViewMarkerAdapterActivity.this));

                mMapView.addOnMapChangedListener(new MapView.OnMapChangedListener() {
                    @Override
                    public void onMapChanged(@MapView.MapChange int change) {
                        if (change == MapView.REGION_IS_CHANGING || change == MapView.REGION_DID_CHANGE) {
                            if (!mMapboxMap.getMarkerViewAdapters().isEmpty() && viewCountView != null) {
                                viewCountView.setText("ViewCache size " + (mMapView.getChildCount() - 5));
                            }
                        }
                    }
                });

                mMapboxMap.setOnMarkerViewClickListener(new MapboxMap.OnMarkerViewClickListener() {
                    @Override
                    public boolean onMarkerClick(@NonNull Marker marker, @NonNull View view, @NonNull MapboxMap.MarkerViewAdapter adapter) {
                        Toast.makeText(ViewMarkerAdapterActivity.this, "Hello " + marker.getId(), Toast.LENGTH_SHORT).show();
                        return false;
                    }
                });
            }
        });
    }

    private static class TextAdapter extends MapboxMap.MarkerViewAdapter<MarkerView> {

        private LayoutInflater inflater;

        public TextAdapter(@NonNull Context context) {
            super(context);
            this.inflater = LayoutInflater.from(context);
        }

        @Nullable
        @Override
        public View getView(@NonNull MarkerView marker, @Nullable View convertView, @NonNull ViewGroup parent) {
            ViewHolder viewHolder;
            if (convertView == null) {
                viewHolder = new ViewHolder();
                convertView = inflater.inflate(R.layout.view_text_marker, parent, false);
                viewHolder.title = (TextView) convertView.findViewById(R.id.textView);
                convertView.setTag(viewHolder);
            } else {
                viewHolder = (ViewHolder) convertView.getTag();
            }
            Log.v(MapboxConstants.TAG, "SSSSST");
            viewHolder.title.setText(marker.getTitle());
            return convertView;
        }

        private static class ViewHolder {
            TextView title;
        }
    }

    private static class CountryAdapter extends MapboxMap.MarkerViewAdapter<CountryMarkerView> {

        private LayoutInflater inflater;

        public CountryAdapter(@NonNull Context context) {
            super(context);
            this.inflater = LayoutInflater.from(context);
        }

        @Nullable
        @Override
        public View getView(@NonNull CountryMarkerView marker, @Nullable View convertView, @NonNull ViewGroup parent) {
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
            Log.v(MapboxConstants.TAG, "TESTSSSSS");
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
}
