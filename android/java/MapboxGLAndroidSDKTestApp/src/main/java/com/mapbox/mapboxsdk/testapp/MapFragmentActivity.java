package com.mapbox.mapboxsdk.testapp;

import android.content.Context;
import android.os.Bundle;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import com.mapbox.mapboxsdk.views.MapView;

public class MapFragmentActivity extends AppCompatActivity {

    private static final String TAG_FRAGMENT = "map";
    private MyMapFragment mMapFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map_fragment);

        if (savedInstanceState == null) {
            FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
            transaction.add(R.id.fragment_container, mMapFragment = new MyMapFragment(), TAG_FRAGMENT);
            transaction.commit();
        } else {
            mMapFragment = (MyMapFragment) getSupportFragmentManager().findFragmentByTag(TAG_FRAGMENT);
        }
    }

    public static class MyMapFragment extends MapFragment {

        @Override
        public void onViewCreated(View view, Bundle savedInstanceState) {
            super.onViewCreated(view, savedInstanceState);
            Context context = view.getContext();

            MapView map = getMap();
            map.setAccessToken(Util.getAccessToken(context));
            map.setStyleUrl(MapView.StyleUrls.EMERALD);
        }
    }
}
