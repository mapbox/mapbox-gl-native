package com.mapbox.mapboxsdk.attribution;

import android.graphics.Bitmap;
import android.graphics.PointF;
import android.widget.TextView;

public class AttributionProvider {

  private Bitmap logo;
  private Bitmap logoSmall;
  private Bitmap snapshot;
  private TextView textView;
  private float margin;

  public AttributionProvider(Bitmap snapshot, Bitmap logo, Bitmap logoSmall, TextView textView, float marginPadding) {
    this.snapshot = snapshot;
    this.logo = logo;
    this.logoSmall = logoSmall;
    this.textView = textView;
    this.margin = marginPadding;
  }

  public AttributionPlacement calculateAttributionPlacement() {
    float logoContainerWidth = logo.getWidth() + (2 * margin);

    PointF anchor = new PointF(snapshot.getWidth() - textView.getMeasuredWidth() - margin, snapshot.getHeight() - margin - textView.getMeasuredHeight());
    float textViewContainerWidth =  textView.getMeasuredWidth() + margin;
    if (logoContainerWidth + textViewContainerWidth <= snapshot.getWidth()) {
      // it fits! align to right
      return new AttributionPlacement(logo, anchor);
    } else {
      // it doesn't fit let's try again with a smaller logo
      float smallLogoContainerWidth = logoSmall.getWidth() + (2 * margin);
      if (smallLogoContainerWidth + textViewContainerWidth <= snapshot.getWidth()) {
        // it fits! align to right + use small logo
        return new AttributionPlacement(logoSmall, anchor);
      } else {
        // it doesn't fit let try without a logo
        if (textViewContainerWidth + margin <= snapshot.getWidth()) {
          // it fits
          return new AttributionPlacement(null, anchor);
        } else {
          // it doesn't fit
          return new AttributionPlacement(null, null);
        }
      }
    }
  }
}
