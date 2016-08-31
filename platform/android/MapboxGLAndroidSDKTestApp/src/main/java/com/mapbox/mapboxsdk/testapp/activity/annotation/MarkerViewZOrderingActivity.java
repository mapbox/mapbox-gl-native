package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.animation.Animator;
import android.animation.AnimatorInflater;
import android.animation.AnimatorListenerAdapter;
import android.content.Context;
import android.os.Bundle;
import android.os.PersistableBundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.mapbox.mapboxsdk.annotations.BaseMarkerViewOptions;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.model.annotations.PulseMarkerView;
import com.mapbox.mapboxsdk.testapp.model.annotations.PulseMarkerViewOptions;
import com.mapbox.mapboxsdk.testapp.model.annotations.TextMarkerView;
import com.mapbox.mapboxsdk.testapp.model.annotations.TextMarkerViewOptions;

import java.util.ArrayList;

public class MarkerViewZOrderingActivity extends AppCompatActivity {

    private static final String KEY_PARCEABLE_MARKERVIEWOPTIONS = "com.mapbox.markerviewoptions";

    private MapView mapView;
    private ArrayList<BaseMarkerViewOptions> markerViewOptions;

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_marker_view_z);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mapView = (MapView) findViewById(R.id.mapView);
        mapView.onCreate(savedInstanceState);
        mapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(MapboxMap mapboxMap) {

                // add marker view adapter
                MarkerViewManager markerViewManager = mapboxMap.getMarkerViewManager();
                markerViewManager.addMarkerViewAdapter(new BackgroundViewAdapter(MarkerViewZOrderingActivity.this), 0 /* Index in parent layout */);
                markerViewManager.addMarkerViewAdapter(new TextAdapter(MarkerViewZOrderingActivity.this, mapboxMap), 1 /* I'm added in front of above */);

                if (savedInstanceState == null) {
                    markerViewOptions = new ArrayList<>();

                    TextMarkerViewOptions diplomaticRoomOptions = new TextMarkerViewOptions().position(new LatLng(38.897605, -77.036580)).text("Diplomatic Room");
                    MarkerView diplomaticRoom = mapboxMap.addMarker(diplomaticRoomOptions);
                    markerViewOptions.add(diplomaticRoomOptions);

                    TextMarkerViewOptions kitchenOptions = new TextMarkerViewOptions().position(new LatLng(38.897745, -77.036784)).text("Kitchen");
                    mapboxMap.addMarker(kitchenOptions);
                    markerViewOptions.add(kitchenOptions);

                    TextMarkerViewOptions library = new TextMarkerViewOptions().position(new LatLng(38.897751, -77.036407)).text("Library");
                    mapboxMap.addMarker(library);
                    markerViewOptions.add(library);

                    PulseMarkerViewOptions background = new PulseMarkerViewOptions().position(new LatLng(38.897654, -77.036589)).anchor(0.5f, 0.5f).alpha(0.33f);
                    PulseMarkerView markerView = (PulseMarkerView) mapboxMap.addMarker(background);
                    markerView.setAlpha(0.33f);
                    markerView.setAnchor(0.5f, 0.5f);
                    markerViewOptions.add(background);

                } else {
                    // restore markers
                    markerViewOptions = savedInstanceState.getParcelableArrayList(KEY_PARCEABLE_MARKERVIEWOPTIONS);
                    if (markerViewOptions != null) {
                        for (BaseMarkerViewOptions textMarkerViewOptions : markerViewOptions) {
                            mapboxMap.addMarker(textMarkerViewOptions);
                        }
                    }
                }
            }
        });
    }

    private static class BackgroundViewAdapter extends MapboxMap.MarkerViewAdapter<PulseMarkerView> {

        private LayoutInflater inflater;

        public BackgroundViewAdapter(@NonNull Context context) {
            super(context);
            this.inflater = LayoutInflater.from(context);
        }

        @Nullable
        @Override
        public View getView(@NonNull PulseMarkerView marker, @Nullable View convertView, @NonNull ViewGroup parent) {
            return inflater.inflate(R.layout.view_rectangle, parent, false);
        }
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
    protected void onDestroy() {
        super.onDestroy();
        mapView.onDestroy();
    }

    @Override
    public void onSaveInstanceState(Bundle outState, PersistableBundle outPersistentState) {
        super.onSaveInstanceState(outState, outPersistentState);
        mapView.onSaveInstanceState(outState);
        outState.putParcelableArrayList(KEY_PARCEABLE_MARKERVIEWOPTIONS, markerViewOptions);
    }

    @Override
    public void onLowMemory() {
        super.onLowMemory();
        mapView.onLowMemory();
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

    private static class TextAdapter extends MapboxMap.MarkerViewAdapter<TextMarkerView> {

        private LayoutInflater inflater;
        private MapboxMap mapboxMap;

        public TextAdapter(@NonNull Context context, @NonNull MapboxMap mapboxMap) {
            super(context);
            this.inflater = LayoutInflater.from(context);
            this.mapboxMap = mapboxMap;
        }

        @Nullable
        @Override
        public View getView(@NonNull TextMarkerView marker, @Nullable View convertView, @NonNull ViewGroup parent) {
            ViewHolder viewHolder;
            if (convertView == null) {
                viewHolder = new ViewHolder();
                convertView = inflater.inflate(R.layout.view_text_marker, parent, false);
                viewHolder.textView = (TextView) convertView.findViewById(R.id.textView);
                convertView.setTag(viewHolder);
            } else {
                viewHolder = (ViewHolder) convertView.getTag();
            }
            viewHolder.textView.setText(marker.getText());
            return convertView;
        }

        @Override
        public boolean onSelect(@NonNull final TextMarkerView marker, @NonNull final View convertView, boolean reselectionForViewReuse) {
            animateGrow(marker, convertView, 0);

            // false indicates that we are calling selectMarker after our animation ourselves
            // true will let the system call it for you, which will result in showing an InfoWindow instantly
            return false;
        }

        @Override
        public void onDeselect(@NonNull TextMarkerView marker, @NonNull final View convertView) {
            animateShrink(convertView, 350);
        }

        @Override
        public boolean prepareViewForReuse(@NonNull MarkerView marker, @NonNull View convertView) {
            // this method is called before a view will be reused, we need to restore view state
            // as we have scaled the view in onSelect. If not correctly applied other MarkerView will
            // become large since these have been recycled

            // cancel ongoing animation
            convertView.animate().cancel();

            if (marker.isSelected()) {
                // shrink view to be able to be reused
                animateShrink(convertView, 0);
            }

            // true if you want reuse to occur automatically, false if you want to manage this yourself
            return true;
        }

        private void animateGrow(@NonNull final MarkerView marker, @NonNull final View convertView, int duration) {
            convertView.setLayerType(View.LAYER_TYPE_HARDWARE, null);
            Animator animator = AnimatorInflater.loadAnimator(convertView.getContext(), R.animator.scale_up);
            animator.setDuration(duration);
            animator.addListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    super.onAnimationEnd(animation);
                    convertView.setLayerType(View.LAYER_TYPE_NONE, null);
                    mapboxMap.selectMarker(marker);
                }
            });
            animator.setTarget(convertView);
            animator.start();
        }

        private void animateShrink(@NonNull final View convertView, int duration) {
            convertView.setLayerType(View.LAYER_TYPE_HARDWARE, null);
            Animator animator = AnimatorInflater.loadAnimator(convertView.getContext(), R.animator.scale_down);
            animator.setDuration(duration);
            animator.addListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    super.onAnimationEnd(animation);
                    convertView.setLayerType(View.LAYER_TYPE_NONE, null);
                }
            });
            animator.setTarget(convertView);
            animator.start();
        }

        private static class ViewHolder {
            TextView textView;
        }
    }
}
