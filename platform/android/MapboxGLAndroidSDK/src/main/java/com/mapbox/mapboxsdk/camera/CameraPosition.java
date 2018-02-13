package com.mapbox.mapboxsdk.camera;

import android.content.res.TypedArray;
import android.os.Parcel;
import android.os.Parcelable;
import android.support.annotation.FloatRange;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.utils.MathUtils;

/**
 * Resembles the position, angle, zoom and tilt of the user's viewpoint.
 */
public final class CameraPosition implements Parcelable {

  public static final CameraPosition DEFAULT = new CameraPosition(new LatLng(), 0, 0, 0);

  public static final Parcelable.Creator<CameraPosition> CREATOR =
    new Parcelable.Creator<CameraPosition>() {
      public CameraPosition createFromParcel(Parcel in) {
        double bearing = in.readDouble();
        LatLng target = in.readParcelable(LatLng.class.getClassLoader());
        double tilt = in.readDouble();
        double zoom = in.readDouble();
        return new CameraPosition(target, zoom, tilt, bearing);
      }

      public CameraPosition[] newArray(int size) {
        return new CameraPosition[size];
      }
    };

  /**
   * Direction that the camera is pointing in, in degrees clockwise from north.
   */
  public final double bearing;

  /**
   * The location that the camera is pointing at.
   */
  public final LatLng target;

  /**
   * The angle, in degrees, of the camera angle from the nadir (directly facing the Earth).
   * See tilt(float) for details of restrictions on the range of values.
   */
  public final double tilt;

  /**
   * Zoom level near the center of the screen. See zoom(float) for the definition of the camera's
   * zoom level.
   */
  public final double zoom;

  /**
   * Constructs a CameraPosition.
   *
   * @param target  The target location to align with the center of the screen.
   * @param zoom    Zoom level at target. See zoom(float) for details of restrictions.
   * @param tilt    The camera angle, in degrees, from the nadir (directly down). See tilt(float)
   *                for details of restrictions.
   * @param bearing Direction that the camera is pointing in, in degrees clockwise from north.
   *                This value will be normalized to be within 0 degrees inclusive and 360 degrees
   *                exclusive.
   * @throws NullPointerException     if target is null
   * @throws IllegalArgumentException if tilt is outside the range of 0 to 90 degrees inclusive.
   */
  CameraPosition(LatLng target, double zoom, double tilt, double bearing) {
    this.target = target;
    this.bearing = bearing;
    this.tilt = tilt;
    this.zoom = zoom;
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
    out.writeDouble(bearing);
    out.writeParcelable(target, flags);
    out.writeDouble(tilt);
    out.writeDouble(zoom);
  }

  /**
   * Returns a String with the camera target, zoom, bearing and tilt.
   *
   * @return A String with CameraPosition information.
   */
  @Override
  public String toString() {
    return "Target: " + target + ", Zoom:" + zoom + ", Bearing:" + bearing + ", Tilt:" + tilt;
  }

  /**
   * Compares this {@link CameraPosition} object with another {@link CameraPosition} and
   * determines if their target, zoom, tilt, and bearing match.
   *
   * @param o Another {@link CameraPosition} to compare with this object.
   * @return True if target, zoom, tilt, and bearing match this {@link CameraPosition} object.
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

    CameraPosition cameraPosition = (CameraPosition) o;
    if (target != null && !target.equals(cameraPosition.target)) {
      return false;
    } else if (zoom != cameraPosition.zoom) {
      return false;
    } else if (tilt != cameraPosition.tilt) {
      return false;
    } else if (bearing != cameraPosition.bearing) {
      return false;
    }
    return true;
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
    result = 31 * result + (target != null ? target.hashCode() : 0);
    return result;
  }

  /**
   * Builder for composing CameraPosition objects.
   */
  public static final class Builder {

    private double bearing = -1;
    private LatLng target = null;
    private double tilt = -1;
    private double zoom = -1;

