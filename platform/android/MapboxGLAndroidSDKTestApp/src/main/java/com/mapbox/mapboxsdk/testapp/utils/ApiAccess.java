package com.mapbox.mapboxsdk.testapp.utils;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.constants.MapboxConstants;

/**
 * {@code ApiAccess} provides a method to load the Mapbox access token.
 */
public final class ApiAccess {

    /**
     * <p>
     * Returns the Mapbox access token set in the app resources.
     * </p>
     * It will first search the application manifest for a {@link MapboxConstants#KEY_META_DATA_MANIFEST}
     * meta-data value. If not found it will then attempt to load the access token from the
     * {@code res/raw/token.txt} development file.
     *
     * @param context The {@link Context} of the {@link android.app.Activity} or {@link android.app.Fragment}.
     * @return The Mapbox access token or null if not found.
     * @see MapboxConstants#KEY_META_DATA_MANIFEST
     */
    public static String getToken(@NonNull Context context) {
        try {
            // read out AndroidManifest
            PackageManager packageManager = context.getPackageManager();
            ApplicationInfo appInfo = packageManager.getApplicationInfo(context.getPackageName(), PackageManager.GET_META_DATA);
            String token = appInfo.metaData.getString(MapboxConstants.KEY_META_DATA_MANIFEST);
            if (token == null || token.isEmpty()) {
                throw new IllegalArgumentException();
            }
            return token;
        } catch (Exception e) {
            // use fallback on string resource, used for development
            int tokenResId = context.getResources().getIdentifier("mapbox_access_token", "string", context.getPackageName());
            return tokenResId != 0 ? context.getString(tokenResId) : null;
        }
    }
}
