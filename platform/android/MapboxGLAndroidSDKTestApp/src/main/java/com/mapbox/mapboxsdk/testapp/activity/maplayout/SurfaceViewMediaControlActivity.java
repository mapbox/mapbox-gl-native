package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.widget.FrameLayout;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class SurfaceViewMediaControlActivity extends AppCompatActivity implements OnMapReadyCallback {

    private MapView mMapView;
    private MapboxMap mapboxMap;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_surfaceview_mediacontrols);

        final Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setDisplayShowHomeEnabled(true);
        }

        mMapView = (MapView) findViewById(R.id.mapView);
        mMapView.onCreate(savedInstanceState);
        mMapView.getMapAsync(this);

        // add another SurfaceView to the Layout
        FrameLayout container = (FrameLayout) findViewById(R.id.container);
        GLSurfaceView mediaControlSurfaceView = new ClearGLSurfaceView(this);
        mediaControlSurfaceView.setZOrderMediaOverlay(true);
        container.addView(mediaControlSurfaceView);
    }

    @Override
    public void onMapReady(MapboxMap mapboxMap) {
        this.mapboxMap = mapboxMap;
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
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_padding, menu);
        return true;
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

    class ClearGLSurfaceView extends GLSurfaceView {

        private ClearRenderer mRenderer;

        public ClearGLSurfaceView(Context context) {
            super(context);
            mRenderer = new ClearRenderer();
            setRenderer(mRenderer);
        }

        public boolean onTouchEvent(final MotionEvent event) {
            queueEvent(new Runnable() {
                public void run() {
                    mRenderer.setColor(event.getRawX() / getWidth(),
                            event.getRawY() / getHeight(), 1.0f);
                }
            });
            return true;
        }
    }

    class ClearRenderer implements GLSurfaceView.Renderer {

        private float mRed;
        private float mGreen;
        private float mBlue;

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            // Do nothing special.
        }

        public void onSurfaceChanged(GL10 gl, int w, int h) {
            gl.glViewport(0, 0, w, h);
        }

        public void onDrawFrame(GL10 gl) {
            gl.glClearColor(mRed, mGreen, mBlue, 1.0f);
            gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
        }

        public void setColor(float r, float g, float b) {
            mRed = r;
            mGreen = g;
            mBlue = b;
        }
    }
}

