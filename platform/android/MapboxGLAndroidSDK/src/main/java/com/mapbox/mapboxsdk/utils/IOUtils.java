package com.mapbox.mapboxsdk.utils;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.support.annotation.NonNull;
import android.util.Log;

public class IOUtils {

    private static IOUtils instance = null;
    private Context context = null;

    /**
     * Private Constructor for Singleton
     * @param context Context
     */
    private IOUtils(@NonNull Context context) {
        super();
        this.context = context;
    }

    public static IOUtils getInstance(Context context) {
        if (instance == null) {
            instance = new IOUtils(context);
        }
        return instance;
    }

    /**
     * Determines whether we have an Internet connection available.
     *
     * @return true if there is an Internet connection, false otherwise
     */
    public boolean isConnected() {
        ConnectivityManager cm = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo activeNetwork = cm.getActiveNetworkInfo();
        boolean result = (activeNetwork != null && activeNetwork.isConnected());
        Log.i("IOUtils", "isConnected result = " + result);
        return result;
    }
}
