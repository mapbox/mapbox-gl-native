package com.mapbox.mapboxsdk.annotations;

import android.os.Parcelable;
import android.support.annotation.NonNull;

import com.mapbox.mapboxsdk.geometry.LatLng;

/**
 * Abstract builder class for composing custom MarkerView objects.
 * <p>
 * Extending this class requires implementing Parceable interface.
 * </p>
 *
 * @param <U> Type of the marker view to be composed
 * @param <T> Type of the builder to be used for composing
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
    protected int rotation;
    protected boolean visible = true;
    protected boolean selected;

    /**
     * Default constructor
     */
    public BaseMarkerViewOptions() {
    }

    /**
     * Set the geographical location of the MarkerView.
     *
     * @param position the location to position the MarkerView
     * @return the object for which the method was called
     */
    public T position(@NonNull LatLng position) {
        this.position = position;
        return getThis();
    }

    /**
     * Set the snippet of the MarkerView.
     *
     * @param snippet the snippet of the MarkerView
     * @return the object for which the method was called
     */
    public T snippet(String snippet) {
        this.snippet = snippet;
        return getThis();
    }

    /**
     * Set the title of the MarkerView.
     *
     * @param title the title of the MarkerView
     * @return the object for which the method was called
     */
    public T title(String title) {
        this.title = title;
        return getThis();
    }

    /**
     * Set the icon of the MarkerView.
     *
     * @param icon the icon of the MarkerView
     * @return the object for which the method was called
     */
    public T icon(Icon icon) {
        this.icon = icon;
        return getThis();
    }

    /**
     * Set the flat state of the MarkerView.
     *
     * @param flat the flat state of the MarkerView
     * @return the object for which the method was called
     */
    public T flat(boolean flat) {
        this.flat = flat;
        return getThis();
    }

    /**
     * Set the anchor of the MarkerView.
     *
     * @param u the u-value
     * @param v the v-value
     * @return the object for which the method was called
     */
    public T anchor(float u, float v) {
        this.anchorU = u;
        this.anchorV = v;
        return getThis();
    }

    /**
     * Set the InfoWindow anchor of the MarkerView.
     *
     * @param u the u-value
     * @param v the v-values
     * @return the object for which the method was called
     */
    public T infoWindowAnchor(float u, float v) {
        this.infoWindowAnchorU = u;
        this.infoWindowAnchorV = v;
        return getThis();
    }

    /**
     * Set the rotation of the MarkerView.
     *
     * @param rotation the rotation value
     * @return the object for which the method was called
     */
    public T rotation(int rotation) {
        this.rotation = rotation;
        return getThis();
    }

    /**
     * Set the visibility state of the MarkerView.
     *
     * @param visible the visible state
     * @return the object for which the method was calleds
     */
    public T visible(boolean visible) {
        this.visible = visible;
        return getThis();
    }

    /**
     * Get the geographical location of the MarkerView.
     *
     * @return the geographical location
     */
    public LatLng getPosition() {
        return position;
    }

    /**
     * Get the snippet of the MarkerView.
     *
     * @return the snippet
     */
    public String getSnippet() {
        return snippet;
    }

    /**
     * Get the title of the MarkerView.
     *
     * @return the title
     */
    public String getTitle() {
        return title;
    }

    /**
     * Get the icon of the MarkerView.
     *
     * @return the icon
     */
    public Icon getIcon() {
        return icon;
    }

    /**
     * Get the flat state of the MarkerView.
     *
     * @return the flat state
     */
    public boolean isFlat() {
        return flat;
    }

    /**
     * Get the u-value of the MarkerView anchor.
     *
     * @return the u-value
     */
    public float getAnchorU() {
        return anchorU;
    }

    /**
     * Get the v-value of the MarkerView anchor.
     *
     * @return the v-value
     */
    public float getAnchorV() {
        return anchorV;
    }

    /**
     * Get the u-value of the MarkerView InfoWindow anchor.
     *
     * @return the u-value
     */
    public float getInfoWindowAnchorU() {
        return infoWindowAnchorU;
    }

    /**
     * Get the v-value of the MarkerView InfoWindow anchor.
     *
     * @return the v-value
     */
    public float getInfoWindowAnchorV() {
        return infoWindowAnchorV;
    }

    /**
     * Get the rotation of the MarkerView.
     *
     * @return the rotation value
     */
    public int getRotation() {
        return rotation;
    }

    /**
     * Get the visibility state of the MarkerView.
     *
     * @return the visibility state
     */
    public boolean isVisible() {
        return visible;
    }

    /**
     * Get the instance of the object for which this method was called.
     *
     * @return the object for which the this method was called
     */
    public abstract T getThis();

    /**
     * Get the MarkerView.
     *
     * @return the MarkerView created from this builder
     */
    public abstract U getMarker();
}
