package com.mapbox.mapboxsdk.maps.widgets;

import android.graphics.drawable.Drawable;
import android.support.annotation.ColorInt;
import android.support.annotation.IntRange;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.maps.MapView;

public class MyLocationViewSettings {

    private MapView mapView;
    private UserLocationView userLocationView;

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

    private int arruracyAlpha;

    @ColorInt
    private int accuracyTintColor;

    //
    // Padding
    //

    private int[] padding = new int[4];

    public MyLocationViewSettings(MapView mapView, UserLocationView userLocationView) {
        this.mapView = mapView;
        this.userLocationView = userLocationView;
    }

    public boolean isEnabled() {
        return enabled;
    }

    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
        userLocationView.setEnabled(enabled);
    }

    public void setForegroundDrawable(Drawable foregroundDrawable, Drawable foregroundBearingDrawable) {
        this.foregroundDrawable = foregroundDrawable;
        this.foregroundBearingDrawable = foregroundBearingDrawable;
        userLocationView.setForegroundDrawables(foregroundDrawable, foregroundBearingDrawable);
    }

    public Drawable getForegroundDrawable() {
        return foregroundDrawable;
    }

    public Drawable getForegroundBearingDrawable() {
        return foregroundBearingDrawable;
    }

    public void setForegroundTintColor(@ColorInt int foregroundTintColor) {
        this.foregroundTintColor = foregroundTintColor;
        userLocationView.setForegroundDrawableTint(foregroundTintColor);
    }

    public int getForegroundTintColor() {
        return foregroundTintColor;
    }

    public void setBackgroundDrawable(Drawable backgroundDrawable) {
        this.backgroundDrawable = backgroundDrawable;
        userLocationView.setShadowDrawable(backgroundDrawable);
    }

    public Drawable getBackgroundDrawable() {
        return backgroundDrawable;
    }

    public void setBackgroundTintColor(@ColorInt int backgroundTintColor) {
        this.backgroundTintColor = backgroundTintColor;
        userLocationView.setShadowDrawableTint(backgroundTintColor);
    }

    public int getBackgroundTintColor() {
        return backgroundTintColor;
    }

    public void setBackgroundOffset(int left, int top, int right, int bottom) {
        backgroundOffset = new int[]{left, top, right, bottom};
        userLocationView.setShadowDrawable(backgroundDrawable, left, top, right, bottom);
    }

    public int[] getBackgroundOffset() {
        return backgroundOffset;
    }

    public void setPadding(int left, int top, int right, int bottom) {
        padding = new int[]{left, top, right, bottom};
        userLocationView.setContentPadding(padding);
        mapView.invalidateContentPadding();
    }

    public int[] getPadding() {
        return padding;
    }

    public int getAccuracyAlpha() {
        return arruracyAlpha;
    }

    public void setAccuracyAlpha(@IntRange(from = 0, to = 255) int arruracyAlpha) {
        this.arruracyAlpha = arruracyAlpha;
        userLocationView.setAccuracyAlpha(arruracyAlpha);
    }

    public int getAccuracyTintColor() {
        return accuracyTintColor;
    }

    public void setAccuracyTintColor(@ColorInt int accuracyTintColor) {
        this.accuracyTintColor = accuracyTintColor;
        userLocationView.setAccuracyTint(accuracyTintColor);
    }

}
