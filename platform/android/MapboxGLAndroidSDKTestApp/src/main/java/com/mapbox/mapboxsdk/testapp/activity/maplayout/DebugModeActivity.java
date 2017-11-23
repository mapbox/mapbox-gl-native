package com.mapbox.mapboxsdk.testapp.activity.maplayout;

import android.content.Context;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;

import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.maps.OnMapReadyCallback;
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
public class DebugModeActivity extends AppCompatActivity implements OnMapReadyCallback {

  private MapView mapView;
  private MapboxMap mapboxMap;
  private ActionBarDrawerToggle actionBarDrawerToggle;
  private int currentStyleIndex = 0;

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

      DrawerLayout drawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);
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
    mapView = (MapView) findViewById(R.id.mapView);
    mapView.addOnMapChangedListener(change -> {
      if (change == MapView.DID_FINISH_LOADING_STYLE && mapboxMap != null) {
        Timber.v("New style loaded with JSON: %s", mapboxMap.getStyleJson());
        setupNavigationView(mapboxMap.getLayers());
      }
    });

    mapView.setTag(true);
    mapView.setStyleUrl(STYLES[currentStyleIndex]);
    mapView.onCreate(savedInstanceState);
    mapView.getMapAsync(this);
  }

  @Override
  public void onMapReady(MapboxMap map) {
    mapboxMap = map;
    mapboxMap.getUiSettings().setZoomControlsEnabled(true);

    setupNavigationView(mapboxMap.getLayers());
    setupZoomView();
    setFpsView();
  }

  private void setFpsView() {
    final TextView fpsView = (TextView) findViewById(R.id.fpsView);
    mapboxMap.setOnFpsChangedListener(fps ->
      fpsView.setText(String.format(Locale.US, "FPS: %4.2f", fps))
    );
  }

  private void setupNavigationView(List<Layer> layerList) {
    final LayerListAdapter adapter = new LayerListAdapter(this, layerList);
    ListView listView = (ListView) findViewById(R.id.listView);
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
    DrawerLayout drawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);
    drawerLayout.closeDrawers();
  }

  private void setupZoomView() {
    final TextView textView = (TextView) findViewById(R.id.textZoom);
    mapboxMap.setOnCameraChangeListener(position ->
      textView.setText(String.format(getString(R.string.debug_zoom), position.zoom))
    );
  }

  private void setupDebugChangeView() {
    FloatingActionButton fabDebug = (FloatingActionButton) findViewById(R.id.fabDebug);
    fabDebug.setOnClickListener(view -> {
      if (mapboxMap != null) {
        Timber.d("Debug FAB: isDebug Active? %s", mapboxMap.isDebugActive());
        mapboxMap.cycleDebugOptions();
      }
    });
  }

  private void setupStyleChangeView() {
    FloatingActionButton fabStyles = (FloatingActionButton) findViewById(R.id.fabStyles);
    fabStyles.setOnClickListener(view -> {
      if (mapboxMap != null) {
        currentStyleIndex++;
        if (currentStyleIndex == STYLES.length) {
          currentStyleIndex = 0;
        }
        mapboxMap.setStyleUrl(STYLES[currentStyleIndex], style -> Timber.d("Style loaded %s", style));
      }
    });
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    return actionBarDrawerToggle.onOptionsItemSelected(item) || super.onOptionsItemSelected(item);
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
          (TextView) view.findViewById(android.R.id.text1),
          (TextView) view.findViewById(android.R.id.text2)
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
