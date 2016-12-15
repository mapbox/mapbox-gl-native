package com.mapbox.weartestapp.activity;

import android.content.Intent;
import android.os.Bundle;
import android.support.wearable.activity.WearableActivity;
import android.support.wearable.view.WearableRecyclerView;

import com.mapbox.weartestapp.R;
import com.mapbox.weartestapp.adapter.FeatureAdapter;
import com.mapbox.weartestapp.model.Feature;
import com.mapbox.weartestapp.utils.OffsettingHelper;

import java.util.ArrayList;
import java.util.List;

public class FeatureOverviewActivity extends WearableActivity implements FeatureAdapter.ItemSelectedListener {

  private WearableRecyclerView wearableRecyclerView;
  private List<Feature> exampleItemModels;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_feature_overview);

    wearableRecyclerView = (WearableRecyclerView) findViewById(R.id.recycler_launcher_view);
    wearableRecyclerView.setHasFixedSize(true);

    OffsettingHelper offsettingHelper = new OffsettingHelper();

    wearableRecyclerView.setOffsettingHelper(offsettingHelper);

    exampleItemModels = new ArrayList<>();
    exampleItemModels.add(new Feature(R.string.activity_simple_mapview_title, new Intent(FeatureOverviewActivity.this,
      SimpleMapViewActivity.class)));

    FeatureAdapter exampleAdapter = new FeatureAdapter(FeatureOverviewActivity.this, exampleItemModels);
    wearableRecyclerView.setAdapter(exampleAdapter);

    exampleAdapter.setListener(this);
  }

  @Override
  public void onItemSelected(int position) {
    startActivity(exampleItemModels.get(position).getActivity());
  }
}
