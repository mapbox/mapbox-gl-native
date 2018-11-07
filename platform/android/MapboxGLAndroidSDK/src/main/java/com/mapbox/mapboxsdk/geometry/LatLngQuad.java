package com.mapbox.mapboxsdk.geometry;

import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.Keep;
import android.support.annotation.NonNull;

/**
 * A geographical area representing a non-aligned quadrilateral
 * <p>
 * This class does not wrap values to the world bounds
 * </p>
 */
public class LatLngQuad implements Parcelable {

  @Keep
  private final LatLng topLeft;
  @Keep
  private final LatLng topRight;
  @Keep
  private final LatLng bottomRight;
  @Keep
  private final LatLng bottomLeft;

  /**
   * Construct a new LatLngQuad based on its corners,
   * in order top left, top right, bottom left, bottom right
   */
  @Keep
  public LatLngQuad(final LatLng topLeft, final LatLng topRight, final LatLng bottomRight, final LatLng bottomLeft) {
    this.topLeft = topLeft;
    this.topRight = topRight;
    this.bottomRight = bottomRight;
    this.bottomLeft = bottomLeft;
  }

  public LatLng getTopLeft() {
    return this.topLeft;
  }

  public LatLng getTopRight() {
    return this.topRight;
  }

  public LatLng getBottomRight() {
    return this.bottomRight;
  }

  public LatLng getBottomLeft() {
    return this.bottomLeft;
  }

  public static final Parcelable.Creator<LatLngQuad> CREATOR = new Parcelable.Creator<LatLngQuad>() {
    @Override
    public LatLngQuad createFromParcel(@NonNull final Parcel in) {
      return readFromParcel(in);
    }

    @Override
    public LatLngQuad[] newArray(final int size) {
      return new LatLngQuad[size];
    }
  };

  @Override
  public int hashCode() {
    int code = topLeft.hashCode();
    code = (code ^ code >>> 31) + topRight.hashCode();
    code = (code ^ code >>> 31) + bottomRight.hashCode();
    code = (code ^ code >>> 31) + bottomLeft.hashCode();
    return code;
  }

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(@NonNull final Parcel out, final int arg1) {
    topLeft.writeToParcel(out, arg1);
    topRight.writeToParcel(out, arg1);
    bottomRight.writeToParcel(out, arg1);
    bottomLeft.writeToParcel(out, arg1);
  }

  private static LatLngQuad readFromParcel(@NonNull final Parcel in) {
    final LatLng topLeft = new LatLng(in);
    final LatLng topRight = new LatLng(in);
    final LatLng bottomRight = new LatLng(in);
    final LatLng bottomLeft = new LatLng(in);
    return new LatLngQuad(topLeft, topRight, bottomRight, bottomLeft);
  }
}
