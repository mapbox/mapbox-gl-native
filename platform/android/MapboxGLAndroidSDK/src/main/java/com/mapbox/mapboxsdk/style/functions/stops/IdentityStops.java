package com.mapbox.mapboxsdk.style.functions.stops;

/**
 * The {@link Stops} implementation for identity functions
 *
 * @param <T> the input/output type
 */
public class IdentityStops<T> extends Stops<T, T> {

  /**
   * {@inheritDoc}
   */
  @Override
  protected String getTypeName() {
    return "identity";
  }

}
