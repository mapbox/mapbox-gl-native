package com.mapbox.mapboxsdk.annotations;


import android.os.Parcel;
import android.os.Parcelable;

import com.mapbox.mapboxsdk.geometry.LatLng;

import java.util.ArrayList;
import java.util.List;

/**
 * Builder for composing {@link Polygon} objects.
 */
public final class PolygonOptions implements Parcelable {

  public static final Parcelable.Creator<PolygonOptions> CREATOR =
    new Parcelable.Creator<PolygonOptions>() {
      public PolygonOptions createFromParcel(Parcel in) {
        return new PolygonOptions(in);
      }

      public PolygonOptions[] newArray(int size) {
        return new PolygonOptions[size];
      }
    };

  private PolygonOptions(Parcel in) {
    polygon = new Polygon();
    List<LatLng> pointsList = new ArrayList<>();
    in.readList(pointsList, LatLng.class.getClassLoader());
    addAll(pointsList);
    List<List<LatLng>> holes = new ArrayList<>();
    in.readList(holes, LatLng.class.getClassLoader());
    addAllHoles(holes);
    alpha(in.readFloat());
    fillColor(in.readInt());
    strokeColor(in.readInt());
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
    out.writeList(getHoles());
    out.writeFloat(getAlpha());
    out.writeInt(getFillColor());
    out.writeInt(getStrokeColor());
  }

  private Polygon polygon;

  /**
   * Defines options for a polygon.
   */
  public PolygonOptions() {
    polygon = new Polygon();
  }

  /**
   * Adds a vertex to the outline of the polygon being built.
   *
   * @param point {@link LatLng} point to be added to polygon geometry.
   * @return This {@link PolygonOptions} object with the given point added to the outline.
   */
  public PolygonOptions add(LatLng point) {
    polygon.addPoint(point);
    return this;
  }

  /**
   * Adds vertices to the outline of the polygon being built.
   *
   * @param points {@link LatLng} points to be added to polygon geometry.
   * @return This {@link PolygonOptions} object with the given points added to the outline.
   */
  public PolygonOptions add(LatLng... points) {
    for (LatLng point : points) {
      add(point);
    }
    return this;
  }

  /**
   * Adds vertices to the outline of the polygon being built.
   *
   * @param points {@link Iterable} list made up of {@link LatLng} points defining the polygon
   *               geometry
   * @return This {@link PolygonOptions} object with the given points added to the outline.
   */
  public PolygonOptions addAll(Iterable<LatLng> points) {
    for (LatLng point : points) {
      add(point);
    }
    return this;
  }

  /**
   * Adds a hole to the outline of the polygon being built.
   *
   * @param hole {@link List} list made up of {@link LatLng} points defining the hole
   * @return This {@link PolygonOptions} object with the given hole added to the outline.
   */
  public PolygonOptions addHole(List<LatLng> hole) {
    polygon.addHole(hole);
    return this;
  }

  /**
   * Adds holes to the outline of the polygon being built.
   *
   * @param holes {@link List} list made up of {@link LatLng} holes to be added to polygon geometry
   * @return This {@link PolygonOptions} object with the given holes added to the outline.
   */
  public PolygonOptions addHole(List<LatLng>... holes) {
    for (List<LatLng> hole : holes) {
      addHole(hole);
    }
    return this;
  }

  /**
   * Adds holes to the outline of the polygon being built.
   *
   * @param holes {@link Iterable} list made up of {@link List} list of {@link LatLng} holes defining the hole geometry
   * @return This {@link PolygonOptions} object with the given holes added to the outline.
   */
  public PolygonOptions addAllHoles(Iterable<List<LatLng>> holes) {
    for (List<LatLng> hole : holes) {
      addHole(hole);
    }
    return this;
  }

  /**
   * Set the alpha value of the polyline.
   *
   * @param alpha float value between 0 (not visible) and 1.
   * @return This {@link PolygonOptions} object with the given polygon alpha value.
   */
  public PolygonOptions alpha(float alpha) {
    polygon.setAlpha(alpha);
    return this;
  }

  /**
   * Gets the alpha set for this {@link PolygonOptions} object.
   *
   * @return float value between 0 and 1 defining the alpha.
   */
  public float getAlpha() {
    return polygon.getAlpha();
  }

  /**
   * Specifies the polygon's fill color, as 32-bit ARGB. The default color is black.
   *
   * @param color 32-bit ARGB color.
   * @return This {@link PolylineOptions} object with a new color set.
   */
  public PolygonOptions fillColor(int color) {
    polygon.setFillColor(color);
    return this;
  }

  /**
   * Gets the fill color set for this {@link PolygonOptions} object.
   *
   * @return The fill color of the polygon in ARGB format.
   */
  public int getFillColor() {
    return polygon.getFillColor();
  }

  /**
   * Do not use this method. Used internally by the SDK.
   *
   * @return Polygon the Polygon to return
   */
  public Polygon getPolygon() {
    return polygon;
  }

  /**
   * Specifies the polygon's stroke color, as 32-bit ARGB. The default color is black.
   *
   * @param color 32-bit ARGB color.
   * @return This {@link PolygonOptions} object with a new stroke color set.
   */
  public PolygonOptions strokeColor(int color) {
    polygon.setStrokeColor(color);
    return this;
  }

  /**
   * Gets the stroke color set for this {@link PolygonOptions} object.
   *
   * @return The stroke color of the polygon in ARGB format.
   */
  public int getStrokeColor() {
    return polygon.getStrokeColor();
  }

  /**
   * Gets the points set for this {@link PolygonOptions} object.
   *
   * @return The list made up of {@link LatLng} points defining the polygon.
   */
  public List<LatLng> getPoints() {
    // the getter gives us a copy, which is the safe thing to do...
    return polygon.getPoints();
  }

  /**
   * Gets the holes set for this {@link PolygonOptions} object.
   *
   * @return The list made up of {@link List} of {@link List} of {@link LatLng} points defining the holes.
   */
  public List<List<LatLng>> getHoles() {
    return polygon.getHoles();
  }


  /**
   * Compares this {@link PolygonOptions} object with another {@link PolygonOptions} and
   * determines if their color, alpha, stroke color, and vertices match.
   *
   * @param o Another {@link PolygonOptions} to compare with this object.
   * @return True if color, alpha, stroke color, vertices and holes match this {@link PolygonOptions}
   * {@link PolygonOptions} object. Else, false.
   */
  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    PolygonOptions polygon = (PolygonOptions) o;

    if (Float.compare(polygon.getAlpha(), getAlpha()) != 0) {
      return false;
    }
    if (getFillColor() != polygon.getFillColor()) {
      return false;
    }
    if (getStrokeColor() != polygon.getStrokeColor()) {
      return false;
    }
    if (getPoints() != null ? !getPoints().equals(polygon.getPoints()) : polygon.getPoints() != null) {
      return false;
    }
    return !(getHoles() != null ? !getHoles().equals(polygon.getHoles()) : polygon.getHoles() != null);
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
    result = 31 * result + getFillColor();
    result = 31 * result + getStrokeColor();
    result = 31 * result + (getPoints() != null ? getPoints().hashCode() : 0);
    result = 31 * result + (getHoles() != null ? getHoles().hashCode() : 0);
    return result;
  }
}
