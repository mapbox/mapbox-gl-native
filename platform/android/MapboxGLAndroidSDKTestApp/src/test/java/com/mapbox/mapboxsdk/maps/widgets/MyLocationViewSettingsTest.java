package com.mapbox.mapboxsdk.maps.widgets;

import android.graphics.Color;
import android.graphics.drawable.Drawable;

import com.mapbox.mapboxsdk.maps.MapView;

import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;

import java.util.Arrays;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertFalse;
import static junit.framework.Assert.assertNotNull;
import static junit.framework.Assert.assertTrue;
import static org.mockito.Mockito.mock;

public class MyLocationViewSettingsTest {

    @InjectMocks
    MapView mMapView = mock(MapView.class);

    @InjectMocks
    UserLocationView userLocationView = mock(UserLocationView.class);

    MyLocationViewSettings locationViewSettings;

    @Before
    public void beforeTest() {
        locationViewSettings = new MyLocationViewSettings(mMapView, userLocationView);
    }

    @Test
    public void testSanity() {
        assertNotNull("should not be null", locationViewSettings);
    }

    @Test
    public void testForegroundDrawables() {
        Drawable foregroundDrawable = mock(Drawable.class);
        Drawable foregroundBearingDrawable = mock(Drawable.class);
        locationViewSettings.setForegroundDrawable(foregroundDrawable, foregroundBearingDrawable);
        assertEquals("foreground should match", foregroundDrawable, locationViewSettings.getForegroundDrawable());
        assertEquals("foreground bearing should match", foregroundBearingDrawable, locationViewSettings.getForegroundBearingDrawable());
    }

    @Test
    public void testBackgroundDrawable() {
        Drawable backgroundDrawable = mock(Drawable.class);
        locationViewSettings.setBackgroundDrawable(backgroundDrawable);
        assertEquals("foreground should match", backgroundDrawable, locationViewSettings.getBackgroundDrawable());
    }

    @Test
    public void testBackgroundOffset() {
        int[] offset = new int[]{1, 2, 3, 4};
        locationViewSettings.setBackgroundOffset(1, 2, 3, 4);
        assertTrue("offsets should match", Arrays.equals(offset, locationViewSettings.getBackgroundOffset()));
    }

    @Test
    public void testForegroundTint() {
        int color = Color.RED;
        locationViewSettings.setForegroundTintColor(Color.RED);
        assertEquals("color should match", color, locationViewSettings.getForegroundTintColor());
    }

    @Test
    public void testBackgroundTint(){
        int color = Color.RED;
        locationViewSettings.setBackgroundTintColor(Color.RED);
        assertEquals("color should match", color, locationViewSettings.getBackgroundTintColor());
    }

    @Test
    public void testEnabled() {
        assertFalse("initial state should be false", locationViewSettings.isEnabled());
        locationViewSettings.setEnabled(true);
        assertTrue("state should be true", locationViewSettings.isEnabled());
    }

}
