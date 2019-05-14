package com.mapbox.mapboxsdk.testapp.utils;

import android.app.Activity;
import android.content.Intent;
import androidx.annotation.NonNull;
import com.mapbox.mapboxsdk.testapp.activity.FeatureOverviewActivity;

public class NavUtils {

  public static void navigateHome(@NonNull Activity context) {
    context.startActivity(new Intent(context, FeatureOverviewActivity.class));
    context.finish();
  }
}
