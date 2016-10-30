package com.mapbox.mapboxsdk.annotations;

import android.support.annotation.FloatRange;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMap;

/**
 * MarkerView is an annotation that shows an View at a geographical location.
 * <p>
 * This class uses {@link com.mapbox.mapboxsdk.maps.MapboxMap.MarkerViewAdapter} to adapt a
 * MarkerView model to an Android SDK {@link android.view.View} object.
 * </p>
 * <p>
 * An {@link InfoWindow} can be shown when a MarkerView is pressed
 * </p>
 */
public class MarkerView extends Marker {

    private MarkerViewManager markerViewManager;

    private float width;
    private float height;

    private float anchorU;
    private float anchorV;

    private float offsetX = MapboxConstants.UNMEASURED;
    private float offsetY = MapboxConstants.UNMEASURED;

    private float infoWindowAnchorU;
    private float infoWindowAnchorV;

    private boolean flat;
    private boolean visible = true;

    private float tiltValue;
    private float rotation;
    private float alpha = 1;

    private Icon markerViewIcon;

    private boolean selected;


    /**
     * Publicly hidden default constructor
     */
    MarkerView() {
    }

    /**
     * Creates a instance of MarkerView using the builder of MarkerView
     *
     * @param baseMarkerViewOptions the builder used to construct the MarkerView
     */
    public MarkerView(BaseMarkerViewOptions baseMarkerViewOptions) {
        super(baseMarkerViewOptions);
        this.alpha = baseMarkerViewOptions.getAlpha();
        this.anchorU = baseMarkerViewOptions.getAnchorU();
        this.anchorV = baseMarkerViewOptions.getAnchorV();
        this.infoWindowAnchorU = baseMarkerViewOptions.getInfoWindowAnchorU();
        this.infoWindowAnchorV = baseMarkerViewOptions.getInfoWindowAnchorV();
        this.flat = baseMarkerViewOptions.isFlat();
        this.rotation = baseMarkerViewOptions.getRotation();
        this.selected = baseMarkerViewOptions.selected;
    }

    float getWidth() {
        return width;
    }

    void setWidth(float width) {
        this.width = width;
    }

    float getHeight() {
        return height;
    }

    void setHeight(float height) {
        this.height = height;
    }

    /**
     * Specifies the anchor being set on a particular point point of the MarkerView.
     * <p>
     * The anchor point is specified in the continuous space [0.0, 1.0] x [0.0, 1.0], where (0, 0)
     * is the top-left corner of the image, and (1, 1) is the bottom-right corner.
     * </p>
     *
     * @param u u-coordinate of the anchor, as a ratio of the image width (in the range [0, 1])
     * @param v v-coordinate of the anchor, as a ratio of the image height (in the range [0, 1])
     */
    public void setAnchor(@FloatRange(from = 0.0, to = 1.0) float u, @FloatRange(from = 0.0, to = 1.0) float v) {
        this.anchorU = u;
        this.anchorV = v;
        setOffset(-1, -1);
    }

    /**
     * Get the horizontal distance, normalized to [0, 1], of the anchor from the left edge.
     *
     * @return the u-value of the anchor
     */
    public float getAnchorU() {
        return anchorU;
    }

    /**
     * Get the vertical distance, normalized to [0, 1], of the anchor from the top edge.
     *
     * @return the v-value of the anchor
     */
    public float getAnchorV() {
        return anchorV;
    }

    /**
     * Internal method to set the calculated offset.
     * <p>
     * These are calculated based on the View bounds and the provided anchor.
     * </p>
     *
     * @param x the x-value of the offset
     * @param y the y-value of the offset
     */
    void setOffset(float x, float y) {
        offsetX = x;
        offsetY = y;
    }

    /**
     * Internal method to get the horizontal calculated offset
     *
     * @return the calculated horizontal offset
     */
    float getOffsetX() {
        return offsetX;
    }

    /**
     * Internal method to get the vertical calculated offset
     *
     * @return the calculated vertical offset
     */
    float getOffsetY() {
        return offsetY;
    }

    /**
     * Specifies the anchor point of the info window on the View of the MarkerView.
     * <p>
     * This is specified in the same coordinate system as the anchor.
     * </p>
     * <p>
     * The default is the top middle of the View.
     * </p>
     *
     * @param u u-coordinate of the info window anchor, as a ratio of the image width (in the range [0, 1])
     * @param v v-coordinate of the info window anchor, as a ratio of the image height (in the range [0, 1])
     * @see #setAnchor(float, float) for more details.
     */
    public void setInfoWindowAnchor(@FloatRange(from = 0.0, to = 1.0) float u, @FloatRange(from = 0.0, to = 1.0) float v) {
        this.infoWindowAnchorU = u;
        this.infoWindowAnchorV = v;
    }

    /**
     * Get the horizontal distance, normalized to [0, 1], of the info window anchor from the left edge.
     *
     * @return the u value of the InfoWindow anchor.
     */
    public float getInfoWindowAnchorU() {
        return infoWindowAnchorU;
    }

    /**
     * Get the vertical distance, normalized to [0, 1], of the info window anchor from the top edge.
     *
     * @return the v value of the InfoWindow anchor.
     */
    public float getInfoWindowAnchorV() {
        return infoWindowAnchorV;
    }

