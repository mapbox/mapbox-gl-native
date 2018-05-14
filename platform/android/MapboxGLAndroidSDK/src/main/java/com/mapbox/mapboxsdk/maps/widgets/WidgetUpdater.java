package com.mapbox.mapboxsdk.maps.widgets;

import android.arch.lifecycle.LifecycleOwner;
import android.arch.lifecycle.ViewModelProviders;
import android.content.Context;
import android.graphics.Color;
import android.support.annotation.NonNull;
import android.support.v4.app.FragmentActivity;
import android.support.v4.content.ContextCompat;
import android.view.View;
import android.widget.ImageView;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.maps.UiSettings;
import com.mapbox.mapboxsdk.utils.ColorUtils;
import com.mapbox.mapboxsdk.utils.ViewUtils;

/**
 * Internal class used to update widget views to reflect {@link UiSettings} changes.
 */
public final class WidgetUpdater {
  private final CompassView compassView;
  private final ImageView attributionView;
  private final View logoView;

  private final UiSettings uiSettings;

  public WidgetUpdater(@NonNull Context context, @NonNull CompassView compassView,
                       @NonNull ImageView attributionView, @NonNull View logoView) {
    this.compassView = compassView;
    this.attributionView = attributionView;
    this.logoView = logoView;

    uiSettings = ViewModelProviders.of((FragmentActivity) context).get(UiSettings.class);
    initialiseCompassObservableSettings(context);
    initialiseAttributionObservableSettings(context);
    initialiseLogoObservableSettings(context);
  }

  private void initialiseCompassObservableSettings(Context context) {
    LifecycleOwner lifecycleOwner = (LifecycleOwner) context;

    // gravity
    uiSettings.getCompassGravityObservable().observe(lifecycleOwner,
      gravity -> ViewUtils.setViewGravity(compassView, gravity));

    // image
    uiSettings.getCompassImageObservable().observe(lifecycleOwner, compassView::setImageDrawable);

    // enabled
    uiSettings.isCompassEnabledObservable().observe(lifecycleOwner, compassView::setEnabled);

    // fading north
    uiSettings.isCompassFadeWhenFacingNorthObservable().observe(lifecycleOwner,
      compassView::hideCompassViewFacingNorth);

    // margins
    uiSettings.getCompassMarginsObservable().observe(lifecycleOwner, margins ->
      ViewUtils.setViewMargins(compassView, margins[0], margins[1], margins[2], margins[3]));

    // rotation
    uiSettings.getCompassRotationObservable().observe(lifecycleOwner, compassView::updateBearing);
  }

  private void initialiseAttributionObservableSettings(Context context) {
    LifecycleOwner lifecycleOwner = (LifecycleOwner) context;

    // gravity
    uiSettings.getAttributionGravityObservable().observe(lifecycleOwner,
      gravity -> ViewUtils.setViewGravity(attributionView, gravity));

    // enabled
    uiSettings.isAttributionEnabledObservable().observe(lifecycleOwner,
      enabled -> attributionView.setVisibility(enabled ? View.VISIBLE : View.GONE));

    // tint color
    uiSettings.getAttributionTintColorObservable().observe(lifecycleOwner, tintColor -> {
      // Check that the tint color being passed in isn't transparent.
      if (Color.alpha(tintColor) == 0) {
        ColorUtils.setTintList(attributionView,
          ContextCompat.getColor(attributionView.getContext(), R.color.mapbox_blue));
      } else {
        ColorUtils.setTintList(attributionView, tintColor);
      }
    });

    // margins
    uiSettings.getAttributionMarginsObservable().observe(lifecycleOwner, margins ->
      ViewUtils.setViewMargins(attributionView, margins[0], margins[1], margins[2], margins[3]));
  }

  private void initialiseLogoObservableSettings(Context context) {
    LifecycleOwner lifecycleOwner = (LifecycleOwner) context;

    // gravity
    uiSettings.getLogoGravityObservable().observe(lifecycleOwner,
      gravity -> ViewUtils.setViewGravity(logoView, gravity));

    // enabled
    uiSettings.isLogoEnabledObservable().observe(lifecycleOwner,
      enabled -> logoView.setVisibility(enabled ? View.VISIBLE : View.GONE));

    // margins
    uiSettings.getLogoMarginsObservable().observe(lifecycleOwner, margins ->
      ViewUtils.setViewMargins(logoView, margins[0], margins[1], margins[2], margins[3]));
  }
}
