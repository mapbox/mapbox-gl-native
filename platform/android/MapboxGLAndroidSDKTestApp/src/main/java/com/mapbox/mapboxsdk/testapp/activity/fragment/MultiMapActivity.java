package com.mapbox.mapboxsdk.testapp.activity.fragment;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test Activity showcasing using multiple static map fragments in one layout.
 */
public class MultiMapActivity extends AppCompatActivity {

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_multi_map);
  }
}
