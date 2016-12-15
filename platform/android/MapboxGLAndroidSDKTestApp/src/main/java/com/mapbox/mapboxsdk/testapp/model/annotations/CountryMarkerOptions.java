package com.mapbox.mapboxsdk.testapp.model.annotations;

import android.graphics.Bitmap;
import android.os.Parcel;
import android.os.Parcelable;

import com.mapbox.mapboxsdk.annotations.BaseMarkerOptions;
import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;

public class CountryMarkerOptions extends BaseMarkerOptions<CountryMarker, CountryMarkerOptions> {

  private String abbrevName;
  private int flagRes;

  public CountryMarkerOptions abbrevName(String name) {
    abbrevName = name;
    return getThis();
  }

  public CountryMarkerOptions flagRes(int imageRes) {
    flagRes = imageRes;
    return getThis();
  }

  public CountryMarkerOptions() {
  }

  private CountryMarkerOptions(Parcel in) {
    position((LatLng) in.readParcelable(LatLng.class.getClassLoader()));
    snippet(in.readString());
    String iconId = in.readString();
    Bitmap iconBitmap = in.readParcelable(Bitmap.class.getClassLoader());
    Icon icon = IconFactory.recreate(iconId, iconBitmap);
    icon(icon);
    title(in.readString());
  }

  @Override
  public CountryMarkerOptions getThis() {
    return this;
  }

  @Override
  public CountryMarker getMarker() {
    return new CountryMarker(this, abbrevName, flagRes);
  }

  public static final Parcelable.Creator<CountryMarkerOptions> CREATOR
    = new Parcelable.Creator<CountryMarkerOptions>() {
      public CountryMarkerOptions createFromParcel(Parcel in) {
        return new CountryMarkerOptions(in);
      }

      public CountryMarkerOptions[] newArray(int size) {
        return new CountryMarkerOptions[size];
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
