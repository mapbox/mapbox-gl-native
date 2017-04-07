package com.mapbox.mapboxsdk.annotations;


import android.os.Parcel;
import android.os.Parcelable;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.ArrayList;
import java.util.List;

/**
 * Encapsulates a {@link List} of {@link LatLng} points defining a hole
 */
public class Hole extends ArrayList<LatLng> implements Parcelable {

  public Hole() {
    super();
  }

  /**
   * Creates a Hole.
   *
   * @param holePoints {@link List} list of {@link LatLng} points defining a hole
   */
  public Hole(List<LatLng> holePoints) {
    super(holePoints);
  }

  protected Hole(Parcel in) {
    in.readTypedList(this, LatLng.CREATOR);
  }

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel parcel, int i) {
    parcel.writeTypedList(this);
  }

  public static final Parcelable.Creator<Hole> CREATOR = new Parcelable.Creator<Hole>() {
    public Hole createFromParcel(Parcel in) {
      return new Hole(in);
    }

    public Hole[] newArray(int size) {
      return new Hole[size];
    }
  };
}
