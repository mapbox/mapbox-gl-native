package com.mapbox.mapboxsdk.annotations;

import android.support.annotation.IntDef;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

class ArrowDirection {
  @IntDef( {LEFT, RIGHT, TOP, BOTTOM})
  @Retention(RetentionPolicy.SOURCE)
  @interface Value {
  }

  static final int LEFT = 0;
  static final int RIGHT = 1;
  static final int TOP = 2;
  static final int BOTTOM = 3;

  @Value
  private final int value;

  ArrowDirection(@Value int value) {
    this.value = value;
  }

  @Value
  public int getValue() {
    return value;
  }
}
