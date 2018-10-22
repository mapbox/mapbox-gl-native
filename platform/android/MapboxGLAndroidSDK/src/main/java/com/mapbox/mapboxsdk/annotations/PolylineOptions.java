package com.mapbox.mapboxsdk.annotations;

import android.os.Parcel;
import android.os.Parcelable;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.ArrayList;
import java.util.List;

/**
 * Builder for composing {@link Polyline} objects.
 */
public final class PolylineOptions implements Parcelable {


  public static final Parcelable.Creator<PolylineOptions> CREATOR =
    new Parcelable.Creator<PolylineOptions>() {
      public PolylineOptions createFromParcel(Parcel in) {
        return new PolylineOptions(in);
      }

      public PolylineOptions[] newArray(int size) {
        return new PolylineOptions[size];
      }
    };

  private PolylineOptions(Parcel in) {
    polyline = new Polyline();
    ArrayList<LatLng> pointsList = new ArrayList<>();
    in.readList(pointsList, LatLng.class.getClassLoader());
    addAll(pointsList);
    alpha(in.readFloat());
    color(in.readInt());
    width(in.readFloat());
  }

  /**
   * Describe the kinds of special objects contained in this Parcelable's
   * marshalled representation.
   *
   * @return integer 0.
   */
  @Override
  public int describeContents() {
    return 0;
  }

  /**
   * Flatten this object in to a Parcel.
   *
   * @param out   The Parcel in which the object should be written.
   * @param flags Additional flags about how the object should be written. May be 0 or
   *              {@link #PARCELABLE_WRITE_RETURN_VALUE}.
   */
  @Override
  public void writeToParcel(Parcel out, int flags) {
    out.writeList(getPoints());
    out.writeFloat(getAlpha());
    out.writeInt(getColor());
    out.writeFloat(getWidth());
  }

  private Polyline polyline;

  /**
   * Defines options for a polyline.
   */
  public PolylineOptions() {
    polyline = new Polyline();
  }

  /**
   * Adds a vertex to the end of the polyline being built.
   *
   * @param point {@link LatLng} point to be added to polyline geometry.
   * @return This {@link PolylineOptions} object with the given point on the end.
   */
  public PolylineOptions add(LatLng point) {
    polyline.addPoint(point);
    return this;
  }

  /**
   * Adds vertices to the end of the polyline being built.
   *
   * @param points {@link LatLng} points defining the polyline geometry.
   * @return This {@link PolylineOptions} object with the given point on the end.
   */
  public PolylineOptions add(LatLng... points) {
    for (LatLng point : points) {
      add(point);
    }
    return this;
  }

  /**
   * Adds vertices to the end of the polyline being built.
   *
   * @param points {@link Iterable} list made up of {@link LatLng} points defining the polyline
   *               geometry
   * @return This {@link PolylineOptions} object with the given points on the end.
   */
  public PolylineOptions addAll(Iterable<LatLng> points) {
    for (LatLng point : points) {
      add(point);
    }
    return this;
  }

  /**
   * Set the alpha value of the polyline.
   *
   * @param alpha float value between 0 (not visible) and 1.
   * @return This {@link PolylineOptions} object with the given polyline alpha value.
   */
  public PolylineOptions alpha(float alpha) {
    polyline.setAlpha(alpha);
    return this;
  }

  /**
   * Gets the alpha set for this {@link PolylineOptions} object.
   *
   * @return float value between 0 and 1 defining the alpha.
   */
  public float getAlpha() {
    return polyline.getAlpha();
  }

  /**
   * Sets the color of the polyline as a 32-bit ARGB color. The default color is black.
   *
   * @param color 32-bit ARGB color.
   * @return This {@link PolylineOptions} object with a new color set.
   */
  public PolylineOptions color(int color) {
    polyline.setColor(color);
    return this;
  }

  /**
   * Gets the color set for this {@link PolylineOptions} object.
   *
   * @return The color of the polyline in ARGB format.
   */
  public int getColor() {
    return polyline.getColor();
  }

  /**
   * Do not use this method. Used internally by the SDK.
   *
   * @return PolyLine The polyline build by this class.
   */
  public Polyline getPolyline() {
    return polyline;
  }

  /**
   * Gets the width set for this {@link PolylineOptions} object.
   *
   * @return The width of the polyline in screen pixels.
   */
  public float getWidth() {
    return polyline.getWidth();
  }

  /**
   * Sets the width of the polyline in screen pixels. The default is 10.
   *
   * @param width float value defining width of polyline using unit pixels.
   * @return This {@link PolylineOptions} object with a new width set.
   */
  public PolylineOptions width(float width) {
    polyline.setWidth(width);
    return this;
  }

  /**
   * Gets the points set for this {@link PolylineOptions} object.
   *
   * @return a {@link List} of {@link LatLng}s specifying the vertices of the polyline.
   */
  public List<LatLng> getPoints() {
    // the getter gives us a copy, which is the safe thing to do...
    return polyline.getPoints();
  }

  /**
   * Compares this {@link PolylineOptions} object with another {@link PolylineOptions} and
   * determines if their color, alpha, width, and vertices match.
   *
   * @param o Another {@link PolylineOptions} to compare with this object.
   * @return True if color, alpha, width, and vertices match this {@link PolylineOptions} object.
   * Else, false.
   */
  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    PolylineOptions polyline = (PolylineOptions) o;

    if (Float.compare(polyline.getAlpha(), getAlpha()) != 0) {
      return false;
    }
    if (getColor() != polyline.getColor()) {
      return false;
    }
    if (Float.compare(polyline.getWidth(), getWidth()) != 0) {
      return false;
    }
    return !(getPoints() != null ? !getPoints().equals(polyline.getPoints()) : polyline.getPoints() != null);
  }

  /**
   * Gives an integer which can be used as the bucket number for storing elements of the set/map.
   * This bucket number is the address of the element inside the set/map. There's no guarantee
   * that this hash value will be consistent between different Java implementations, or even
   * between different execution runs of the same program.
   *
   * @return integer value you can use for storing element.
   */
  @Override
  public int hashCode() {
    int result = 1;
    result = 31 * result + (getAlpha() != +0.0f ? Float.floatToIntBits(getAlpha()) : 0);
    result = 31 * result + getColor();
    result = 31 * result + (getWidth() != +0.0f ? Float.floatToIntBits(getWidth()) : 0);
    result = 31 * result + (getPoints() != null ? getPoints().hashCode() : 0);
    return result;
  }
}
