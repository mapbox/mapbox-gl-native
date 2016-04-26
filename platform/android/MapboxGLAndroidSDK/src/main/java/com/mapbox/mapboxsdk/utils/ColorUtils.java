package com.mapbox.mapboxsdk.utils;

import android.content.Context;
import android.content.res.ColorStateList;
import android.content.res.Resources;
import android.graphics.drawable.Drawable;
import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.v4.graphics.drawable.DrawableCompat;
import android.util.TypedValue;
import android.widget.ImageView;

import com.mapbox.mapboxsdk.R;

public class ColorUtils {

    /**
     * Returns a color integer associated as primary color from a theme based on a {@link Context}.
     *
     * @param context The context used to style the color attributes.
     * @return The primary color value of current theme in the form 0xAARRGGBB.
     */
    @ColorInt
    public static int getPrimaryColor(@NonNull Context context) {
        TypedValue typedValue = new TypedValue();
        Resources.Theme theme = context.getTheme();
        theme.resolveAttribute(R.attr.colorPrimary, typedValue, true);
        return typedValue.data;
    }

    /**
     * Returns a color integer associated as primary dark color from a theme based on a {@link Context}.
     *
     * @param context The context used to style the color attributes.
     * @return The primary dark color value of current theme in the form 0xAARRGGBB.
     */
    @ColorInt
    public static int getPrimaryDarkColor(@NonNull Context context) {
        TypedValue typedValue = new TypedValue();
        Resources.Theme theme = context.getTheme();
        theme.resolveAttribute(R.attr.colorPrimaryDark, typedValue, true);
        return typedValue.data;
    }

    /**
     * Returns a color integer associated as accent color from a theme based on a {@link Context}.
     *
     * @param context The context used to style the color attributes.
     * @return The accent color value of current theme in the form 0xAARRGGBB.
     */
    @ColorInt
    public static int getAccentColor(@NonNull Context context) {
        TypedValue typedValue = new TypedValue();
        Resources.Theme theme = context.getTheme();
        theme.resolveAttribute(R.attr.colorAccent, typedValue, true);
        return typedValue.data;
    }

    /**
     * Returns a color state list associated with a theme based on a {@link Context}
     *
     * @param color The color used for tinting.
     * @return A ColorStateList object containing the primary color of a theme
     */
    @NonNull
    public static ColorStateList getSelector(@ColorInt int color) {
        return new ColorStateList(
                new int[][]{
                        new int[]{android.R.attr.state_pressed},
                        new int[]{}
                },
                new int[]{
                        color,
                        color
                }
        );
    }

    /**
     * Set a color tint list to the {@link Drawable} of an {@link ImageView}.
     *
     * @param imageView The view to set the default tint list.
     * @param tintColor The color to tint.
     */
    public static void setTintList(@NonNull ImageView imageView, @ColorInt int tintColor) {
        Drawable originalDrawable = imageView.getDrawable();
        Drawable wrappedDrawable = DrawableCompat.wrap(originalDrawable);
        DrawableCompat.setTintList(wrappedDrawable, getSelector(tintColor));
    }
}