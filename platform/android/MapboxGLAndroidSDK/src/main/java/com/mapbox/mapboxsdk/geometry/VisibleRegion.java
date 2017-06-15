package com.mapbox.mapboxsdk.geometry;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * Contains the four points defining the four-sided polygon that is visible in a map's camera.
 * This polygon can be a trapezoid instead of a rectangle, because a camera can have tilt.
 * If the camera is directly over the center of the camera, the shape is rectangular,
 * but if the camera is tilted, the shape will appear to be a trapezoid whose
 * smallest side is closest to the point of view.
 */
public class VisibleRegion implements Parcelable {

  /**
   * LatLng object that defines the far left corner of the camera.
   */
  public final LatLng farLeft;

  /**
   * LatLng object that defines the far right corner of the camera.
   */
  public final LatLng farRight;

  /**
   * LatLng object that defines the bottom left corner of the camera.
   */
  public final LatLng nearLeft;

  /**
   * LatLng object that defines the bottom right corner of the camera.
   */
  public final LatLng nearRight;

  /**
   * The smallest bounding box that includes the visible region defined in this class.
   */
  public final LatLngBounds latLngBounds;

  /**
   * Creates a VisibleRegion from a Parcel.
   *
   * @param in The Parcel to create this from
   */
  private VisibleRegion(Parcel in) {
    this.farLeft = in.readParcelable(LatLng.class.getClassLoader());
    this.farRight = in.readParcelable(LatLng.class.getClassLoader());
    this.nearLeft = in.readParcelable(LatLng.class.getClassLoader());
    this.nearRight = in.readParcelable(LatLng.class.getClassLoader());
    this.latLngBounds = in.readParcelable(LatLngBounds.class.getClassLoader());
  }

  /**
   * Creates a VisibleRegion given the four corners of the camera.
   *
   * @param farLeft      A LatLng object containing the latitude and longitude of the near left corner of the region.
   * @param farRight     A LatLng object containing the latitude and longitude of the near left corner of the region.
   * @param nearLeft     A LatLng object containing the latitude and longitude of the near left corner of the region.
   * @param nearRight    A LatLng object containing the latitude and longitude of the near left corner of the region.
   * @param latLngBounds The smallest bounding box that includes the visible region defined in this class.
   */
  public VisibleRegion(LatLng farLeft, LatLng farRight, LatLng nearLeft, LatLng nearRight, LatLngBounds latLngBounds) {
    this.farLeft = farLeft;
    this.farRight = farRight;
    this.nearLeft = nearLeft;
    this.nearRight = nearRight;
    this.latLngBounds = latLngBounds;
  }

  /**
   * Compares this VisibleRegion to another object.
   * If the other object is actually a pointer to this object,
   * or if all four corners and the bounds of the two objects are the same,
   * this method returns true. Otherwise, this method returns false.
   *
   * @param o The Object to compare with.
   * @return true if both objects are the same object.
   */
  @Override
  public boolean equals(Object o) {
    if (!(o instanceof VisibleRegion)) {
      return false;
    }
    if (o == this) {
      return true;
    }

    VisibleRegion visibleRegion = (VisibleRegion) o;
    return farLeft.equals(visibleRegion.farLeft)
      && farRight.equals(visibleRegion.farRight)
      && nearLeft.equals(visibleRegion.nearLeft)
      && nearRight.equals(visibleRegion.nearRight)
      && latLngBounds.equals(visibleRegion.latLngBounds);
  }

  /**
   * The string representation of the object.
   *
   * @return the string representation of this
   */
  @Override
  public String toString() {
    return "[farLeft [" + farLeft + "], farRight [" + farRight + "], nearLeft [" + nearLeft + "], nearRight ["
      + nearRight + "], latLngBounds [" + latLngBounds + "]]";
  }

  /**
   * Returns a hash code value for the object.
   *
   * @return the hash code
   */
  @Override
  public int hashCode() {
    return ((farLeft.hashCode() + 90)
      + ((farRight.hashCode() + 90) * 1000)
      + ((nearLeft.hashCode() + 180) * 1000000)
      + ((nearRight.hashCode() + 180) * 1000000000));
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
    out.writeParcelable(farLeft, flags);
    out.writeParcelable(farRight, flags);
    out.writeParcelable(nearLeft, flags);
    out.writeParcelable(nearRight, flags);
    out.writeParcelable(latLngBounds, flags);
  }

  /**
   * Inner class responsible for recreating Parcels into objects.
   */
  public static final Parcelable.Creator<VisibleRegion> CREATOR =
    new Parcelable.Creator<VisibleRegion>() {
      public VisibleRegion createFromParcel(Parcel in) {
        return new VisibleRegion(in);
      }

      public VisibleRegion[] newArray(int size) {
        return new VisibleRegion[size];
      }
    };

}
