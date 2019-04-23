package com.mapbox.mapboxsdk.test.render;

import android.app.Application;
import com.mapbox.mapboxsdk.Mapbox;

public class RenderTestApplication extends Application {

  @Override
  public void onCreate() {
    super.onCreate();
    FileUtils.createTestResultRootFolder();
    Mapbox.getInstance(getApplicationContext(), getResources().getString(R.string.mapbox_access_token));
  }
}