package com.mapbox.mapboxgl;

import android.content.Context;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import org.apache.commons.io.Charsets;
import org.apache.commons.io.IOUtils;

import java.io.IOException;

public class MapView extends SurfaceView {

    private static String TAG = "MapView";

    private String defaultStyleJSON;

    public MapView(Context context) {
        super(context);
        initialize(context, null, 0);
    }

    public MapView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initialize(context, attrs, 0);
    }

    public MapView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        initialize(context, attrs, defStyle);
    }

    private void initialize(Context context, AttributeSet attrs, int defStyle) {
        Log.v(TAG, "initialize");

        // Load attributes
        final TypedArray a = getContext().obtainStyledAttributes(attrs, R.styleable.MapView, defStyle, 0);

        a.recycle();

        try {
            defaultStyleJSON = IOUtils.toString(context.getResources().openRawResource(R.raw.style), Charsets.UTF_8);
        } catch (IOException e) {
            throw new RuntimeException("Couldn't load default style JSON resource", e);
        }

        getHolder().addCallback(new Callbacks());
    }

    public void onPause() {
        Log.v(TAG, "onPause");
        if (nativeMapViewPtr != 0) {
            nativeOnPause(nativeMapViewPtr);
        }
    }

    public void onResume() {
        Log.v(TAG, "onResume");
        if (nativeMapViewPtr != 0) {
            nativeOnResume(nativeMapViewPtr);
        }
    }

    public void onStop() {
        Log.v(TAG, "onStop");
        if (nativeMapViewPtr != 0) {
            nativeOnStop(nativeMapViewPtr);
        }
    }

    private class Callbacks implements SurfaceHolder.Callback2 {
        @Override
        public void surfaceRedrawNeeded(SurfaceHolder holder) {
            Log.v(TAG, "surfaceRedrawNeeded");
            if (nativeMapViewPtr != 0) {
                nativeSurfaceRedrawNeeded(nativeMapViewPtr);
            }
        }

        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            Log.v(TAG, "surfaceCreated");
            if (nativeMapViewPtr != 0) {
                nativeSurfaceDestroyed(nativeMapViewPtr);
            }
            nativeMapViewPtr = nativeSurfaceCreated(holder.getSurface(), defaultStyleJSON);
            if (nativeMapViewPtr == 0) {
                Log.e(TAG, "nativeSurfaceCreated failed");
                throw new RuntimeException("Unable to initialize NativeMapView.");
            }
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            Log.v(TAG, "surfaceChanged");
            if (nativeMapViewPtr != 0) {
                nativeSurfaceChanged(nativeMapViewPtr, format, width, height);
            }
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            Log.v(TAG, "surfaceDestroyed");
            if (nativeMapViewPtr != 0) {
                nativeSurfaceDestroyed(nativeMapViewPtr);
            }
            nativeMapViewPtr = 0;
        }
    }

    static {
        System.loadLibrary("NativeMapView");
    }

    private long nativeMapViewPtr;

    private native long nativeSurfaceCreated(Surface surface, String defaultStyleJSON);
    private native void nativeSurfaceDestroyed(long mapViewPtr);
    private native void nativeSurfaceChanged(long mapViewPtr, int format, int width, int height);
    private native void nativeSurfaceRedrawNeeded(long mapViewPtr);

    private native void nativeOnPause(long mapViewPtr);
    private native void nativeOnResume(long mapViewPtr);
    private native void nativeOnStop(long mapViewPtr);
 }
