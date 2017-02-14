package com.mapbox.mapboxsdk.maps;

import com.mapbox.mapboxsdk.style.layers.Layer;
import com.mapbox.mapboxsdk.style.sources.Source;

public interface Callback<T> {

  void onResult(T t);

  interface SourceCallback<U extends Source> {
    void onResult(U u);
  }

  interface LayerCallback<V extends Layer> {
    void onResult(V v);
  }

}
