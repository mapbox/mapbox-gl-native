package com.mapbox.mapboxsdk.plugins.locationlayer;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.drawable.Drawable;
import android.support.annotation.ColorInt;
import android.support.annotation.DrawableRes;
import android.support.v4.content.ContextCompat;

import com.mapbox.mapboxsdk.R;

import static com.mapbox.mapboxsdk.plugins.locationlayer.Utils.generateShadow;
import static com.mapbox.mapboxsdk.plugins.locationlayer.Utils.getBitmapFromDrawable;
import static com.mapbox.mapboxsdk.plugins.locationlayer.Utils.getDrawable;

class LayerBitmapProvider {

  private final Context context;

  LayerBitmapProvider(Context context) {
    this.context = context;
  }

  Bitmap generateBitmap(@DrawableRes int drawableRes, @ColorInt Integer tintColor) {
    Drawable drawable = getDrawable(context, drawableRes, tintColor);
    return getBitmapFromDrawable(drawable);
  }

  Bitmap generateShadowBitmap(LocationLayerOptions options) {
    Drawable shadowDrawable = ContextCompat.getDrawable(context, R.drawable.mapbox_user_icon_shadow);
    return generateShadow(shadowDrawable, options.elevation());
  }
}
