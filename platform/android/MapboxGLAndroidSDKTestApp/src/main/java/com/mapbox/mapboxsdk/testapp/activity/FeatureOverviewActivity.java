package com.mapbox.mapboxsdk.testapp.activity;

import android.content.ComponentName;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;

import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;

import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.adapter.FeatureAdapter;
import com.mapbox.mapboxsdk.testapp.adapter.FeatureSectionAdapter;
import com.mapbox.mapboxsdk.testapp.model.activity.Feature;
import com.mapbox.mapboxsdk.testapp.utils.ItemClickSupport;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public class FeatureOverviewActivity extends AppCompatActivity {

    private static final String KEY_STATE_FEATURES = "featureList";

    private RecyclerView recyclerView;
    private FeatureSectionAdapter sectionAdapter;
    private List<Feature> features;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        toolbar.setTitle(getString(R.string.app_name));
        setSupportActionBar(toolbar);

        ActionBar actionBar = getSupportActionBar();
        if(actionBar!=null){
            actionBar.setElevation(getResources().getDimension(R.dimen.toolbar_shadow));
        }

        recyclerView = (RecyclerView) findViewById(R.id.recyclerView);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        recyclerView.addOnItemTouchListener(new RecyclerView.SimpleOnItemTouchListener());
        recyclerView.setHasFixedSize(true);

        ItemClickSupport.addTo(recyclerView).setOnItemClickListener(new ItemClickSupport.OnItemClickListener() {
            @Override
            public void onItemClicked(RecyclerView recyclerView, int position, View v) {
                if (!sectionAdapter.isSectionHeaderPosition(position)) {
                    Intent intent = new Intent();
                    int realPosition = sectionAdapter.getConvertedPosition(position);
                    intent.setComponent(new ComponentName(getPackageName(), features.get(realPosition).getName()));
                    startActivity(intent);
                }
            }
        });

        if (savedInstanceState == null) {
            loadFeatures();
        } else {
            features = savedInstanceState.getParcelableArrayList(KEY_STATE_FEATURES);
            onFeaturesLoaded(features);
        }
    }

    private void loadFeatures() {
        try {
            new LoadFeatureTask().execute(getPackageManager().getPackageInfo(getPackageName(), PackageManager.GET_ACTIVITIES | PackageManager.GET_META_DATA));
        } catch (PackageManager.NameNotFoundException e) {
            Log.e(MapboxConstants.TAG, "Could not resolve package info", e);
        }
    }

    private void onFeaturesLoaded(List<Feature> featuresList) {
        features = featuresList;

        List<FeatureSectionAdapter.Section> sections = new ArrayList<>();
        String currentCat = "";
        for (int i = 0; i < features.size(); i++) {
            String category = features.get(i).getCategory();
            if (!currentCat.equals(category)) {
                sections.add(new FeatureSectionAdapter.Section(i, category));
                currentCat = category;
            }
        }

        FeatureSectionAdapter.Section[] dummy = new FeatureSectionAdapter.Section[sections.size()];
        sectionAdapter = new FeatureSectionAdapter(this, R.layout.section_main_layout, R.id.section_text, new FeatureAdapter(features));
        sectionAdapter.setSections(sections.toArray(dummy));
        recyclerView.setAdapter(sectionAdapter);
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        outState.putParcelableArrayList(KEY_STATE_FEATURES, (ArrayList<Feature>) features);
    }

    private class LoadFeatureTask extends AsyncTask<PackageInfo, Void, List<Feature>> {

        @Override
        protected List<Feature> doInBackground(PackageInfo... params) {
            List<Feature> features = new ArrayList<>();
            PackageInfo app = params[0];

            String metaDataKey = getString(R.string.category);
            for (ActivityInfo info : app.activities) {
                if (!info.name.equals(FeatureOverviewActivity.class.getName())) {
                    String label = getString(info.labelRes);
                    String description = getString(info.descriptionRes);
                    String category = resolveMetaData(info.metaData, metaDataKey);
                    features.add(new Feature(info.name, label, description, category));
                }
            }

            if (!features.isEmpty()) {
                Comparator<Feature> comparator = new Comparator<Feature>() {
                    @Override
                    public int compare(Feature lhs, Feature rhs) {
                        return lhs.getCategory().compareToIgnoreCase(rhs.getCategory());
                    }
                };
                Collections.sort(features, comparator);
            }

            return features;
        }

        private String resolveMetaData(Bundle bundle, String key) {
            String category = null;
            if (bundle != null) {
                category = bundle.getString(key);
            }
            return category;
        }

        @Override
        protected void onPostExecute(List<Feature> features) {
            super.onPostExecute(features);
            onFeaturesLoaded(features);
        }
    }
}