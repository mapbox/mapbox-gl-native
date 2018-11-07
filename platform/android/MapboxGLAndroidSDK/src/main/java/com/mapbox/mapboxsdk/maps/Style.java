package com.mapbox.mapboxsdk.maps;

import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.sources.Source;

import java.util.HashMap;

public class Style {

  private NativeMapView nativeMapView;
  private final HashMap<String, Source> sources = new HashMap<>();
  private final HashMap<String, Layer> layers = new HashMap<>();

  public Style(NativeMapView nativeMapView) {
    this.nativeMapView = nativeMapView;
  }

  public void loadStyle(@com.mapbox.mapboxsdk.constants.Style.StyleUrl String styleUrl) {
    for (Source source : sources.values()) {
      if (source != null) {
        source.setDetached();
        nativeMapView.removeSource(source);
      }
    }

    for (Layer layer : layers.values()) {
      if (layer != null) {
        layer.setDetached();
        nativeMapView.removeLayer(layer);
      }
    }

    nativeMapView.setStyleUrl(styleUrl);
  }

  //
  // Source
  //

  public void addSource(Source source) {
    sources.put(source.getId(), source);
    nativeMapView.addSource(source);
  }

  public Source getSource(String id) {
    Source source = sources.get(id);
    if (source == null) {
      source = nativeMapView.getSource(id);
    }
    return source;
  }

  //
  // Layer
  //

  public void addLayer(Layer layer) {
    layers.put(layer.getId(), layer);
    nativeMapView.addLayer(layer);
  }

  public Layer getLayer(String id) {
    Layer layer = layers.get(id);
    if (layer == null) {
      layer = nativeMapView.getLayer(id);
    }
    return layer;
  }


}
