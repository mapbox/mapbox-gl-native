package com.mapbox.mapboxsdk.testapp.activity.annotation;

import android.content.Context;
import android.graphics.Color;
import android.graphics.PointF;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.TypedValue;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toast;

import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

public class PolylineActivity extends AppCompatActivity {

    private static final String STATE_POLYLINE_OPTIONS = "polylineOptions";

    private static final LatLng ANDORRA = new LatLng(42.505777, 1.52529);
    private static final LatLng LUXEMBOURG = new LatLng(49.815273, 6.129583);
    private static final LatLng MONACO = new LatLng(43.738418, 7.424616);
    private static final LatLng VATICAN_CITY = new LatLng(41.902916, 12.453389);
    private static final LatLng SAN_MARINO = new LatLng(43.942360, 12.457777);
    private static final LatLng LIECHTENSTEIN = new LatLng(47.166000, 9.555373);

    private static final float FULL_ALPHA = 1.0f;
    private static final float PARTIAL_ALPHA = 0.5f;
    private static final float NO_ALPHA = 0.0f;
    private static final String TAG = PolylineActivity.class.getSimpleName();

    private List<Polyline> mPolylines;
    private ArrayList<PolylineOptions> mPolylineOptions = new ArrayList<>();
    private MapView mMapView;
    private MapboxMap mMapboxMap;

    private boolean fullAlpha = true;
    private boolean visible = true;
    private boolean width = true;
    private boolean color = true;
    private String[] colors = {"red", "orange", "purple", "green", "brown", "blue"};
    private HashMap<Polyline, String> polylineColor = new HashMap<>();

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

