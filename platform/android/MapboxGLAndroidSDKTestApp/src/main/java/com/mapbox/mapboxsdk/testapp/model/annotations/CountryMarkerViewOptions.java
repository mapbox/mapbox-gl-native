package com.mapbox.mapboxsdk.testapp.model.annotations;

import android.graphics.Bitmap;
import android.os.Parcel;
import android.os.Parcelable;

import com.mapbox.mapboxsdk.annotations.BaseMarkerViewOptions;
import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.IconFactory;
import com.mapbox.mapboxsdk.geometry.LatLng;

public class CountryMarkerViewOptions extends BaseMarkerViewOptions<CountryMarkerView, CountryMarkerViewOptions> {

  private String abbrevName;
  private int flagRes;

  public CountryMarkerViewOptions() {
  }

  protected CountryMarkerViewOptions(Parcel in) {
    position((LatLng) in.readParcelable(LatLng.class.getClassLoader()));
    snippet(in.readString());
    title(in.readString());
    flat(in.readByte() != 0);
    anchor(in.readFloat(), in.readFloat());
    infoWindowAnchor(in.readFloat(), in.readFloat());
    rotation(in.readFloat());
    visible(in.readByte() != 0);
    alpha(in.readFloat());
    if (in.readByte() != 0) {
      // this means we have an icon
      String iconId = in.readString();
      Bitmap iconBitmap = in.readParcelable(Bitmap.class.getClassLoader());
      Icon icon = IconFactory.recreate(iconId, iconBitmap);
      icon(icon);
    }
    abbrevName(in.readString());
    flagRes(in.readInt());
  }

  @Override
  public CountryMarkerViewOptions getThis() {
    return this;
  }

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel out, int flags) {
    out.writeParcelable(getPosition(), flags);
    out.writeString(getSnippet());
    out.writeString(getTitle());
    out.writeByte((byte) (isFlat() ? 1 : 0));
    out.writeFloat(getAnchorU());
    out.writeFloat(getAnchorV());
    out.writeFloat(getInfoWindowAnchorU());
    out.writeFloat(getInfoWindowAnchorV());
    out.writeFloat(getRotation());
    out.writeByte((byte) (isVisible() ? 1 : 0));
    out.writeFloat(getAlpha());
    Icon icon = getIcon();
    out.writeByte((byte) (icon != null ? 1 : 0));
    if (icon != null) {
      out.writeString(getIcon().getId());
      out.writeParcelable(getIcon().getBitmap(), flags);
    }
    out.writeString(abbrevName);
    out.writeInt(flagRes);
  }

  @Override
  public CountryMarkerView getMarker() {
    return new CountryMarkerView(this, abbrevName, flagRes);
  }

  public CountryMarkerViewOptions abbrevName(String abbrevName) {
    this.abbrevName = abbrevName;
    return getThis();
  }

  public CountryMarkerViewOptions flagRes(int flagRes) {
    this.flagRes = flagRes;
    return getThis();
  }

  public static final Parcelable.Creator<CountryMarkerViewOptions> CREATOR
    = new Parcelable.Creator<CountryMarkerViewOptions>() {
      public CountryMarkerViewOptions createFromParcel(Parcel in) {
        return new CountryMarkerViewOptions(in);
      }

      public CountryMarkerViewOptions[] newArray(int size) {
        return new CountryMarkerViewOptions[size];
      }
    };
}
