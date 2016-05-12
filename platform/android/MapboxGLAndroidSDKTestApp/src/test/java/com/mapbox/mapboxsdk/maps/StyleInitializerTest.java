package com.mapbox.mapboxsdk.maps;

import android.content.Context;
import android.content.res.Resources;

import com.mapbox.mapboxsdk.testapp.R;
import com.mapbox.mapboxsdk.testapp.model.constants.AppConstant;

import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;
import org.mockito.MockitoAnnotations;

import java.util.Locale;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class StyleInitializerTest {

    @InjectMocks
    Context context = mock(Context.class);

    @InjectMocks
    Resources resources = mock(Resources.class);

    MapView.StyleInitializer initializer;

    @Before
    public void beforeTest() {
        MockitoAnnotations.initMocks(this);
        when(context.getResources()).thenReturn(resources);
        when(resources.getInteger(R.integer.style_version)).thenReturn(AppConstant.STYLE_VERSION);
        initializer = new MapView.StyleInitializer(context);
    }

    @Test
    public void testSanity() {
        assertNotNull("initializer should not be null", initializer);
    }

    @Test
    public void testDefaultStyle() {
        assertTrue(initializer.isDefaultStyle());
        assertEquals(String.format(Locale.US, "mapbox://styles/mapbox/streets-v%d", AppConstant.STYLE_VERSION), "mapbox://styles/mapbox/streets-v9");
    }

    @Test
    public void testUpdateStyle() {
        String customStyle = "test";
        initializer.setStyle(customStyle);
        assertFalse(initializer.isDefaultStyle());
        assertEquals(customStyle, initializer.getStyle());
    }

    @Test
    public void testUpdateStyleNull() {
        String customStyle = null;
        initializer.setStyle(customStyle);
        assertTrue(initializer.isDefaultStyle());
        assertEquals(String.format(Locale.US, "mapbox://styles/mapbox/streets-v%d", AppConstant.STYLE_VERSION), "mapbox://styles/mapbox/streets-v9");
    }

    @Test
    public void testOverrideDefaultStyle() {
        String customStyle = "test";
        initializer.setStyle(customStyle, true);
        assertTrue(initializer.isDefaultStyle());
        assertEquals(customStyle, initializer.getStyle());
    }
}
