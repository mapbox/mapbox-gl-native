package com.mapbox.mapboxsdk.attribution;

import android.graphics.Bitmap;
import android.graphics.PointF;
import android.support.annotation.Nullable;

public class AttributionPlacement {

  private Bitmap logo;
  private PointF anchorPoint;

  public AttributionPlacement(@Nullable Bitmap logo, @Nullable PointF anchorPoint) {
    this.logo = logo;
    this.anchorPoint = anchorPoint;
  }

  public Bitmap getLogo() {
    return logo;
  }

  public PointF getAnchorPoint() {
    return anchorPoint;
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    AttributionPlacement that = (AttributionPlacement) o;

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
    return "AttributionPlacement{" +
      "logo=" + logo +
      ", anchorPoint=" + anchorPoint +
      '}';
  }
}
