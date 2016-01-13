package com.mapbox.mapboxsdk.utils;

import android.content.Context;
import android.content.res.ColorStateList;
import android.content.res.Resources;
import android.graphics.drawable.Drawable;
import android.support.annotation.ColorInt;
import android.support.annotation.NonNull;
import android.support.v4.graphics.drawable.DrawableCompat;
import android.util.Log;
import android.util.TypedValue;
import android.view.View;
import android.widget.ImageView;

import com.mapbox.mapboxsdk.R;

public class MaterialUtils {

    @ColorInt
    public static int getPrimaryColor(@NonNull Context context) {
        TypedValue typedValue = new TypedValue();
        Resources.Theme theme = context.getTheme();
        theme.resolveAttribute(R.attr.colorPrimary, typedValue, true);
        Log.v("KLEUR",Integer.toHexString(typedValue.data));
        return typedValue.data;
    }

    @ColorInt
    public static int getPrimaryDarkColor(@NonNull Context context) {
        TypedValue typedValue = new TypedValue();
        Resources.Theme theme = context.getTheme();
        theme.resolveAttribute(R.attr.colorPrimaryDark, typedValue, true);
        return typedValue.data;
    }

    @ColorInt
    public static int getAccentColor(@NonNull Context context) {
        TypedValue typedValue = new TypedValue();
        Resources.Theme theme = context.getTheme();
        theme.resolveAttribute(R.attr.colorAccent, typedValue, true);
        return typedValue.data;
    }

    @NonNull
    public static ColorStateList getSelector(@NonNull Context context){
        return new ColorStateList(
                new int[][]{
                        new int[]{android.R.attr.state_pressed},
                        new int[]{}
                },
                new int[] {
                        getPrimaryColor(context),
                        getPrimaryColor(context)
                }
        );
    }

    public static void setTintList(@NonNull ImageView imageView){
        Drawable originalDrawable = imageView.getDrawable();
        Drawable wrappedDrawable = DrawableCompat.wrap(originalDrawable);
        DrawableCompat.setTintList(wrappedDrawable, getSelector(imageView.getContext()));
    }

    public static void setTint(@NonNull Context context, @NonNull Drawable drawable){
        Drawable wrappedDrawable = DrawableCompat.wrap(drawable);
        DrawableCompat.setTint(wrappedDrawable, MaterialUtils.getPrimaryColor(context));
    }

}
