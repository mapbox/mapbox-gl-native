package com.mapbox.mapboxsdk.testapp.activity.offline;

import android.content.Context;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.text.InputType;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.EditText;
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
 * Test activity showing integration of updating metadata of an OfflineRegion.
 */
public class UpdateMetadataActivity extends AppCompatActivity implements AdapterView.OnItemClickListener {

  private OfflineRegionMetadataAdapter adapter;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_metadata_update);

    ListView listView = (ListView) findViewById(R.id.listView);
    listView.setAdapter(adapter = new OfflineRegionMetadataAdapter(this));
    listView.setEmptyView(findViewById(android.R.id.empty));
    listView.setOnItemClickListener(this);
  }

  @Override
  public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
    final OfflineRegion region = adapter.getItem(position);
    String metadata = OfflineUtils.convertRegionName(region.getMetadata());

    AlertDialog.Builder builder = new AlertDialog.Builder(this);
    builder.setTitle("Rename metadata");

    final EditText input = new EditText(this);
    input.setText(metadata);
    input.setInputType(InputType.TYPE_CLASS_TEXT);
    input.setSelection(metadata.length());
    builder.setView(input);

    builder.setPositiveButton("OK", (dialog, which) ->
      updateMetadata(region, OfflineUtils.convertRegionName(input.getText().toString()))
    );
    builder.setNegativeButton("Cancel", (dialog, which) -> dialog.cancel());

    builder.show();
  }

  private void updateMetadata(OfflineRegion region, byte[] metadata) {
    region.updateMetadata(metadata, new OfflineRegion.OfflineRegionUpdateMetadataCallback() {
      @Override
      public void onUpdate(byte[] metadata) {
        adapter.notifyDataSetChanged();
      }

      @Override
      public void onError(String error) {
        Toast.makeText(
          UpdateMetadataActivity.this,
          "Region metadata update failed with " + error,
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
        Toast.makeText(UpdateMetadataActivity.this, "Error loading regions " + error, Toast.LENGTH_LONG).show();
      }
    });
  }

  private static class OfflineRegionMetadataAdapter extends BaseAdapter {

    private Context context;
    private List<OfflineRegion> offlineRegions;

    OfflineRegionMetadataAdapter(Context ctx) {
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
