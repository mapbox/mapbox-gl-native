package com.mapbox.mapboxsdk.location;

import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import androidx.annotation.NonNull;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.maps.Style;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExpectedException;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.junit.MockitoJUnit;
import org.mockito.junit.MockitoJUnitRunner;
import org.mockito.junit.MockitoRule;

import static org.junit.Assert.assertNotNull;
import static org.mockito.Mockito.when;

@RunWith(MockitoJUnitRunner.class)
public class LocationComponentActivationOptionsTest {

  @Mock
  private Context context;
  @Mock
  private TypedArray array;
  @Mock
  private Resources resources;
  @Mock
  private Style style;

  @Rule
  public MockitoRule mockitoRule = MockitoJUnit.rule();

  @NonNull
  @Rule
  public ExpectedException thrown = ExpectedException.none();

  @Before
  public void setUp() throws Exception {
    when(context.obtainStyledAttributes(R.style.mapbox_LocationComponent, R.styleable.mapbox_LocationComponent))
      .thenReturn(array);
    when(array.getResourceId(R.styleable.mapbox_LocationComponent_mapbox_foregroundDrawable, -1))
      .thenReturn(R.drawable.mapbox_user_icon);
    when(context.getResources()).thenReturn(resources);
  }

  @Test
  public void sanity() throws Exception {
    when(style.isFullyLoaded()).thenReturn(true);

    LocationComponentOptions locationComponentOptions = LocationComponentOptions.builder(context)
      .accuracyAlpha(0.5f)
      .build();
    assertNotNull(locationComponentOptions);

    LocationComponentActivationOptions locationComponentActivationOptions =
      LocationComponentActivationOptions.builder(context, style)
        .locationComponentOptions(locationComponentOptions)
        .useDefaultLocationEngine(true)
        .build();
    assertNotNull(locationComponentActivationOptions);
  }

  @Test
  public void includingBothStyleResAndComponentOptions_causesExceptionToBeThrown() throws Exception {

    thrown.expect(IllegalArgumentException.class);
    thrown.expectMessage("You've provided both a style resource and a LocationComponentOptions"
      + " object to the LocationComponentActivationOptions builder. You can't use both and "
      + "you must choose one of the two to style the LocationComponent.");

    LocationComponentOptions locationComponentOptions = LocationComponentOptions.builder(context)
      .accuracyAlpha(0.5f)
      .build();

    LocationComponentActivationOptions.builder(context, style)
      .locationComponentOptions(locationComponentOptions)
      .styleRes(R.style.mapbox_LocationComponent)
      .build();
  }

  @Test
  public void nullContext_causesExceptionToBeThrown() throws Exception {
    thrown.expect(NullPointerException.class);
    thrown.expectMessage("Context in LocationComponentActivationOptions is null.");

    LocationComponentActivationOptions.builder(null, style)
      .build();
  }

  @Test
  public void nullStyle_causesExceptionToBeThrown() throws Exception {
    thrown.expect(NullPointerException.class);
    thrown.expectMessage("Style in LocationComponentActivationOptions is null. Make sure the Style object isn't null."
      + " Wait for the map to fully load before passing the Style object to LocationComponentActivationOptions.");

    LocationComponentActivationOptions.builder(context, null)
      .build();
  }

  @Test
  public void locationComponent_exceptionThrownWithDefaultLocationEngineButNotFullyLoadedStyle() throws Exception {

    when(style.isFullyLoaded()).thenReturn(false);

    thrown.expect(IllegalArgumentException.class);
    thrown.expectMessage("Style in LocationComponentActivationOptions isn't fully loaded. Wait for the "
        + "map to fully load before passing the Style object to "
        + "LocationComponentActivationOptions.");

    LocationComponentActivationOptions.builder(context, style)
        .build();
  }
}