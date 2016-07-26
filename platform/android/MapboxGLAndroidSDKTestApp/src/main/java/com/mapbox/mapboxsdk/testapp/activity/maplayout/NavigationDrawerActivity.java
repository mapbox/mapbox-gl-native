package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentManager;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.graphics.Color;
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
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.testapp.R;

public class NavigationDrawerActivity extends AppCompatActivity {

    private boolean firstStyle = true;

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
                MapboxMapOptions options = new MapboxMapOptions();
                options.styleUrl(firstStyle ? Style.LIGHT : Style.SATELLITE);
                options.camera(new CameraPosition.Builder()
                        .target(new LatLng(39.913271, 116.413891))
                        .zoom(12)
                        .build());
                FragmentManager fragmentManager = getFragmentManager();
                fragmentManager.beginTransaction()
                        .replace(R.id.container, MapFragment.newInstance(options))
                        .commit();

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

        private ActionBarDrawerToggle mDrawerToggle;

        private DrawerLayout mDrawerLayout;
        private ListView mDrawerListView;
        private View mFragmentContainerView;

        private int mCurrentSelectedPosition = 0;
        private boolean mFromSavedInstanceState;
        private boolean mUserLearnedDrawer;

        public NavigationDrawerFragment() {
        }

        @Override
        public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(getActivity());
            mUserLearnedDrawer = sp.getBoolean(PREF_USER_LEARNED_DRAWER, false);

            if (savedInstanceState != null) {
                mCurrentSelectedPosition = savedInstanceState.getInt(STATE_SELECTED_POSITION);
                mFromSavedInstanceState = true;
            }
            selectItem(mCurrentSelectedPosition);
        }

        @Override
        public void onActivityCreated(Bundle savedInstanceState) {
            super.onActivityCreated(savedInstanceState);
            setHasOptionsMenu(true);
        }

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                                 Bundle savedInstanceState) {
            mDrawerListView = (ListView) inflater.inflate(
                    R.layout.drawer_navigation_drawer, container, false);
            mDrawerListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
                @Override
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    selectItem(position);
                }
            });
            mDrawerListView.setAdapter(new ArrayAdapter<>(
                    inflater.getContext(),
                    android.R.layout.simple_list_item_activated_1,
                    android.R.id.text1,
                    new String[]{
                            getString(R.string.title_section1),
                            getString(R.string.title_section2)
                    }));
            mDrawerListView.setItemChecked(mCurrentSelectedPosition, true);
            return mDrawerListView;
        }

        /**
         * Users of this fragment must call this method to set up the navigation drawer interactions.
         *
         * @param fragmentId   The android:id of this fragment in its activity's layout.
         * @param drawerLayout The DrawerLayout containing this fragment's UI.
         */
        public void setUp(Activity activity, int fragmentId, DrawerLayout drawerLayout, ActionBar actionBar) {
            mFragmentContainerView = activity.findViewById(fragmentId);
            mDrawerLayout = drawerLayout;
//            mDrawerLayout.setScrimColor(Color.TRANSPARENT);

            actionBar.setDisplayHomeAsUpEnabled(true);
            actionBar.setHomeButtonEnabled(true);

            mDrawerToggle = new ActionBarDrawerToggle(
                    activity,
                    mDrawerLayout,
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

                    if (!mUserLearnedDrawer) {
                        mUserLearnedDrawer = true;
                        SharedPreferences sp = PreferenceManager
                                .getDefaultSharedPreferences(getActivity());
                        sp.edit().putBoolean(PREF_USER_LEARNED_DRAWER, true).apply();
                    }
                    getActivity().invalidateOptionsMenu();
                }
            };

            if (!mUserLearnedDrawer && !mFromSavedInstanceState) {
                mDrawerLayout.openDrawer(mFragmentContainerView);
            }
            mDrawerLayout.post(new Runnable() {
                @Override
                public void run() {
                    mDrawerToggle.syncState();
                }
            });
            mDrawerLayout.setDrawerListener(mDrawerToggle);
        }

        private void selectItem(int position) {
            mCurrentSelectedPosition = position;
            if (mDrawerListView != null) {
                mDrawerListView.setItemChecked(position, true);
            }
            if (mDrawerLayout != null) {
                mDrawerLayout.closeDrawer(mFragmentContainerView);
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
            outState.putInt(STATE_SELECTED_POSITION, mCurrentSelectedPosition);
        }

        @Override
        public void onConfigurationChanged(Configuration newConfig) {
            super.onConfigurationChanged(newConfig);
            mDrawerToggle.onConfigurationChanged(newConfig);
        }

        @Override
        public boolean onOptionsItemSelected(MenuItem item) {
            if (mDrawerToggle.onOptionsItemSelected(item)) {
                return true;
            }
            return super.onOptionsItemSelected(item);
        }
    }
}
