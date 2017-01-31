package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentManager;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.design.widget.Snackbar;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapFragment;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.testapp.R;

public class NavigationDrawerActivity extends AppCompatActivity {

  private boolean firstStyle = true;
  private MapboxMap mapboxMap;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_navigation_drawer);

    Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
    setSupportActionBar(toolbar);

    NavigationDrawerFragment navigationDrawerFragment;

    getFragmentManager()
      .beginTransaction()
      .add(R.id.navigation_drawer, navigationDrawerFragment = new NavigationDrawerFragment())
      .commit();

    navigationDrawerFragment.setUp(this,
      R.id.navigation_drawer,
      (DrawerLayout) findViewById(R.id.drawer_layout),
      getSupportActionBar());
  }

  public void onNavigationDrawerItemSelected(int position) {
    // update the main content by replacing fragments
    switch (position) {
      case 0:
        // options
        MapboxMapOptions options = new MapboxMapOptions();
        options.styleUrl(firstStyle ? Style.LIGHT : Style.SATELLITE);
        options.camera(new CameraPosition.Builder()
          .target(new LatLng(39.913271, 116.413891))
          .zoom(12)
          .build());

        // fragment
        MapFragment mapFragment = MapFragment.newInstance(options);
        FragmentManager fragmentManager = getFragmentManager();
        fragmentManager.beginTransaction()
          .replace(R.id.container, mapFragment)
          .commit();

        // get callback when map is ready
        mapFragment.getMapAsync(new OnMapReadyCallback() {
          @Override
          public void onMapReady(MapboxMap map) {
            mapboxMap = map;
          }
        });

        firstStyle = !firstStyle;
        break;
      case 1:
        Snackbar.make(
          findViewById(android.R.id.content),
          "Hello from snackbar",
          Snackbar.LENGTH_LONG)
          .show();
        break;
    }
  }

  public static class NavigationDrawerFragment extends Fragment {

    private static final String STATE_SELECTED_POSITION = "selected_navigation_drawer_position";
    private static final String PREF_USER_LEARNED_DRAWER = "navigation_drawer_learned";

    private ActionBarDrawerToggle drawerToggle;

    private DrawerLayout drawerLayout;
    private ListView drawerListView;
    private View fragmentContainerView;

    private int currentSelectedPosition = 0;
    private boolean fromSavedInstanceState;
    private boolean userLearnedDrawer;

    public NavigationDrawerFragment() {
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
      SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(getActivity());
      userLearnedDrawer = sp.getBoolean(PREF_USER_LEARNED_DRAWER, false);

      if (savedInstanceState != null) {
        currentSelectedPosition = savedInstanceState.getInt(STATE_SELECTED_POSITION);
        fromSavedInstanceState = true;
      }
      selectItem(currentSelectedPosition);
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
      super.onActivityCreated(savedInstanceState);
      setHasOptionsMenu(true);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
      drawerListView = (ListView) inflater.inflate(
        R.layout.drawer_navigation_drawer, container, false);
      drawerListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
          selectItem(position);
        }
      });
      drawerListView.setAdapter(new ArrayAdapter<>(
        inflater.getContext(),
        android.R.layout.simple_list_item_activated_1,
        android.R.id.text1,
        new String[] {
          "Different style",
          "Show snackbar"
        }));
      drawerListView.setItemChecked(currentSelectedPosition, true);
      return drawerListView;
    }

    /**
     * Users of this fragment must call this method to set up the navigation drawer interactions.
     *
     * @param fragmentId   The android:id of this fragment in its activity's layout.
     * @param drawerLayout The DrawerLayout containing this fragment's UI.
     */
    public void setUp(Activity activity, int fragmentId, DrawerLayout drawerLayout, ActionBar actionBar) {
      fragmentContainerView = activity.findViewById(fragmentId);
      this.drawerLayout = drawerLayout;
      // drawerLayout.setScrimColor(Color.TRANSPARENT);

      actionBar.setDisplayHomeAsUpEnabled(true);
      actionBar.setHomeButtonEnabled(true);

      drawerToggle = new ActionBarDrawerToggle(
        activity,
        NavigationDrawerFragment.this.drawerLayout,
        R.string.navigation_drawer_open,
        R.string.navigation_drawer_close
      ) {
        @Override
        public void onDrawerClosed(View drawerView) {
          super.onDrawerClosed(drawerView);
          if (!isAdded()) {
            return;
          }
          getActivity().invalidateOptionsMenu();
        }

        @Override
        public void onDrawerOpened(View drawerView) {
          super.onDrawerOpened(drawerView);
          if (!isAdded()) {
            return;
          }

          if (!userLearnedDrawer) {
            userLearnedDrawer = true;
            SharedPreferences sp = PreferenceManager
              .getDefaultSharedPreferences(getActivity());
            sp.edit().putBoolean(PREF_USER_LEARNED_DRAWER, true).apply();
          }
          getActivity().invalidateOptionsMenu();
        }
      };

      if (!userLearnedDrawer && !fromSavedInstanceState) {
        this.drawerLayout.openDrawer(fragmentContainerView);
      }
      this.drawerLayout.post(new Runnable() {
        @Override
        public void run() {
          drawerToggle.syncState();
        }
      });
      this.drawerLayout.setDrawerListener(drawerToggle);
    }

    private void selectItem(int position) {
      currentSelectedPosition = position;
      if (drawerListView != null) {
        drawerListView.setItemChecked(position, true);
      }
      if (drawerLayout != null) {
        drawerLayout.closeDrawer(fragmentContainerView);
      }

      Activity activity = getActivity();
      if (activity != null && activity instanceof NavigationDrawerActivity) {
        NavigationDrawerActivity navActivity = (NavigationDrawerActivity) activity;
        navActivity.onNavigationDrawerItemSelected(position);
      }
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
      super.onSaveInstanceState(outState);
      outState.putInt(STATE_SELECTED_POSITION, currentSelectedPosition);
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
      super.onConfigurationChanged(newConfig);
      drawerToggle.onConfigurationChanged(newConfig);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
      if (drawerToggle.onOptionsItemSelected(item)) {
        return true;
      }
      return super.onOptionsItemSelected(item);
    }
  }
}
