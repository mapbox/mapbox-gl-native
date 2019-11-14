package com.mapbox.mapboxsdk.location;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.mapbox.mapboxsdk.maps.Style;
import com.mapbox.mapboxsdk.style.layers.Layer;

class LocationComponentPositionManager {

  @NonNull
  private final Style style;

  @Nullable
  private String layerAbove;

  @Nullable
  private String layerBelow;

  LocationComponentPositionManager(@NonNull Style style, @Nullable String layerAbove, @Nullable String layerBelow) {
    this.style = style;
    this.layerAbove = layerAbove;
    this.layerBelow = layerBelow;
  }

  /**
   * Returns true whenever layer above/below configuration has changed and requires re-layout.
   */
  boolean update(@Nullable String layerAbove, @Nullable String layerBelow) {
    boolean requiresUpdate =
      !(this.layerAbove == layerAbove || (this.layerAbove != null && this.layerAbove.equals(layerAbove)))
        || !(this.layerBelow == layerBelow || (this.layerBelow != null && this.layerBelow.equals(layerBelow)));

    this.layerAbove = layerAbove;
    this.layerBelow = layerBelow;
    return requiresUpdate;
  }

  void addLayerToMap(@NonNull Layer layer) {
    if (layerAbove != null) {
      style.addLayerAbove(layer, layerAbove);
    } else if (layerBelow != null) {
      style.addLayerBelow(layer, layerBelow);
    } else {
      style.addLayer(layer);
    }
  }
}
