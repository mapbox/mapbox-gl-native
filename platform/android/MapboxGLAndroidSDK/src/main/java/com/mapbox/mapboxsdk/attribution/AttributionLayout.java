package com.mapbox.mapboxsdk.attribution;

import android.graphics.Bitmap;
import android.graphics.PointF;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public class AttributionLayout {

  @Nullable
  private Bitmap logo;
  @Nullable
  private PointF anchorPoint;
  private boolean shortText;

  public AttributionLayout(@Nullable Bitmap logo, @Nullable PointF anchorPoint, boolean shortText) {
    this.logo = logo;
    this.anchorPoint = anchorPoint;
    this.shortText = shortText;
  }

  @Nullable
  public Bitmap getLogo() {
    return logo;
  }

  @Nullable
  public PointF getAnchorPoint() {
    return anchorPoint;
  }

  public boolean isShortText() {
    return shortText;
  }

  @Override
  public boolean equals(@Nullable Object o) {
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

  @NonNull
  @Override
  public String toString() {
    return "AttributionLayout{"
      + "logo=" + logo
      + ", anchorPoint=" + anchorPoint
      + '}';
  }
}