    /**
     * Create an empty builder.
     */
    public Builder() {
      super();
    }

    /**
     * Create a builder with an existing CameraPosition data.
     *
     * @param previous Existing CameraPosition values to use
     */
    public Builder(CameraPosition previous) {
      super();
      if (previous != null) {
        this.bearing = previous.bearing;
        this.target = previous.target;
        this.tilt = previous.tilt;
        this.zoom = previous.zoom;
      }
    }

    /**
     * Create a builder with an existing CameraPosition data.
     *
     * @param typedArray TypedArray containing attribute values
     */
    public Builder(TypedArray typedArray) {
      super();
      if (typedArray != null) {
        this.bearing = typedArray.getFloat(R.styleable.mapbox_MapView_mapbox_cameraBearing, 0.0f);
        double lat = typedArray.getFloat(R.styleable.mapbox_MapView_mapbox_cameraTargetLat, 0.0f);
        double lng = typedArray.getFloat(R.styleable.mapbox_MapView_mapbox_cameraTargetLng, 0.0f);
        this.target = new LatLng(lat, lng);
        this.tilt = typedArray.getFloat(R.styleable.mapbox_MapView_mapbox_cameraTilt, 0.0f);
        this.zoom = typedArray.getFloat(R.styleable.mapbox_MapView_mapbox_cameraZoom, 0.0f);
      }
    }

    /**
     * Create a builder from an existing CameraPositionUpdate update.
     *
     * @param update Update containing camera options
     */
    public Builder(CameraUpdateFactory.CameraPositionUpdate update) {
      super();
      if (update != null) {
        bearing = update.getBearing();
        target = update.getTarget();
        tilt = update.getTilt();
        zoom = update.getZoom();
      }
    }

    /**
     * Create builder from an existing CameraPositionUpdate update.
     *
     * @param update Update containing camera options
     */
    public Builder(CameraUpdateFactory.ZoomUpdate update) {
      super();
      if (update != null) {
        this.zoom = update.getZoom();
      }
    }

    /**
     * Sets the direction that the camera is pointing in, in degrees clockwise from north.
     *
     * @param bearing Bearing
     * @return this
     */
    public Builder bearing(double bearing) {
      double direction = bearing;

      while (direction >= 360) {
        direction -= 360;
      }
      while (direction < 0) {
        direction += 360;
      }

      this.bearing = direction;
      return this;
    }

    /**
     * Sets the location where the camera is pointing at.
     *
     * @param location target of the camera
     * @return this
     */
    public Builder target(LatLng location) {
      this.target = location;
      return this;
    }

    /**
     * Set the tilt of the camera in degrees
     * <p>
     * value is clamped to {@link MapboxConstants#MINIMUM_TILT} and {@link MapboxConstants#MAXIMUM_TILT}.
     * </p>
     *
     * @param tilt Tilt value of the camera
     * @return this
     */
    public Builder tilt(@FloatRange(from = MapboxConstants.MINIMUM_TILT,
      to = MapboxConstants.MAXIMUM_TILT) double tilt) {
      this.tilt = MathUtils.clamp(tilt, MapboxConstants.MINIMUM_TILT, MapboxConstants.MAXIMUM_TILT);
      return this;
    }

    /**
     * Set the zoom of the camera
     * <p>
     * Zoom ranges from {@link MapboxConstants#MINIMUM_ZOOM} to {@link MapboxConstants#MAXIMUM_ZOOM}
     * </p>
     *
     * @param zoom Zoom value of the camera
     * @return this
     */
    public Builder zoom(@FloatRange(from = MapboxConstants.MINIMUM_ZOOM,
      to = MapboxConstants.MAXIMUM_ZOOM) double zoom) {
      this.zoom = zoom;
      return this;
    }

    /**
     * Builds the CameraPosition.
     *
     * @return CameraPosition
     */
    public CameraPosition build() {
      return new CameraPosition(target, zoom, tilt, bearing);
    }
  }
}
