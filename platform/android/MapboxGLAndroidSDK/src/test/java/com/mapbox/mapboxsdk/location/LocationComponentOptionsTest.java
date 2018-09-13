package com.mapbox.mapboxsdk.location;

import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;

import com.mapbox.mapboxsdk.R;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExpectedException;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.junit.MockitoJUnitRunner;

import static org.junit.Assert.assertNotNull;
import static org.mockito.Mockito.when;

@RunWith(MockitoJUnitRunner.class)
public class LocationComponentOptionsTest {

  @Mock
  private Context context;
  @Mock
  private TypedArray array;
  @Mock
  private Resources resources;

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
    LocationComponentOptions locationComponentOptions = LocationComponentOptions.builder(context)
      .accuracyAlpha(0.5f)
      .build();
    assertNotNull(locationComponentOptions);
  }

  @Test
  public void passingOutOfRangeAccuracyAlpha_throwsException() throws Exception {
    thrown.expect(IllegalArgumentException.class);
    thrown.expectMessage("Accuracy alpha value must be between 0.0 and "
      + "1.0.");
    LocationComponentOptions.builder(context)
      .accuracyAlpha(2f)
      .build();
  }

  @Test
  public void negativeElevation_causesExceptionToBeThrown() throws Exception {
    thrown.expect(IllegalArgumentException.class);
    thrown.expectMessage("Invalid shadow size -500.0. Must be >= 0");
    LocationComponentOptions.builder(context)
      .elevation(-500)
      .build();
  }
}