package com.mapbox.mapboxsdk.maps.widgets;

import android.graphics.drawable.Drawable;
import android.support.annotation.ColorInt;
import android.support.annotation.IntRange;

import com.mapbox.mapboxsdk.maps.MapView;

public class MyLocationViewSettings {

    private MapView mapView;
    private MyLocationView myLocationView;

    //
    // State
    //

    private boolean enabled;

    //
    // Foreground
    //

    private Drawable foregroundDrawable;
    private Drawable foregroundBearingDrawable;

    @ColorInt
    private int foregroundTintColor;

    //
    // Background
    //

    private Drawable backgroundDrawable;
    private int[] backgroundOffset = new int[4];

    @ColorInt
    private int backgroundTintColor;

    //
    // Accuracy
    //

    private int accuracyAlpha;

    @ColorInt
    private int accuracyTintColor;

    //
    // Padding
    //

    private int[] padding = new int[4];

    public MyLocationViewSettings(MapView mapView, MyLocationView myLocationView) {
        this.mapView = mapView;
        this.myLocationView = myLocationView;
    }

    public boolean isEnabled() {
        return enabled;
    }

    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
        myLocationView.setEnabled(enabled);
    }

    public void setForegroundDrawable(Drawable foregroundDrawable, Drawable foregroundBearingDrawable) {
        this.foregroundDrawable = foregroundDrawable;
        this.foregroundBearingDrawable = foregroundBearingDrawable;
        myLocationView.setForegroundDrawables(foregroundDrawable, foregroundBearingDrawable);
    }

    public Drawable getForegroundDrawable() {
        return foregroundDrawable;
    }

    public Drawable getForegroundBearingDrawable() {
        return foregroundBearingDrawable;
    }

    public void setForegroundTintColor(@ColorInt int foregroundTintColor) {
        this.foregroundTintColor = foregroundTintColor;
        myLocationView.setForegroundDrawableTint(foregroundTintColor);
    }

    public int getForegroundTintColor() {
        return foregroundTintColor;
    }

    public void setBackgroundDrawable(Drawable backgroundDrawable, int[] padding) {
        this.backgroundDrawable = backgroundDrawable;
        this.backgroundOffset = padding;
        if (padding != null && padding.length == 4) {
            myLocationView.setShadowDrawable(backgroundDrawable, padding[0], padding[1], padding[2], padding[3]);
        } else {
            myLocationView.setShadowDrawable(backgroundDrawable);
        }
    }

    public Drawable getBackgroundDrawable() {
        return backgroundDrawable;
    }

    public void setBackgroundTintColor(@ColorInt int backgroundTintColor) {
        this.backgroundTintColor = backgroundTintColor;
        myLocationView.setShadowDrawableTint(backgroundTintColor);
    }

    public int getBackgroundTintColor() {
        return backgroundTintColor;
    }

    public int[] getBackgroundOffset() {
        return backgroundOffset;
    }

    public void setPadding(int left, int top, int right, int bottom) {
        padding = new int[]{left, top, right, bottom};
        myLocationView.setContentPadding(padding);
        mapView.invalidateContentPadding();
    }

    public int[] getPadding() {
        return padding;
    }

    public int getAccuracyAlpha() {
        return accuracyAlpha;
    }

    public void setAccuracyAlpha(@IntRange(from = 0, to = 255) int arruracyAlpha) {
        this.accuracyAlpha = arruracyAlpha;
        myLocationView.setAccuracyAlpha(arruracyAlpha);
    }

    public int getAccuracyTintColor() {
        return accuracyTintColor;
    }

    public void setAccuracyTintColor(@ColorInt int accuracyTintColor) {
        this.accuracyTintColor = accuracyTintColor;
        myLocationView.setAccuracyTint(accuracyTintColor);
    }
}
