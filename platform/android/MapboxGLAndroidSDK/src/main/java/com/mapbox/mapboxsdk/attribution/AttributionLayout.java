package com.mapbox.mapboxsdk.attribution;

import android.graphics.Bitmap;
import android.graphics.PointF;
import android.support.annotation.Nullable;

public class AttributionLayout {

  private Bitmap logo;
  private PointF anchorPoint;
  private boolean shortText;

  public AttributionLayout(@Nullable Bitmap logo, @Nullable PointF anchorPoint, boolean shortText) {
    this.logo = logo;
    this.anchorPoint = anchorPoint;
    this.shortText = shortText;
  }

  public Bitmap getLogo() {
    return logo;
  }

  public PointF getAnchorPoint() {
    return anchorPoint;
  }

  public boolean isShortText() {
    return shortText;
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    AttributionLayout that = (AttributionLayout) o;

    if (logo != null ? !logo.equals(that.logo) : that.logo != null) {
      return false;
    }
    return anchorPoint != null ? anchorPoint.equals(that.anchorPoint) : that.anchorPoint == null;
  }

  @Override
  public int hashCode() {
    int result = logo != null ? logo.hashCode() : 0;
    result = 31 * result + (anchorPoint != null ? anchorPoint.hashCode() : 0);
    return result;
  }

  @Override
  public String toString() {
    return "AttributionLayout{"
      + "logo=" + logo
      + ", anchorPoint=" + anchorPoint
      + '}';
  }
}
