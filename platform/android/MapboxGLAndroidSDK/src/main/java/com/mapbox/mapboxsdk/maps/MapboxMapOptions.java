package com.mapbox.mapboxsdk.maps;

import android.content.Context;
import android.os.Parcel;
import android.os.Parcelable;
import android.util.AttributeSet;

import com.mapbox.mapboxsdk.camera.CameraPosition;

/**
 * Builder for composing {@link MapboxMap} objects. These options can be used when adding a
 * map to your application programmatically (as opposed to via XML). If you are using a MapFragment,
 * you can pass these options in using the static factory method newInstance(MapboxMapOptions).
 * If you are using a MapView, you can pass these options in using the constructor MapView(Context, MapboxMapOptions).
 */
public class MapboxMapOptions implements Parcelable {

    public MapboxMap mMapboxMap;

    public MapboxMapOptions(MapboxMap mapboxMap) {
        mMapboxMap = mapboxMap;
    }

    public MapboxMapOptions(Parcel in) {
        throw new UnsupportedOperationException();
    }

    public MapboxMapOptions camera(CameraPosition camera) {
        mMapboxMap.setCameraPosition(camera);
        return this;
    }

    public CameraPosition getCamera() {
        return mMapboxMap.getCameraPosition();
    }

    public MapboxMapOptions compassEnabled(boolean enabled) {
        mMapboxMap.setCompassEnabled(enabled);
        return this;
    }

    public boolean getCompassEnabled() {
        return mMapboxMap.isCompassEnabled();
    }

    public MapboxMapOptions rotateEnabled(boolean rotateEnabled) {
        mMapboxMap.setRotateEnabled(rotateEnabled);
        return this;
    }

    public MapboxMapOptions rotateGesturesEnabled(boolean enabled) {
        mMapboxMap.setRotateEnabled(enabled);
        return this;
    }

    public boolean getRotateGesturesEnabled() {
        return mMapboxMap.isRotateEnabled();
    }

    public MapboxMapOptions scrollGesturesEnabled(boolean enabled) {
        mMapboxMap.setScrollEnabled(enabled);
        return this;
    }

    public boolean getScrollGesturesEnabled() {
        return mMapboxMap.isScrollEnabled();
    }

    public MapboxMapOptions tiltGesturesEnabled(boolean enabled) {
        mMapboxMap.setTiltEnabled(enabled);
        return this;
    }

    public boolean getTiltGesturesEnabled() {
        return mMapboxMap.isTiltEnabled();
    }

    public MapboxMapOptions zoomControlsEnabled(boolean enabled) {
        mMapboxMap.setZoomControlsEnabled(enabled);
        return this;
    }

    public boolean getZoomControlsEnabled() {
        return mMapboxMap.isZoomControlsEnabled();
    }

    public boolean getZoomGesturesEnabled() {
        return mMapboxMap.isZoomEnabled();
    }

    public MapboxMapOptions createFromAttributes(Context context, AttributeSet attrs) {
        throw new UnsupportedOperationException();
    }

    public static final Parcelable.Creator<MapboxMapOptions> CREATOR = new Parcelable.Creator<MapboxMapOptions>() {
        public MapboxMapOptions createFromParcel(Parcel in) {
            return new MapboxMapOptions(in);
        }

        public MapboxMapOptions[] newArray(int size) {
            return new MapboxMapOptions[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel out, int flags) {
        throw new UnsupportedOperationException();
    }
}
