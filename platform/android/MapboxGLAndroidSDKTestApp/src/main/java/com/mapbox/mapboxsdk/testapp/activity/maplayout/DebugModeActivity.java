package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.content.Context;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;

import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.MapboxMapOptions;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.layers.Property;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.List;
import java.util.Locale;

import timber.log.Timber;

import static com.mapbox.mapboxsdk.style.layers.PropertyFactory.visibility;

/**
 * Test activity showcasing the different debug modes and allows to cycle between the default map styles.
 */
public class DebugModeActivity extends AppCompatActivity implements OnMapReadyCallback, MapboxMap.OnFpsChangedListener {

  private MapView mapView;
  private MapboxMap mapboxMap;
  private MapboxMap.OnCameraMoveListener cameraMoveListener;
  private ActionBarDrawerToggle actionBarDrawerToggle;
  private int currentStyleIndex;
  private boolean isReportFps = true;

  private static final String[] STYLES = new String[] {
    Style.MAPBOX_STREETS,
    Style.OUTDOORS,
    Style.LIGHT,
    Style.DARK,
    Style.SATELLITE,
    Style.SATELLITE_STREETS,
    Style.TRAFFIC_DAY,
    Style.TRAFFIC_NIGHT
  };
  private TextView fpsView;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_debug_mode);
    setupToolbar();
    setupMapView(savedInstanceState);
    setupDebugChangeView();
    setupStyleChangeView();
  }

  private void setupToolbar() {
    ActionBar actionBar = getSupportActionBar();
    if (actionBar != null) {
      getSupportActionBar().setDisplayHomeAsUpEnabled(true);
      getSupportActionBar().setHomeButtonEnabled(true);
      DrawerLayout drawerLayout = findViewById(R.id.drawer_layout);
      actionBarDrawerToggle = new ActionBarDrawerToggle(this,
        drawerLayout,
        R.string.navigation_drawer_open,
        R.string.navigation_drawer_close
      );
      actionBarDrawerToggle.setDrawerIndicatorEnabled(true);
      actionBarDrawerToggle.syncState();
    }
  }

  private void setupMapView(Bundle savedInstanceState) {
    MapboxMapOptions mapboxMapOptions = setupMapboxMapOptions();
    mapView = new MapView(this, mapboxMapOptions);
    ((ViewGroup) findViewById(R.id.coordinator_layout)).addView(mapView, 0);
    mapView.addOnDidFinishLoadingStyleListener(() -> {
      if (mapboxMap != null) {
        setupNavigationView(mapboxMap.getStyle().getLayers());
      }
    });

    mapView.setTag(true);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
    mapView.addOnDidFinishLoadingStyleListener(() -> Timber.d("Style loaded"));
  }

  protected MapboxMapOptions setupMapboxMapOptions() {
    return MapboxMapOptions.createFromAttributes(this, null);
  }

  @Override
  public void onMapReady(@NonNull MapboxMap map) {
    mapboxMap = map;
    mapboxMap.setStyle(
      new Style.Builder().fromUri(STYLES[currentStyleIndex]), style -> setupNavigationView(style.getLayers())
    );
    setupZoomView();
    setFpsView();
  }

  private void setFpsView() {
    fpsView = findViewById(R.id.fpsView);
    mapboxMap.setOnFpsChangedListener(this);
  }

  @Override
  public void onFpsChanged(double fps) {
    fpsView.setText(String.format(Locale.US, "FPS: %4.2f", fps));
  }

  private void setupNavigationView(List<Layer> layerList) {
    Timber.v("New style loaded with JSON: %s", mapboxMap.getStyle().getJson());
    final LayerListAdapter adapter = new LayerListAdapter(this, layerList);
    ListView listView = findViewById(R.id.listView);
    listView.setAdapter(adapter);
    listView.setOnItemClickListener((parent, view, position, id) -> {
      Layer clickedLayer = adapter.getItem(position);
      toggleLayerVisibility(clickedLayer);
      closeNavigationView();
    });
  }

  private void toggleLayerVisibility(Layer layer) {
    boolean isVisible = layer.getVisibility().getValue().equals(Property.VISIBLE);
    layer.setProperties(
      visibility(
        isVisible ? Property.NONE : Property.VISIBLE
      )
    );
  }

  private void closeNavigationView() {
    DrawerLayout drawerLayout = findViewById(R.id.drawer_layout);
    drawerLayout.closeDrawers();
  }

  private void setupZoomView() {
    final TextView textView = findViewById(R.id.textZoom);
    mapboxMap.addOnCameraMoveListener(cameraMoveListener = new MapboxMap.OnCameraMoveListener() {
      @Override
      public void onCameraMove() {
        textView.setText(String.format(DebugModeActivity.this.getString(
          R.string.debug_zoom), mapboxMap.getCameraPosition().zoom));
      }
    });
  }

  private void setupDebugChangeView() {
    FloatingActionButton fabDebug = findViewById(R.id.fabDebug);
    fabDebug.setOnClickListener(view -> {
      if (mapboxMap != null) {
        Timber.d("Debug FAB: isDebug Active? %s", mapboxMap.isDebugActive());
        mapboxMap.cycleDebugOptions();
      }
    });
  }

  private void setupStyleChangeView() {
    FloatingActionButton fabStyles = findViewById(R.id.fabStyles);
    fabStyles.setOnClickListener(view -> {
      if (mapboxMap != null) {
        currentStyleIndex++;
        if (currentStyleIndex == STYLES.length) {
          currentStyleIndex = 0;
        }
        mapboxMap.setStyle(new Style.Builder().fromUri(STYLES[currentStyleIndex]));
      }
    });
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    int itemId = item.getItemId();
    if (itemId == R.id.menu_action_toggle_report_fps) {
      isReportFps = !isReportFps;
      fpsView.setVisibility(isReportFps ? View.VISIBLE : View.GONE);
      mapboxMap.setOnFpsChangedListener(isReportFps ? this : null);
    } else if (itemId == R.id.menu_action_limit_to_30_fps) {
      mapView.setMaximumFps(30);
    } else if (itemId == R.id.menu_action_limit_to_60_fps) {
      mapView.setMaximumFps(60);
    }

    return actionBarDrawerToggle.onOptionsItemSelected(item) || super.onOptionsItemSelected(item);
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    getMenuInflater().inflate(R.menu.menu_debug, menu);
    return true;
  }

  @Override
  protected void onStart() {
    super.onStart();
    mapView.onStart();
  }

  @Override
  protected void onResume() {
    super.onResume();
    mapView.onResume();
  }

  @Override
  protected void onPause() {
    super.onPause();
    mapView.onPause();
  }

  @Override
  protected void onStop() {
    super.onStop();
    mapView.onStop();
  }

  @Override
  protected void onSaveInstanceState(Bundle outState) {
    super.onSaveInstanceState(outState);
    mapView.onSaveInstanceState(outState);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    if (mapboxMap != null) {
      mapboxMap.removeOnCameraMoveListener(cameraMoveListener);
    }
    mapView.onDestroy();
  }

  @Override
  public void onLowMemory() {
    super.onLowMemory();
    mapView.onLowMemory();
  }

  private static class LayerListAdapter extends BaseAdapter {

    private LayoutInflater layoutInflater;
    private List<Layer> layers;

    LayerListAdapter(Context context, List<Layer> layers) {
      this.layoutInflater = LayoutInflater.from(context);
      this.layers = layers;
    }

    @Override
    public int getCount() {
      return layers.size();
    }

    @Override
    public Layer getItem(int position) {
      return layers.get(position);
    }

    @Override
    public long getItemId(int position) {
      return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
      Layer layer = layers.get(position);
      View view = convertView;
      if (view == null) {
        view = layoutInflater.inflate(android.R.layout.simple_list_item_2, parent, false);
        ViewHolder holder = new ViewHolder(
          view.findViewById(android.R.id.text1),
          view.findViewById(android.R.id.text2)
        );
        view.setTag(holder);
      }
      ViewHolder holder = (ViewHolder) view.getTag();
      holder.text.setText(layer.getClass().getSimpleName());
      holder.subText.setText(layer.getId());
      return view;
    }

    private static class ViewHolder {
      final TextView text;
      final TextView subText;

      ViewHolder(TextView text, TextView subText) {
        this.text = text;
        this.subText = subText;
      }
    }
  }
}
