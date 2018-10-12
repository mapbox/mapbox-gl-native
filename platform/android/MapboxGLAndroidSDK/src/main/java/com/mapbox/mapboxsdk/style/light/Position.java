package com.mapbox.mapboxsdk.style.light;

import android.support.annotation.Keep;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

/**
 * Position of the light source relative to lit (extruded) geometries.
 * <p>
 * The position is constructed out of a radial coordinate, an azimuthal angle and a polar angle.
 * where the radial coordinate indicates the distance from the center of the base of an object to its light, the
 * azimuthal angle indicates the position of the light relative to 0&#xB0; (0&#xB0; when
 * {@link com.mapbox.mapboxsdk.style.layers.Property.ANCHOR} is set to viewport corresponds to the top of the
 * viewport, or 0&#xB0; when {@link com.mapbox.mapboxsdk.style.layers.Property.ANCHOR} is set to map corresponds to due
 * north, and degrees proceed clockwise), and polar indicates the height of the light
 * (from 0&#xB0;, directly above, to 180&#xB0;, directly below).
 */
public class Position {

  @Keep
  private float radialCoordinate;
  @Keep
  private float azimuthalAngle;
  @Keep
  private float polarAngle;

  /**
   * Creates a Position from a radial coordinate, an azimuthal angle and a polar angle.
   *
   * @param radialCoordinate the distance from the center of the base of an object to its light
   * @param azimuthalAngle the position of the light relative to 0&#xB0;
   * @param polarAngle the height of the light
   */
  public Position(float radialCoordinate, float azimuthalAngle, float polarAngle) {
    this.radialCoordinate = radialCoordinate;
    this.azimuthalAngle = azimuthalAngle;
    this.polarAngle = polarAngle;
  }

  /**
   * Returns a Position from a radial coordinate, an azimuthal angle and a polar angle
   *
   * @param radialCoordinate the radial coordinate
   * @param azimuthalAngle the azimuthal angle
   * @param polarAngle the polar angle
   * @return the created Position object
   */
  @Keep
  public static Position fromPosition(float radialCoordinate, float azimuthalAngle, float polarAngle) {
    return new Position(radialCoordinate, azimuthalAngle, polarAngle);
  }

  @Override
  public boolean equals(@Nullable Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }

    Position position = (Position) o;

    if (Float.compare(position.radialCoordinate, radialCoordinate) != 0) {
      return false;
    }
    if (Float.compare(position.azimuthalAngle, azimuthalAngle) != 0) {
      return false;
    }
    return Float.compare(position.polarAngle, polarAngle) == 0;
  }

  @Override
  public int hashCode() {
    int result = (radialCoordinate != +0.0f ? Float.floatToIntBits(radialCoordinate) : 0);
    result = 31 * result + (azimuthalAngle != +0.0f ? Float.floatToIntBits(azimuthalAngle) : 0);
    result = 31 * result + (polarAngle != +0.0f ? Float.floatToIntBits(polarAngle) : 0);
    return result;
  }

  @NonNull
  @Override
  public String toString() {
    return "Position{"
      + "radialCoordinate=" + radialCoordinate
      + ", azimuthalAngle=" + azimuthalAngle
      + ", polarAngle=" + polarAngle
      + '}';
  }
}
