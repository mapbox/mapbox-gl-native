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

public class FeatureAdapter extends RecyclerView.Adapter<FeatureAdapter.ViewHolder> {

    private List<Feature> features;

    public static class ViewHolder extends RecyclerView.ViewHolder {

        public TextView labelView;
        public TextView descriptionView;

        public ViewHolder(View v) {
            super(v);
            Typeface typeface = FontCache.get("Roboto-Regular.ttf",v.getContext());
            labelView = (TextView) v.findViewById(R.id.nameView);
            labelView.setTypeface(typeface);
            descriptionView = (TextView) v.findViewById(R.id.descriptionView);
            descriptionView.setTypeface(typeface);
        }
    }

    public FeatureAdapter(List<Feature> features) {
        this.features = features;
    }

    @Override
    public FeatureAdapter.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View v = LayoutInflater.from(parent.getContext()).inflate(R.layout.item_main_feature, parent, false);
        return new ViewHolder(v);
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