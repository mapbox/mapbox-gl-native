package com.mapbox.weartestapp.adapter;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.support.wearable.view.WearableRecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.mapbox.weartestapp.R;
import com.mapbox.weartestapp.model.Feature;

import java.util.List;

public class FeatureAdapter extends WearableRecyclerView.Adapter<FeatureAdapter.ViewHolder> {

  private List<Feature> data;
  private Context context;
  private ItemSelectedListener itemSelectedListener;

  public FeatureAdapter(Context context, List<Feature> data) {
    this.context = context;
    this.data = data;
  }

  static class ViewHolder extends RecyclerView.ViewHolder {

    private TextView textView;

    ViewHolder(View view) {
      super(view);
      textView = (TextView) view.findViewById(R.id.text_item);
    }

    void bind(final int position, final ItemSelectedListener listener) {

      itemView.setOnClickListener(new View.OnClickListener() {
        @Override
        public void onClick(View view) {
          if (listener != null) {
            listener.onItemSelected(position);
          }
        }
      });
    }
  }

  public void setListener(ItemSelectedListener itemSelectedListener) {
    this.itemSelectedListener = itemSelectedListener;
  }

  @Override
  public FeatureAdapter.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
    return new ViewHolder(LayoutInflater.from(parent.getContext())
      .inflate(R.layout.item_curved_layout, parent, false));
  }

  @Override
  public void onBindViewHolder(FeatureAdapter.ViewHolder holder, final int position) {
    if (data != null && !data.isEmpty()) {
      holder.textView.setText(data.get(position).getTitle());
      holder.bind(position, itemSelectedListener);
    }
  }

  @Override
  public int getItemCount() {
    if (data != null && !data.isEmpty()) {
      return data.size();
    }
    return 0;
  }

  public interface ItemSelectedListener {
    void onItemSelected(int position);
  }
}