package com.mapbox.mapboxsdk.style.layers;

import androidx.annotation.NonNull;

class PaintPropertyValue<T> extends PropertyValue<T> {

  PaintPropertyValue(@NonNull String name, T value) {
    super(name, value);
  }

}
