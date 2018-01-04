package com.mapbox.mapboxsdk.testapp.activity.offline;

import android.content.Context;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.mapbox.mapboxsdk.offline.OfflineManager;
import com.mapbox.mapboxsdk.offline.OfflineRegion;
import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.utils.OfflineUtils;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Test activity showing integration of deleting an OfflineRegion.
 */
public class DeleteRegionActivity extends AppCompatActivity implements AdapterView.OnItemClickListener {

  private OfflineRegionAdapter adapter;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_offline_region_delete);

    ListView listView = (ListView) findViewById(R.id.listView);
    listView.setAdapter(adapter = new OfflineRegionAdapter(this));
    listView.setEmptyView(findViewById(android.R.id.empty));
    listView.setOnItemClickListener(this);
  }

  @Override
  public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
    final OfflineRegion region = adapter.getItem(position);
    String metadata = OfflineUtils.convertRegionName(region.getMetadata());

    AlertDialog.Builder builder = new AlertDialog.Builder(this);
    builder.setTitle("Delete region");

    final TextView input = new TextView(this);
    input.setText(metadata);
    builder.setView(input);

    builder.setPositiveButton("OK", (dialog, which) -> delete(region));
    builder.setNegativeButton("Cancel", (dialog, which) -> dialog.cancel());

    builder.show();
  }

  private void delete(OfflineRegion region) {
    region.delete(new OfflineRegion.OfflineRegionDeleteCallback() {
      @Override
      public void onDelete() {
        Toast.makeText(
          DeleteRegionActivity.this,
          "Region deleted",
          Toast.LENGTH_SHORT
        ).show();
        loadOfflineRegions();
      }

      @Override
      public void onError(String error) {
        Toast.makeText(
          DeleteRegionActivity.this,
          "Region deletion failed with " + error,
          Toast.LENGTH_LONG
        ).show();
      }
    });
  }

  @Override
  protected void onStart() {
    super.onStart();
    loadOfflineRegions();
  }

  private void loadOfflineRegions() {
    OfflineManager.getInstance(this).listOfflineRegions(new OfflineManager.ListOfflineRegionsCallback() {
      @Override
      public void onList(OfflineRegion[] offlineRegions) {
        if (offlineRegions != null && offlineRegions.length > 0) {
          adapter.setOfflineRegions(Arrays.asList(offlineRegions));
        }
      }

      @Override
      public void onError(String error) {
        Toast.makeText(DeleteRegionActivity.this, "Error loading regions " + error, Toast.LENGTH_LONG).show();
      }
    });
  }

  private static class OfflineRegionAdapter extends BaseAdapter {

    private Context context;
    private List<OfflineRegion> offlineRegions;

    OfflineRegionAdapter(Context ctx) {
      context = ctx;
      offlineRegions = new ArrayList<>();
    }

    void setOfflineRegions(List<OfflineRegion> offlineRegions) {
      this.offlineRegions = offlineRegions;
      notifyDataSetChanged();
    }

    @Override
    public int getCount() {
      return offlineRegions.size();
    }

    @Override
    public OfflineRegion getItem(int position) {
      return offlineRegions.get(position);
    }

    @Override
    public long getItemId(int position) {
      return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
      ViewHolder holder;

      if (convertView == null) {
        holder = new ViewHolder();
        convertView = LayoutInflater.from(context).inflate(android.R.layout.simple_list_item_1, parent, false);
        holder.text = (TextView) convertView.findViewById(android.R.id.text1);
        convertView.setTag(holder);
      } else {
        holder = (ViewHolder) convertView.getTag();
      }

      holder.text.setText(OfflineUtils.convertRegionName(getItem(position).getMetadata()));
      return convertView;
    }

    static class ViewHolder {
      TextView text;
    }
  }
}