        if (savedInstanceState != null) {
            mPolylineOptions = savedInstanceState.getParcelableArrayList(STATE_POLYLINE_OPTIONS);
        } else {
            mPolylineOptions.addAll(getAllPolylines());
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(new OnMapReadyCallback() {
            @Override
            public void onMapReady(@NonNull MapboxMap mapboxMap) {
                mMapboxMap = mapboxMap;
                mPolylines = mapboxMap.addPolylines(mPolylineOptions);
                for (int i = 0; i < mPolylines.size(); i++) {
                    polylineColor.put(mPolylines.get(i), colors[i]);
                    Log.d(TAG, "polyline "+colors[i]+ " added");

                }
                int selectionThresholdInPx = (int) dipToPixels(PolylineActivity.this, (float) 15.0);
                configurePolylineClick(createPolylineClickListener(), selectionThresholdInPx);
            }
        });

        View fab = findViewById(R.id.fab);
        if (fab != null) {
            fab.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (mMapboxMap != null) {
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
                        mPolylineOptions.addAll(getRandomLine());
                        mPolylines = mMapboxMap.addPolylines(mPolylineOptions);

                    }
                }
            });
        }

    }

    @NonNull
    private OnPolylineClickListener createPolylineClickListener() {
        return new OnPolylineClickListener() {
            @Override
            public void onPolylineClick(@NonNull Polyline p) {
                String color = polylineColor.get(p);
                Toast.makeText(PolylineActivity.this, "polyline clicked" + p.hashCode()+ " "+color, Toast.LENGTH_SHORT).show();
            }
        };
    }

    private void configurePolylineClick(@NonNull OnPolylineClickListener onPolylineClickListener, int selectionThresholdInPx) {
        mMapboxMap.setOnMapClickListener(createPolylineClickListener(mMapboxMap, onPolylineClickListener, selectionThresholdInPx));
    }

    private MapboxMap.OnMapClickListener createPolylineClickListener(final MapboxMap mMapboxMap, final OnPolylineClickListener onPolylineClickListener, final int selectionThresholdInPx) {
        return new MapboxMap.OnMapClickListener() {
            @Override
            public void onMapClick(@NonNull LatLng point) {
                Polyline p = getProximityPolyline(point, mMapboxMap, selectionThresholdInPx);
                if (p != null) {
                    onPolylineClickListener.onPolylineClick(p);
                }
            }
        };
    }

    @Nullable
    private Polyline getProximityPolyline(@NonNull LatLng latlng, MapboxMap mMapboxMap, int selectionThresholdInPx) {
        List<PolylineInPx> polylinesInPx = convertPolylinesInpx(mMapboxMap, mMapboxMap.getPolylines());
        PointF point = mMapboxMap.getProjection().toScreenLocation(latlng);

        double bestProximity = Double.MAX_VALUE;
        Polyline bestProximityPolyline = null;
        for (PolylineInPx polyline : polylinesInPx) {
            double proximity = bestProximityBeetweenPointAndPolyline(point, polyline);
            if( proximity < polyline.getPolyline().getWidth()/2 + selectionThresholdInPx) {
                Log.d(TAG, "proximity from " + polylineColor.get(polyline.getPolyline()) + " = " + proximity);
                if (proximity < bestProximity) {
                    bestProximityPolyline = polyline.getPolyline();
                }
                bestProximity = Math.min(proximity, bestProximity);
            }
        }
        return bestProximityPolyline;
    }

    public static float dipToPixels(Context context, float dipValue) {
        DisplayMetrics metrics = context.getResources().getDisplayMetrics();
        return TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, dipValue, metrics);
    }

    private List<PolylineInPx> convertPolylinesInpx(MapboxMap mMapboxMap, List<Polyline> polylines) {
        List<PolylineInPx> polylinesInPx = new ArrayList<>();
        for (Polyline polyline : polylines) {
            polylinesInPx.add(convertPolylineInpx(mMapboxMap, polyline));
        }
        return polylinesInPx;
    }

    private PolylineInPx convertPolylineInpx(MapboxMap mMapboxMap, Polyline polyline) {
        PolylineInPx polylineInPx = new PolylineInPx();
        polylineInPx.setPolyline(polyline);
        for (LatLng latLng : polyline.getPoints()) {
            polylineInPx.add(mMapboxMap.getProjection().toScreenLocation(latLng));
        }
        return polylineInPx;
    }

    private static double bestProximityBeetweenPointAndPolyline(PointF point, PolylineInPx polyline) {
        double bestProximity = Double.MAX_VALUE;
        Iterator<Line> lineIterator = polyline.getLineIterator();
        while (lineIterator.hasNext()) {
            bestProximity =  Math.min(computeProximity(point, lineIterator.next()), bestProximity);
        }
        return bestProximity;
    }



    private static double computeProximity(PointF point, Line line) {
        PointF end = line.p2;
        PointF start = line.p1;
//        double normalLength = Math.hypot(end.y - start.y, end.x - start.x);
//        double ret = Math.abs(((point.y - start.y) * (end.x - start.x) - (end.y - start.y))/normalLength );

        return PointFProjection.getDistanceToSegment(line.p1, line.p2, point);
    }

    private List<PolylineOptions> getAllPolylines() {
        List<PolylineOptions> options = new ArrayList<>();
        options.add(generatePolyline(ANDORRA, LUXEMBOURG, "#F44336"));
        options.add(generatePolyline(ANDORRA, MONACO, "#FF5722"));
        options.add(generatePolyline(MONACO, VATICAN_CITY, "#673AB7"));
        options.add(generatePolyline(VATICAN_CITY, SAN_MARINO, "#009688"));
        options.add(generatePolyline(SAN_MARINO, LIECHTENSTEIN, "#795548"));
        options.add(generatePolyline(LIECHTENSTEIN, LUXEMBOURG, "#3F51B5"));
        return options;
    }

    private PolylineOptions generatePolyline(LatLng start, LatLng end, String color) {
        PolylineOptions line = new PolylineOptions();
        line.add(start);
        line.add(end);
        line.color(Color.parseColor(color));
        return line;
    }

    public List<PolylineOptions> getRandomLine() {
        final List<PolylineOptions> randomLines = getAllPolylines();
        Collections.shuffle(randomLines);
        return new ArrayList<PolylineOptions>() {{
            add(randomLines.get(0));
        }};
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
                mMapboxMap.clear();
                return true;

            case R.id.action_id_alpha:
                fullAlpha = !fullAlpha;
                for (Polyline p : mPolylines) {
                    p.setAlpha(fullAlpha ? FULL_ALPHA : PARTIAL_ALPHA);
                }
                return true;

            case R.id.action_id_color:
                color = !color;
                for (Polyline p : mPolylines) {
                    p.setColor(color ? Color.RED : Color.BLUE);
                }
                return true;

            case R.id.action_id_width:
                width = !width;
                for (Polyline p : mPolylines) {
                    p.setWidth(width ? 3.0f : 5.0f);
                }
                return true;

            case R.id.action_id_visible:
                visible = !visible;
                for (Polyline p : mPolylines) {
                    p.setAlpha(visible ? (fullAlpha ? FULL_ALPHA : PARTIAL_ALPHA) : NO_ALPHA);
                }
                return true;

            case android.R.id.home:
                onBackPressed();
                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private static class Line {
        PointF p1;
        PointF p2;

        public Line(PointF p1, PointF p2) {
            this.p1 = p1;
            this.p2 = p2;
        }
    }

    private interface OnPolylineClickListener {
        void onPolylineClick(@NonNull Polyline p);
    }

    private class PolylineInPx {


        private ArrayList<PointF> points = new ArrayList<>();
        private Polyline polyline;

        public void add(PointF pointF) {
            points.add(pointF);
        }

        public Iterator<Line> getLineIterator() {

            return new Iterator<Line>() {
                final Iterator<PointF> it = points.iterator();
                 PointF previousPoint=null;
                @Override
                public boolean hasNext() {
                    return it.hasNext();
                }

                @Override
                public Line next() {
                    if(previousPoint==null) {
                        previousPoint= it.next();
                    }
                    return new Line(previousPoint, it.next());
                }

                @Override
                public void remove() {

                }
            };
        }

        public void setPolyline(Polyline polyline) {
            this.polyline = polyline;
        }

        public Polyline getPolyline() {
            return polyline;
        }
    }
}
