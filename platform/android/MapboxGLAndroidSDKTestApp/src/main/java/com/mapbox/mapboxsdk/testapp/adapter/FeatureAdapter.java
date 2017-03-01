package com.mapbox.mapboxsdk.testapp.adapter;

import android.graphics.Typeface;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.model.activity.Feature;
import com.mapbox.mapboxsdk.testapp.utils.FontCache;

import java.util.List;

/**
 * Adapter used for FeatureOverviewActivity.
 * <p>
 * Adapts a Feature to a visual representation to be shown in a RecyclerView.
 * </p>
 */
public class FeatureAdapter extends RecyclerView.Adapter<FeatureAdapter.ViewHolder> {

  private List<Feature> features;

  public static class ViewHolder extends RecyclerView.ViewHolder {

    public TextView labelView;
    public TextView descriptionView;

    public ViewHolder(View view) {
      super(view);
      Typeface typeface = FontCache.get("Roboto-Regular.ttf", view.getContext());
      labelView = (TextView) view.findViewById(R.id.nameView);
      labelView.setTypeface(typeface);
      descriptionView = (TextView) view.findViewById(R.id.descriptionView);
      descriptionView.setTypeface(typeface);
    }
  }

  public FeatureAdapter(List<Feature> features) {
    this.features = features;
  }

  @Override
  public FeatureAdapter.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
    View view = LayoutInflater.from(parent.getContext()).inflate(R.layout.item_main_feature, parent, false);
    return new ViewHolder(view);
  }

  @Override
  public void onBindViewHolder(ViewHolder holder, int position) {
    holder.labelView.setText(features.get(position).getLabel());
    holder.descriptionView.setText(features.get(position).getDescription());
  }

  @Override
  public int getItemCount() {
    return features.size();
  }
}
