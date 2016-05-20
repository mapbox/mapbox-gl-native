package com.mapbox.mapboxsdk.annotations;

import android.graphics.Point;
import android.graphics.PointF;
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
    protected PointF centerOffset;
    protected Point infoWindowOffset;
    protected int selectAnimRes;
    protected int deselectAnimRes;

    public BaseMarkerViewOptions() {
        centerOffset = new PointF();
        infoWindowOffset = new Point();
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

    public T centerOffset(PointF centerOffset) {
        this.centerOffset = centerOffset;
        return getThis();
    }

    public T infoWindowOffset(Point infoWindowOffset) {
        this.infoWindowOffset = infoWindowOffset;
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

    public PointF getCenterOffset() {
        return centerOffset;
    }

    public Point getInfoWindowOffset() {
        return infoWindowOffset;
    }

    public int getSelectAnimRes() {
        return selectAnimRes;
    }

    public int getDeselectAnimRes() {
        return deselectAnimRes;
    }

    public abstract T getThis();

    public abstract U getMarker();

}
