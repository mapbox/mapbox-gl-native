package com.mapbox.mapboxsdk.style.light;

/**
 * Position of the light source relative to lit (extruded) geometries.
 * <p>
 * The position is constructed out of a radial coordinate, an azimuthal angle and a polar angle.
 * where the radial coordinate indicates the distance from the center of the base of an object to its light, the
 * azimuthal angle indicates the position of the light relative to 0° (0° when
 * {@link com.mapbox.mapboxsdk.style.layers.Property.ANCHOR} is set to viewport corresponds to the top of the
 * viewport, or 0° when {@link com.mapbox.mapboxsdk.style.layers.Property.ANCHOR} is set to map corresponds to due
 * north, and degrees proceed clockwise), and polar indicates the height of the light
 * (from 0°, directly above, to 180°, directly below).
 */
public class Position {

  private float radialCoordinate;
  private float azimuthalAngle;
  private float polarAngle;

  /**
   * Creates a Position from a radial coordinate, an azimuthal angle and a polar angle.
   *
   * @param radialCoordinate the distance from the center of the base of an object to its light
   * @param azimuthalAngle the position of the light relative to 0°
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
  public static Position fromPosition(float radialCoordinate, float azimuthalAngle, float polarAngle) {
    return new Position(radialCoordinate, azimuthalAngle, polarAngle);
  }

  @Override
  public boolean equals(Object o) {
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

  @Override
  public String toString() {
    return "Position{"
      + "radialCoordinate=" + radialCoordinate
      + ", azimuthalAngle=" + azimuthalAngle
      + ", polarAngle=" + polarAngle
      + '}';
  }
}
