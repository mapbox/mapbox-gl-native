package com.mapbox.mapboxsdk.testapp;

import android.app.Application;

import com.squareup.leakcanary.LeakCanary;

public class MapboxApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        LeakCanary.install(this);
    }
}
