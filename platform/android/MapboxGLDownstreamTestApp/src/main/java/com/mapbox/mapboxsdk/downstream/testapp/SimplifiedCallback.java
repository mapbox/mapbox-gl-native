package com.mapbox.mapboxsdk.downstream.testapp;

import com.mapbox.api.directions.v5.models.DirectionsResponse;

import retrofit2.Call;
import retrofit2.Callback;
import timber.log.Timber;

/**
 * Helper class to reduce redundant logging code when no other action is taken in onFailure
 */
public abstract class SimplifiedCallback implements Callback<DirectionsResponse> {
  @Override
  public void onFailure(Call<DirectionsResponse> call, Throwable throwable) {
    Timber.e(throwable, throwable.getMessage());
  }
}

