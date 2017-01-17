package com.mapbox.mapboxsdk.style.layers;

import android.support.annotation.NonNull;

class LayoutPropertyValue<T> extends PropertyValue<T> {

  LayoutPropertyValue(@NonNull String name, T value) {
    super(name, value);
  }

}
