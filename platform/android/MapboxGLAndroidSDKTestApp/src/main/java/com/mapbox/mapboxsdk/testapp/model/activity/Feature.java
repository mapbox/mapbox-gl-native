package com.mapbox.mapboxsdk.testapp.model.activity;

import android.os.Parcel;
import android.os.Parcelable;

public class Feature implements Parcelable {

  private String name;
  private String label;
  private String description;
  private String category;

  public Feature(String name, String label, String description, String category) {
    this.name = name;
    this.label = label;
    this.description = description;
    this.category = category;
  }

  private Feature(Parcel in) {
    name = in.readString();
    label = in.readString();
    description = in.readString();
    category = in.readString();
  }

  public String getName() {
    return name;
  }

  public String getSimpleName() {
    String[] split = name.split("\\.");
    return split[split.length - 1];
  }

  public String getLabel() {
    return label != null ? label : getSimpleName();
  }

  public String getDescription() {
    return description != null ? description : "-";
  }

  public String getCategory() {
    return category;
  }

  public int describeContents() {
    return 0;
  }

  public void writeToParcel(Parcel out, int flags) {
    out.writeString(name);
    out.writeString(label);
    out.writeString(description);
    out.writeString(category);
  }

  public static final Parcelable.Creator<Feature> CREATOR
    = new Parcelable.Creator<Feature>() {
      public Feature createFromParcel(Parcel in) {
        return new Feature(in);
      }

      public Feature[] newArray(int size) {
        return new Feature[size];
      }
    };
}
