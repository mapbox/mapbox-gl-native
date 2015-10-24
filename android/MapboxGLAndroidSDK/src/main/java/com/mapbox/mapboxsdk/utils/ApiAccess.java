package com.mapbox.mapboxsdk.utils;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.text.TextUtils;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.constants.MapboxConstants;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

/**
 * {@code ApiAccess} provides a method to load the Mapbox access token.
 */
public final class ApiAccess {

    /**
     * Returns the Mapbox access token set in the app resources.
     * <p/>
     * It will first search the application manifest for a {@link MapboxConstants#KEY_META_DATA_MANIFEST}
     * meta-data value. If not found it will then attempt to load the access token from the
     * {@code res/raw/token.txt} development file.
     *
     * @param context The {@link Context} of the {@link android.app.Activity} or {@link android.app.Fragment}.
     * @return The Mapbox access token or null if not found.
     * @see MapboxConstants#KEY_META_DATA_MANIFEST
     */
    @Nullable
    public static String getToken(@NonNull Context context) {
        String accessToken = getReleaseToken(context);
        if (TextUtils.isEmpty(accessToken)) {
            accessToken = getDevelopmentToken(context);
        }
        return accessToken;
    }

    private static String getReleaseToken(@NonNull Context context) {
        try {
            PackageManager packageManager = context.getPackageManager();
            ApplicationInfo appInfo = packageManager.getApplicationInfo(context.getPackageName(), PackageManager.GET_META_DATA);
            return appInfo.metaData.getString(MapboxConstants.KEY_META_DATA_MANIFEST);
        } catch (Exception e) {
            return null;
        }
    }

    private static String getDevelopmentToken(@NonNull Context context) {
        try {
            BufferedReader reader = new BufferedReader(new InputStreamReader(context.getResources().openRawResource(R.raw.token)));
            return reader.readLine();
        } catch (IOException e) {
            return null;
        }
    }
}
