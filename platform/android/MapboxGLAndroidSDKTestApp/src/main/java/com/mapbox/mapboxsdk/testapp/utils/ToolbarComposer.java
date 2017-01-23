package com.mapbox.mapboxsdk.testapp.utils;

import android.support.annotation.IdRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;

import com.mapbox.mapboxsdk.testapp.R;

/**
 * Helper class to compose a Toolbar in an AppCompatActivity
 */
public class ToolbarComposer {


  /**
   * Initialises an up navigation toolbar with id R.id.toolbar on an AppCompatActivity.
   *
   * @param activity The activity hosting the Toolbar with id R.id.toolbar
   */
  @Nullable
  public static Toolbar initDefaultUpToolbar(@NonNull AppCompatActivity activity) {
    return initDefaultUpToolbar(activity, R.id.toolbar);
  }

  /**
   * Initialises an up navigation toolbar given a view id on an AppCompatActivity.
   *
   * @param activity   The activity hosting the Toolbar
   * @param toolbarRes The view id resource used to look up the Toolbar
   */
  @Nullable
  public static Toolbar initDefaultUpToolbar(@NonNull AppCompatActivity activity, @IdRes int toolbarRes) {
    Toolbar toolbar = (Toolbar) activity.findViewById(toolbarRes);
    if (toolbar != null) {
      activity.setSupportActionBar(toolbar);

      ActionBar actionBar = activity.getSupportActionBar();
      if (actionBar != null) {
        actionBar.setDisplayHomeAsUpEnabled(true);
        actionBar.setDisplayShowHomeEnabled(true);
      }
    }
    return toolbar;
  }

}
