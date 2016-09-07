package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.animation.Animator;
import android.animation.AnimatorInflater;
import android.animation.AnimatorListenerAdapter;
import android.animation.ObjectAnimator;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
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
import android.widget.Toast;

import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.annotations.MarkerViewOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.model.annotations.CountryMarkerView;
import com.mapbox.mapboxsdk.testapp.model.annotations.CountryMarkerViewOptions;
import com.mapbox.mapboxsdk.testapp.model.annotations.TextMarkerView;
import com.mapbox.mapboxsdk.testapp.model.annotations.TextMarkerViewOptions;

import java.util.Random;

public class MarkerViewActivity extends AppCompatActivity {

    private MapboxMap mapboxMap;
    private MapView mMapView;

    private MarkerView movingMarkerOne, movingMarkerTwo;
    private Random randomAnimator = new Random();
    private Handler locationUpdateHandler = new Handler();
    private Runnable moveMarkerRunnable = new MoveMarkerRunnable();
    private int rotation = 0;

    private final static LatLng[] LAT_LNGS = new LatLng[]{
            new LatLng(38.897424, -77.036508),
            new LatLng(38.909698, -77.029642),
            new LatLng(38.907227, -77.036530),
            new LatLng(38.905607, -77.031916),
            new LatLng(38.889441, -77.050134),
            new LatLng(38.888000, -77.050000) //Slight overlap to show re-ordering on selection
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
                MarkerViewActivity.this.mapboxMap = mapboxMap;

                final MarkerViewManager markerViewManager = mapboxMap.getMarkerViewManager();

                Icon usFlag = IconFactory.getInstance(MarkerViewActivity.this)
                        .fromResource(R.drawable.ic_us);

                // add default ViewMarker markers
                for (int i = 0; i < LAT_LNGS.length; i++) {
                    MarkerViewActivity.this.mapboxMap.addMarker(new MarkerViewOptions()
                            .position(LAT_LNGS[i])
                            .title(String.valueOf(i))
                            .alpha(0.5f)
                            .icon(usFlag)
                    );
                }

                // add custom ViewMarker
                CountryMarkerViewOptions options = new CountryMarkerViewOptions();
                options.flagRes(R.drawable.icon_burned);
                options.abbrevName("Mapbox");
                options.title("Hello");
                options.position(new LatLng(38.899774, -77.023237));
                options.flat(true);
                mapboxMap.addMarker(options);

                MarkerViewActivity.this.mapboxMap.addMarker(new MarkerOptions()
                        .title("United States")
                        .position(new LatLng(38.902580, -77.050102))
                );

                MarkerViewActivity.this.mapboxMap.addMarker(new TextMarkerViewOptions()
                        .text("A")
                        .position(new LatLng(38.889876, -77.008849))
                );

                MarkerViewActivity.this.mapboxMap.addMarker(new TextMarkerViewOptions()
                        .text("B")
                        .position(new LatLng(38.907327, -77.041293))
                );

                MarkerViewActivity.this.mapboxMap.addMarker(new TextMarkerViewOptions()
                        .text("C")
                        .position(new LatLng(38.897642, -77.041980))
                );

                // if you want to customise a ViewMarker you need to extend ViewMarker and provide an adapter implementation
                // set adapters for child classes of ViewMarker
                markerViewManager.addMarkerViewAdapter(new CountryAdapter(MarkerViewActivity.this, mapboxMap));
                markerViewManager.addMarkerViewAdapter(new TextAdapter(MarkerViewActivity.this, mapboxMap));

                // add a change listener to validate the size of amount of child views
                mMapView.addOnMapChangedListener(new MapView.OnMapChangedListener() {
                    @Override
                    public void onMapChanged(@MapView.MapChange int change) {
                        if (change == MapView.REGION_IS_CHANGING || change == MapView.REGION_DID_CHANGE) {
                            if (!markerViewManager.getMarkerViewAdapters().isEmpty() && viewCountView != null) {
                                viewCountView.setText("ViewCache size " + (mMapView.getChildCount() - 5));
                            }
                        }
                    }
                });

                // add a OnMarkerView click listener
                MarkerViewActivity.this.mapboxMap.getMarkerViewManager().setOnMarkerViewClickListener(new MapboxMap.OnMarkerViewClickListener() {
                    @Override
                    public boolean onMarkerClick(@NonNull Marker marker, @NonNull View view, @NonNull MapboxMap.MarkerViewAdapter adapter) {
                        Toast.makeText(MarkerViewActivity.this, "Hello " + marker.getId(), Toast.LENGTH_SHORT).show();
                        return false;
                    }
                });

                movingMarkerOne = MarkerViewActivity.this.mapboxMap.addMarker(new MarkerViewOptions()
                        .position(CarLocation.CAR_0_LNGS[0])
                        .icon(IconFactory.getInstance(mMapView.getContext())
                                .fromResource(R.drawable.ic_chelsea))
                );

                movingMarkerTwo = mapboxMap.addMarker(new MarkerViewOptions()
                        .position(CarLocation.CAR_1_LNGS[0])
                        .icon(IconFactory.getInstance(mMapView.getContext())
                                .fromResource(R.drawable.ic_arsenal))
                );
            }
        });
    }

    @Override
    protected void onStart() {
        super.onStart();
        loopMarkerMove();
    }

    private void loopMarkerMove() {
        locationUpdateHandler.postDelayed(moveMarkerRunnable, randomAnimator.nextInt(3000) + 1000);
    }

    @Override
    protected void onStop() {
        super.onStop();
        locationUpdateHandler.removeCallbacks(moveMarkerRunnable);
    }

    private class MoveMarkerRunnable implements Runnable {
        @Override
        public void run() {
            int i = randomAnimator.nextInt(9);
            if (randomAnimator.nextInt() % 2 == 0) {
                movingMarkerOne.setPosition(CarLocation.CAR_0_LNGS[i]);
                movingMarkerOne.setRotation(rotation = rotation + 45);
            } else {
                movingMarkerTwo.setPosition(CarLocation.CAR_1_LNGS[i]);
                movingMarkerTwo.setRotation(rotation = rotation + 90);
            }
            loopMarkerMove();
        }
    }

    private static class CountryAdapter extends MapboxMap.MarkerViewAdapter<CountryMarkerView> {

        private LayoutInflater inflater;
        private MapboxMap mapboxMap;

        public CountryAdapter(@NonNull Context context, @NonNull MapboxMap mapboxMap) {
            super(context);
            this.inflater = LayoutInflater.from(context);
            this.mapboxMap = mapboxMap;
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
            viewHolder.flag.setImageResource(marker.getFlagRes());
            viewHolder.abbrev.setText(marker.getAbbrevName());
            return convertView;
        }

        @Override
        public boolean onSelect(@NonNull final CountryMarkerView marker, @NonNull final View convertView, boolean reselectionForViewReuse) {
            convertView.setLayerType(View.LAYER_TYPE_HARDWARE, null);
            ObjectAnimator rotateAnimator = ObjectAnimator.ofFloat(convertView, View.ROTATION, 0, 360);
            rotateAnimator.setDuration(reselectionForViewReuse ? 0 : 350);
            rotateAnimator.addListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    super.onAnimationEnd(animation);
                    convertView.setLayerType(View.LAYER_TYPE_NONE, null);
                    mapboxMap.selectMarker(marker);
                }
            });
            rotateAnimator.start();

            // false indicates that we are calling selectMarker after our animation ourselves
            // true will let the system call it for you, which will result in showing an InfoWindow instantly
            return false;
        }

        @Override
        public void onDeselect(@NonNull CountryMarkerView marker, @NonNull final View convertView) {
            convertView.setLayerType(View.LAYER_TYPE_HARDWARE, null);
            ObjectAnimator rotateAnimator = ObjectAnimator.ofFloat(convertView, View.ROTATION, 360, 0);
            rotateAnimator.addListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    super.onAnimationEnd(animation);
                    convertView.setLayerType(View.LAYER_TYPE_NONE, null);
                }
            });
            rotateAnimator.start();
        }

        private static class ViewHolder {
            ImageView flag;
            TextView abbrev;
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

    private static class CarLocation {

        public static LatLng[] CAR_0_LNGS = new LatLng[]{
                new LatLng(38.92334425495122, -77.0533673443786),
                new LatLng(38.9234737236897, -77.05389484528261),
                new LatLng(38.9257094658146, -76.98819752280579),
                new LatLng(38.8324328369647, -77.00690648325929),
                new LatLng(38.87540698725855, -77.0093148713099),
                new LatLng(38.96499498141065, -77.07707916040054),
                new LatLng(38.90794910679896, -76.99695304153806),
                new LatLng(38.86234025281626, -76.9950528034839),
                new LatLng(38.862930274733635, -76.99647808241964)
        };

        public static LatLng[] CAR_1_LNGS = new LatLng[]{
                new LatLng(38.94237975070426, -76.98324549005675),
                new LatLng(38.941520236084486, -76.98234257804742),
                new LatLng(38.85972219720714, -76.98955808483929),
                new LatLng(38.944289166113776, -76.98584257252891),
                new LatLng(38.94375860578053, -76.98470344318412),
                new LatLng(38.943167431929645, -76.98373163938666),
                new LatLng(38.882834728904605, -77.02862535635137),
                new LatLng(38.882869724926245, -77.02992539231113),
                new LatLng(38.9371988177896, -76.97786740676564)
        };

    }
}
