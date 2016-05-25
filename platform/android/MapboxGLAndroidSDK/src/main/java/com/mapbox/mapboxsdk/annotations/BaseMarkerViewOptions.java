package com.mapbox.mapboxsdk.annotations;

import android.os.Parcelable;
import android.support.annotation.AnimatorRes;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.geometry.LatLng;

/**
 * Abstract builder class for composing custom MarkerView objects.
 * <p>
 * Extending this class requires implementing Parceable interface.
 *
 * @param <U> Type of the marker view to be composed
 * @param <T> Type of the builder to be used for composing a custom Marker
 */
public abstract class BaseMarkerViewOptions<U extends MarkerView, T extends BaseMarkerViewOptions<U, T>> implements Parcelable {

    protected LatLng position;
    protected String snippet;
    protected String title;
    protected Icon icon;
    protected boolean flat;
    protected float anchorU = 0.5f;
    protected float anchorV = 1f;
    protected float infoWindowAnchorU = 0.5f;
    protected float infoWindowAnchorV = 0.0f;
    protected int selectAnimRes;
    protected int deselectAnimRes;
    protected int rotation;
    protected boolean visible = true;

    public BaseMarkerViewOptions() {
    }

    public T position(@NonNull LatLng position) {
        this.position = position;
        return getThis();
    }

    public T snippet(String snippet) {
        this.snippet = snippet;
        return getThis();
    }

    public T title(String title) {
        this.title = title;
        return getThis();
    }

    public T icon(Icon icon) {
        this.icon = icon;
        return getThis();
    }

    public T flat(boolean flat) {
        this.flat = flat;
        return getThis();
    }

    public T anchor(float u, float v) {
        this.anchorU = u;
        this.anchorV = v;
        return getThis();
    }

    public T infoWindowAnchor(float u, float v) {
        this.infoWindowAnchorU = u;
        this.infoWindowAnchorV = v;
        return getThis();
    }

    public T selectAnimatorResource(@AnimatorRes int selectAnimRes) {
        this.selectAnimRes = selectAnimRes;
        return getThis();
    }

    public T deselectAnimatorResource(@AnimatorRes int deselectAnimRes) {
        this.deselectAnimRes = deselectAnimRes;
        return getThis();
    }

    public T rotation(int rotation) {
        this.rotation = rotation;
        return getThis();
    }

    public T visible(boolean visible){
        this.visible = visible;
        return getThis();
    }

    public LatLng getPosition() {
        return position;
    }

    public String getSnippet() {
        return snippet;
    }

    public String getTitle() {
        return title;
    }

    public Icon getIcon() {
        return icon;
    }

    public boolean isFlat() {
        return flat;
    }

    public float getAnchorU() {
        return anchorU;
    }

    public float getAnchorV() {
        return anchorV;
    }

    public float getInfoWindowAnchorU() {
        return infoWindowAnchorU;
    }

    public float getInfoWindowAnchorV() {
        return infoWindowAnchorV;
    }

    public int getSelectAnimRes() {
        return selectAnimRes;
    }

    public int getDeselectAnimRes() {
        return deselectAnimRes;
    }

    public int getRotation() {
        return rotation;
    }

    public boolean isVisible() {
        return visible;
    }

    public abstract T getThis();

    public abstract U getMarker();

}
