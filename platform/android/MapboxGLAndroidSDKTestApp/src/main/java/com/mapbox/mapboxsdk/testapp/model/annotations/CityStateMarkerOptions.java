package com.mapbox.mapboxsdk.testapp.model.annotations;

import android.graphics.Bitmap;
import android.os.Parcel;
import android.os.Parcelable;

import com.mapbox.mapboxsdk.annotations.BaseMarkerOptions;
import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;

public class CityStateMarkerOptions extends BaseMarkerOptions<CityStateMarker, CityStateMarkerOptions> {

  private String infoWindowBackgroundColor;

  public CityStateMarkerOptions infoWindowBackground(String color) {
    infoWindowBackgroundColor = color;
    return getThis();
  }

  public CityStateMarkerOptions() {
  }

  private CityStateMarkerOptions(Parcel in) {
    position((LatLng) in.readParcelable(LatLng.class.getClassLoader()));
    snippet(in.readString());
    String iconId = in.readString();
    Bitmap iconBitmap = in.readParcelable(Bitmap.class.getClassLoader());
    Icon icon = IconFactory.recreate(iconId, iconBitmap);
    icon(icon);
    title(in.readString());
  }

  @Override
  public CityStateMarkerOptions getThis() {
    return this;
  }

  @Override
  public CityStateMarker getMarker() {
    return new CityStateMarker(this, infoWindowBackgroundColor);
  }

  public static final Parcelable.Creator<CityStateMarkerOptions> CREATOR
    = new Parcelable.Creator<CityStateMarkerOptions>() {
      public CityStateMarkerOptions createFromParcel(Parcel in) {
        return new CityStateMarkerOptions(in);
      }

      public CityStateMarkerOptions[] newArray(int size) {
        return new CityStateMarkerOptions[size];
      }
    };

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel out, int flags) {
    out.writeParcelable(position, flags);
    out.writeString(snippet);
    out.writeString(icon.getId());
    out.writeParcelable(icon.getBitmap(), flags);
    out.writeString(title);
  }
}
