package com.mapbox.mapboxsdk.geometry;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * ProjectedMeters is a projection of longitude, latitude points in Mercator meters.
 * <p>
 * these have been projected into Mapbox GL's Mercator projection. Instead of decimal
 * degrees, it uses Mercator meters (which are notably not equivalent to SI meters)
 * except at the equator.
 * </p>
 */
public class ProjectedMeters implements IProjectedMeters, Parcelable {

  /**
   * Inner class responsible for recreating Parcels into objects.
   */
  public static final Creator<ProjectedMeters> CREATOR = new Creator<ProjectedMeters>() {
    public ProjectedMeters createFromParcel(Parcel in) {
      return new ProjectedMeters(in);
    }

    public ProjectedMeters[] newArray(int size) {
      return new ProjectedMeters[size];
    }
  };

  private double northing;
  private double easting;

  /**
   * Creates a ProjectedMeters based on projected meters in north and east direction.
   *
   * @param northing the northing in meters
   * @param easting  the easting in meters
   */
  public ProjectedMeters(double northing, double easting) {
    this.northing = northing;
    this.easting = easting;
  }

  /**
   * Creates a ProjecteMeters based on another set of projected meters.
   *
   * @param projectedMeters The projected meters to be based on.
   */
  public ProjectedMeters(ProjectedMeters projectedMeters) {
    this.northing = projectedMeters.northing;
    this.easting = projectedMeters.easting;
  }

  /**
   * Creates a ProjectedMeters from a Parcel.
   *
   * @param in The parcel to create from
   * @return a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
   */
  private ProjectedMeters(Parcel in) {
    northing = in.readDouble();
    easting = in.readDouble();
  }

  /**
   * Get projected meters in north direction.
   *
   * @return Projected meters in north.
   */
  @Override
  public double getNorthing() {
    return northing;
  }

  /**
   * Get projected meters in east direction.
   *
   * @return Projected meters in east.
   */
  @Override
  public double getEasting() {
    return easting;
  }

  /**
   * Indicates whether some other object is "equal to" this one.
   *
   * @param other The object to compare this to
   * @return true if equal, false if not
   */
  @Override
  public boolean equals(Object other) {
    if (this == other) {
      return true;
    }
    if (other == null || getClass() != other.getClass()) {
      return false;
    }

    ProjectedMeters projectedMeters = (ProjectedMeters) other;

    return Double.compare(projectedMeters.easting, easting) == 0
      && Double.compare(projectedMeters.northing, northing) == 0;
  }

  /**
   * Returns a hash code value for the object.
   *
   * @return the hash code of this
   */
  @Override
  public int hashCode() {
    int result;
    long temp;
    temp = Double.doubleToLongBits(easting);
    result = (int) (temp ^ (temp >>> 32));
    temp = Double.doubleToLongBits(northing);
    result = 31 * result + (int) (temp ^ (temp >>> 32));
    return result;
  }

  /**
   * Returns a string representation of the object.
   *
   * @return the string representation of this
   */
  @Override
  public String toString() {
    return "ProjectedMeters [northing=" + northing + ", easting=" + easting + "]";
  }

  /**
   * Describe the kinds of special objects contained in this Parcelable instance's marshaled representation.
   *
   * @return a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
   */
  @Override
  public int describeContents() {
    return 0;
  }

  /**
   * Flatten this object in to a Parcel.
   *
   * @param out   The Parcel in which the object should be written.
   * @param flags Additional flags about how the object should be written
   */
  @Override
  public void writeToParcel(Parcel out, int flags) {
    out.writeDouble(northing);
    out.writeDouble(easting);
  }
}
