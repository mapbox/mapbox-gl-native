package com.mapbox.mapboxsdk.testapp.activity.fragment;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.v7.app.AppCompatActivity;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.SupportMapFragment;
import com.mapbox.mapboxsdk.testapp.R;

/**
 * Test activity showcasing using the Android SDK ViewPager API to show MapFragments.
 */
public class ViewPagerActivity extends AppCompatActivity {

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_viewpager);

    ViewPager viewPager = (ViewPager) findViewById(R.id.viewpager);
    if (viewPager != null) {
      MapFragmentAdapter adapter = new MapFragmentAdapter(getSupportFragmentManager());
      viewPager.setAdapter(adapter);
    }
  }

  static class MapFragmentAdapter extends FragmentPagerAdapter {

    private static int NUM_ITEMS = 3;

    MapFragmentAdapter(FragmentManager fragmentManager) {
      super(fragmentManager);
    }

    @Override
    public int getCount() {
      return NUM_ITEMS;
    }

    @Override
    public Fragment getItem(int position) {
      SupportMapFragment fragment = null;
      MapboxMapOptions options = new MapboxMapOptions();
      options.textureMode(true);

      switch (position) {
        case 0:
          options.styleUrl(Style.MAPBOX_STREETS);
          options.camera(new CameraPosition.Builder().target(new LatLng(34.920526, 102.634774)).zoom(3).build());
          fragment = SupportMapFragment.newInstance(options);
          break;
        case 1:
          options.styleUrl(Style.DARK);
          options.camera(new CameraPosition.Builder().target(new LatLng(62.326440, 92.764913)).zoom(3).build());
          fragment = SupportMapFragment.newInstance(options);
          break;
        case 2:
          options.styleUrl(Style.SATELLITE);
          options.camera(new CameraPosition.Builder().target(new LatLng(-25.007786, 133.623852)).zoom(3).build());
          fragment = SupportMapFragment.newInstance(options);
          break;
      }
      return fragment;
    }

    @Override
    public CharSequence getPageTitle(int position) {
      return "Page " + position;
    }
  }
}