    /**
     * Get the flat state of a MarkerView.
     *
     * @return true is the MarkerView is flat; false is the MarkerView is billboard
     */
    public boolean isFlat() {
        return flat;
    }

    /**
     * Sets whether this marker should be flat against the map true or a billboard facing the camera false.
     *
     * @param flat the flat state of the MarkerView
     */
    public void setFlat(boolean flat) {
        this.flat = flat;
    }

    /**
     * Internal method to get the current tilted value of a MarkerView.
     *
     * @return the tilted value
     */
    float getTilt() {
        return tiltValue;
    }

    /**
     * Internal method to set the current titled value of a MarkerView.
     *
     * @param tiltValue the tilted value to set
     */
    void setTilt(@FloatRange(from = 0.0, to = MapboxConstants.MAXIMUM_TILT) float tiltValue) {
        this.tiltValue = tiltValue;
    }

    /**
     * Set the visible state of a MarkerView.
     *
     * @param visible true will make the MarkerView visible, false will hide the MarkerViews
     */
    public void setVisible(boolean visible) {
        this.visible = visible;
        if (markerViewManager != null) {
            markerViewManager.animateVisible(this, visible);
        }
    }

    /**
     * Returns the visible state of the MarkerView.
     *
     * @return the visible state
     */
    public boolean isVisible() {
        return visible;
    }

    /**
     * Set the rotation value of the MarkerView in degrees.
     * <p>
     * Input will be limited to 0 - 360 degrees
     * </p>
     * <p>
     * This will result in animating the rotation of the MarkerView using an rotation animator
     * from current value to the provided parameter value.
     * </p>
     *
     * @param rotation the rotation value to animate to
     */
    public void setRotation(float rotation) {
        // limit to 0 - 360 degrees
        float newRotation = rotation;
        while (newRotation > 360) {
            newRotation -= 360;
        }
        while (newRotation < 0) {
            newRotation += 360;
        }

        this.rotation = newRotation;
        if (markerViewManager != null) {
            markerViewManager.animateRotationBy(this, newRotation);
        }
    }

    /**
     * Get the rotation value of the MarkerView.
     *
     * @return the rotation value
     */
    public float getRotation() {
        return rotation;
    }

    /**
     * Get the alpha value of the MarkerView.
     *
     * @return the alpha value
     */
    public float getAlpha() {
        return alpha;
    }

    /**
     * Set the alpha value of the MarkerView.
     * <p>
     * This will result in animating the alpha of the MarkerView using an alpha animator
     * from current value to the provided parameter value.
     * </p>
     *
     * @param alpha the alpha value to animate to
     */
    public void setAlpha(@FloatRange(from = 0.0, to = 255.0) float alpha) {
        this.alpha = alpha;
        if (markerViewManager != null) {
            markerViewManager.animateAlpha(this, alpha);
        }
    }

    /**
     * Set the icon of the MarkerView.
     *
     * @param icon the icon to be used as Marker image
     */
    @Override
    public void setIcon(@Nullable Icon icon) {
        if (icon != null) {
            markerViewIcon = IconFactory.recreate(IconFactory.ICON_MARKERVIEW_ID, icon.getBitmap());
        }
        Icon transparentIcon = IconFactory.recreate(IconFactory.ICON_MARKERVIEW_ID,
                IconFactory.ICON_MARKERVIEW_BITMAP);
        if (markerViewManager != null) {
            markerViewManager.updateIcon(this);
        }
        super.setIcon(transparentIcon);
    }

    @Override
    public void setPosition(LatLng position) {
        super.setPosition(position);
        if (markerViewManager != null) {
            markerViewManager.update();
        }
    }

    public boolean isSelected() {
        return selected;
    }

    /**
     * For internal use only, use {@link MapboxMap#selectMarker(Marker)} instead.
     */
    void setSelected(boolean selected) {
        this.selected = selected;
    }

    /**
     * Get the icon of the MarkerView.
     *
     * @return the icon use as Marker image
     */
    @Override
    public Icon getIcon() {
        return markerViewIcon;
    }

    /**
     * Set the MapboxMap associated tot the MapView containing the MarkerView.
     * <p>
     * This method is used to instantiate the MarkerView and provide an instance of {@link com.mapbox.mapboxsdk.maps.MapboxMap.MarkerViewAdapter}
     * </p>
     * <p>
     * This method is used to notify that a MarkerView is no longer active by setting a null value.
     * </p>
     *
     * @param mapboxMap the MapboxMap instances
     */
    @Override
    public void setMapboxMap(MapboxMap mapboxMap) {
        super.setMapboxMap(mapboxMap);
        if(mapboxMap!=null) {
            if (isFlat()) {
                // initial tilt value if MapboxMap is started with a tilt attribute
                tiltValue = (float) mapboxMap.getCameraPosition().tilt;
            }

            markerViewManager = mapboxMap.getMarkerViewManager();
        }
    }

    /**
     * Get the String representation of a MarkerView.
     *
     * @return the String representation
     */
    @Override
    public String toString() {
        return "MarkerView [position[" + getPosition() + "]]";
    }
}
