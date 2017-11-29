package com.mapbox.mapboxsdk.testapp.activity.offline;

import android.content.Context;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.mapbox.mapboxsdk.testapp.R;

import java.util.List;
import java.util.concurrent.TimeUnit;


public class OfflineRegionListAdapter extends ArrayAdapter<Region> {
  public OfflineRegionListAdapter(@NonNull Context context, int resource) {
    super(context, resource);
  }

  public OfflineRegionListAdapter(@NonNull Context context, int resource, int textViewResourceId, @NonNull List<Region> objects) {
    super(context, resource, textViewResourceId, objects);
  }

  @NonNull
  @Override
  public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {
    if (convertView == null) {
      convertView = LayoutInflater.from(parent.getContext())
        .inflate(R.layout.offline_region_list_item, parent, false);
    }

    Region region = getItem(position);
    TextView name =  (TextView)convertView.findViewById(R.id.area_name);
    name.setText(region.getName());

    TextView sizeText =  (TextView)convertView.findViewById(R.id.size);
    ImageView stateIcon = (ImageView) convertView.findViewById(R.id.state_icon);

    // Display downloaded size and status icon according to last reported status


      // Compute a percentage
      int percentage = region.getRequiredResourceCount() >= 0 ?
        (int)(100.0 * region.getCompletedResourceCount() / region.getRequiredResourceCount()) : 0;

      String summary = getSize(region.getCompletedResourceSize()) + ", " + percentage + " %";
      if (region.isComplete()) {
        summary += " " + TimeUnit.MILLISECONDS.toMinutes(region.getDownloadTime())  + " minutes";
      }
      sizeText.setText(summary);

      if (region.isComplete() || percentage == 100) {
        stateIcon.setImageResource(R.mipmap.ic_play_arrow_black_24dp);

      } else if (region.isDownloadStarted()) {
        stateIcon.setImageResource(R.mipmap.ic_autorenew_black_24dp);

      } else {
        stateIcon.setImageResource(R.mipmap.ic_file_download_black_24dp);
      }


    return convertView;
  }

  static String getSize(long size) {
    if (size == 0) {
      return "0 B";
    } else if (size < 1024) {
      return size + " B";
    } else if (size < 1048576){
      return size / 1024 + " KB";
    } else {
      return size /1048576 + " MB";
    }
  }

}
