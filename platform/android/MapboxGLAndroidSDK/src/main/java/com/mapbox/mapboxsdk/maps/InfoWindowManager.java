package com.mapbox.mapboxsdk.maps;

import android.content.Context;
import android.graphics.Bitmap;
import android.support.annotation.Nullable;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.annotations.InfoWindow;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.style.layers.Filter;
import com.mapbox.mapboxsdk.style.layers.PropertyFactory;
import com.mapbox.mapboxsdk.style.layers.SymbolLayer;
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource;
import com.mapbox.mapboxsdk.utils.BitmapUtils;
import com.mapbox.services.commons.geojson.Feature;
import com.mapbox.services.commons.geojson.FeatureCollection;
import com.mapbox.services.commons.geojson.Point;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * Responsible for managing InfoWindows shown on the Map.
 * <p>
 * Maintains a {@link List} of opened {@link InfoWindow} and tracks configurations as
 * allowConcurrentMultipleInfoWindows which allows to have multiple {@link InfoWindow} open at the
 * same time. Responsible for managing listeners as
 * {@link com.mapbox.mapboxsdk.maps.MapboxMap.OnInfoWindowClickListener} and
 * {@link com.mapbox.mapboxsdk.maps.MapboxMap.OnInfoWindowLongClickListener}.
 * </p>
 */
class InfoWindowManager {
  private static final String SOURCE_ID = "info.window.source";
  private static final String LAYER_ID = "info.window.layer";
  private static final String PROPERTY_SELECTED = "info.window.property.selected";
  private static final String PROPERTY_ID = "info.window.property.ID";

  private GeoJsonSource infoWindowsSource;
  private SymbolLayer infoWindowsLayer;

  private MapboxMap mapboxMap;

  private final List<InfoWindow> infoWindows = new ArrayList<>();
  private final HashMap<Long, Marker> markerHashMap = new HashMap<>();
  private final FeatureCollection featureCollection = FeatureCollection.fromFeatures(new ArrayList<Feature>());

  private MapboxMap.InfoWindowAdapter infoWindowAdapter;
  private boolean allowConcurrentMultipleInfoWindows;

  private MapboxMap.OnInfoWindowClickListener onInfoWindowClickListener;
  private MapboxMap.OnInfoWindowLongClickListener onInfoWindowLongClickListener;
  private MapboxMap.OnInfoWindowCloseListener onInfoWindowCloseListener;

  void bind(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
    initialise();
  }

  private void initialise() {
    infoWindowsSource = new GeoJsonSource(SOURCE_ID);
    mapboxMap.addSource(infoWindowsSource);

    infoWindowsLayer = new SymbolLayer(LAYER_ID, SOURCE_ID)
      .withProperties(PropertyFactory.iconImage("{" + PROPERTY_ID + "}"))
      .withFilter(Filter.eq(PROPERTY_SELECTED, true));
    mapboxMap.addLayer(infoWindowsLayer);
  }

  void update() {
    if (!infoWindows.isEmpty()) {
      for (InfoWindow infoWindow : infoWindows) {
        infoWindow.update();
      }
    }
  }

  void setInfoWindowAdapter(@Nullable MapboxMap.InfoWindowAdapter infoWindowAdapter) {
    this.infoWindowAdapter = infoWindowAdapter;
  }

  MapboxMap.InfoWindowAdapter getInfoWindowAdapter() {
    return infoWindowAdapter;
  }

  void setAllowConcurrentMultipleOpenInfoWindows(boolean allow) {
    allowConcurrentMultipleInfoWindows = allow;
  }

  boolean isAllowConcurrentMultipleOpenInfoWindows() {
    return allowConcurrentMultipleInfoWindows;
  }

  boolean isInfoWindowValidForMarker(Marker marker) {
    return marker != null && (!TextUtils.isEmpty(marker.getTitle()) || !TextUtils.isEmpty(marker.getSnippet()));
  }

  void setOnInfoWindowClickListener(@Nullable MapboxMap.OnInfoWindowClickListener listener) {
    onInfoWindowClickListener = listener;
  }

  MapboxMap.OnInfoWindowClickListener getOnInfoWindowClickListener() {
    return onInfoWindowClickListener;
  }

  void setOnInfoWindowLongClickListener(@Nullable MapboxMap.OnInfoWindowLongClickListener listener) {
    onInfoWindowLongClickListener = listener;
  }

  MapboxMap.OnInfoWindowLongClickListener getOnInfoWindowLongClickListener() {
    return onInfoWindowLongClickListener;
  }

  void setOnInfoWindowCloseListener(@Nullable MapboxMap.OnInfoWindowCloseListener listener) {
    onInfoWindowCloseListener = listener;
  }

  MapboxMap.OnInfoWindowCloseListener getOnInfoWindowCloseListener() {
    return onInfoWindowCloseListener;
  }

  void addMarker(MapView mapView, Marker marker) {
    invalidateWindow(mapView, marker);
    addNewFeature(marker);
    refreshSource();
  }

  void invalidateWindows(MapView mapView) {
    for (Marker marker : markerHashMap.values()) {
      invalidateWindow(mapView, marker);
    }
  }

  private void invalidateWindow(MapView mapView, Marker marker) {
    LayoutInflater layoutInflater = LayoutInflater.from(mapView.getContext());
    View view;
    if (infoWindowAdapter == null) {
      view = layoutInflater.inflate(R.layout.mapbox_infowindow_content, mapView, false);
    } else {
      view = infoWindowAdapter.getInfoWindow(marker);
    }
    markerHashMap.put(marker.getId(), marker);

    if (view != null) {
      Bitmap bitmap = BitmapUtils.generate(view);
      mapboxMap.addImage(String.valueOf(marker.getId()), bitmap);
    }
  }

  void addMarkers(MapView mapView, List<Marker> markers) {
    for (Marker marker : markers) {
      invalidateWindow(mapView, marker);
      addNewFeature(marker);
    }
    refreshSource();
  }

  void removeMarker(Context context, Marker marker) {

  }

  public void add(InfoWindow infoWindow) {
    infoWindows.add(infoWindow);
  }

  private void addNewFeature(Marker marker) {
    Feature feature = Feature.fromGeometry(
      Point.fromCoordinates(
        new double[] {marker.getPosition().getLongitude(), marker.getPosition().getLatitude()}
      )
    );
    feature.addNumberProperty(PROPERTY_ID, marker.getId());
    feature.addBooleanProperty(PROPERTY_SELECTED, true);
    featureCollection.getFeatures().add(feature);
  }

  private void refreshSource() {
    infoWindowsSource.setGeoJson(featureCollection);
  }
}
