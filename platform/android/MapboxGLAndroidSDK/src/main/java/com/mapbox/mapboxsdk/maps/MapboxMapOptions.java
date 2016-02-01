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

    private MapboxMap mMapboxMap;
    private UiSettings mUiSettings;

    public MapboxMapOptions(MapboxMap mapboxMap) {
        mMapboxMap = mapboxMap;
        mUiSettings = mapboxMap.getUiSettings();
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
        mUiSettings.setCompassEnabled(enabled);
        return this;
    }

    public boolean getCompassEnabled() {
        return mUiSettings.isCompassEnabled();
    }

    public MapboxMapOptions rotateEnabled(boolean rotateEnabled) {
        mUiSettings.setRotateGesturesEnabled(rotateEnabled);
        return this;
    }

    public MapboxMapOptions rotateGesturesEnabled(boolean enabled) {
        mUiSettings.setRotateGesturesEnabled(enabled);
        return this;
    }

    public boolean getRotateGesturesEnabled() {
        return mUiSettings.isRotateGesturesEnabled();
    }

    public MapboxMapOptions scrollGesturesEnabled(boolean enabled) {
        mUiSettings.setScrollGesturesEnabled(enabled);
        return this;
    }

    public boolean getScrollGesturesEnabled() {
        return mUiSettings.isScrollGesturesEnabled();
    }

    public MapboxMapOptions tiltGesturesEnabled(boolean enabled) {
        mUiSettings.setTiltGesturesEnabled(enabled);
        return this;
    }

    public boolean getTiltGesturesEnabled() {
        return mUiSettings.isTiltGesturesEnabled();
    }

    public MapboxMapOptions zoomControlsEnabled(boolean enabled) {
        mUiSettings.setZoomControlsEnabled(enabled);
        return this;
    }

    public boolean getZoomControlsEnabled() {
        return mUiSettings.isZoomControlsEnabled();
    }

    public boolean getZoomGesturesEnabled() {
        return mUiSettings.isZoomGesturesEnabled();
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
