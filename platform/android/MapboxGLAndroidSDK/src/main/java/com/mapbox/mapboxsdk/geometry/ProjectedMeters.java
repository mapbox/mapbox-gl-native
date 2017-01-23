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

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    ProjectedMeters projectedMeters = (ProjectedMeters) o;

    return Double.compare(projectedMeters.easting, easting) == 0
      && Double.compare(projectedMeters.northing, northing) == 0;

  }

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

  @Override
  public String toString() {
    return "ProjectedMeters [northing=" + northing + ", easting=" + easting + "]";
  }

  @Override
  public int describeContents() {
    return 0;
  }

  @Override
  public void writeToParcel(Parcel out, int flags) {
    out.writeDouble(northing);
    out.writeDouble(easting);
  }
}
