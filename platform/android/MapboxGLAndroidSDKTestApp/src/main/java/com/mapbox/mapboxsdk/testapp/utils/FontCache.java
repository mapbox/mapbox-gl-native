package com.mapbox.mapboxsdk.testapp.utils;

import android.content.Context;
import android.graphics.Typeface;
import android.util.Log;

import com.mapbox.mapboxsdk.constants.MapboxConstants;

import java.util.Hashtable;

public class FontCache {

    private static Hashtable<String, Typeface> fontCache = new Hashtable<>();

    public static Typeface get(String name, Context context) {
        Typeface tf = fontCache.get(name);
        if (tf == null) {
            try {
                tf = Typeface.createFromAsset(context.getAssets(), name);
                fontCache.put(name, tf);
            } catch (Exception exception) {
                Log.e(MapboxConstants.TAG, "Font not found");
            }
        }
        return tf;
    }
}