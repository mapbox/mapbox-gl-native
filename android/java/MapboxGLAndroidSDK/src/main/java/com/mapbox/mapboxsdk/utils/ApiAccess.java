package com.mapbox.mapboxsdk.utils;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.support.annotation.NonNull;
import android.text.TextUtils;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.constants.MapboxConstants;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class ApiAccess {

    public static String getToken(@NonNull Context context) {
        String accessToken = getReleaseToken(context);
        if (TextUtils.isEmpty(accessToken)) {
            accessToken = getDevelopmentToken(context);
        }
        return accessToken;
    }

    private static String getReleaseToken(@NonNull Context context){
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
        }catch (IOException e){
            return null;
        }
    }
}
