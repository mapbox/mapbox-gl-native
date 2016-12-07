package com.mapbox.mapboxsdk.maps;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;

import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;

import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class TrackingSettingsTest {

    @InjectMocks
    MyLocationView myLocationView = mock(MyLocationView.class);

    @InjectMocks
    UiSettings uiSettings = mock(UiSettings.class);

    @InjectMocks
    FocalPointChangeListener focalPointChangeListener = mock(FocalPointChangeListener.class);

    private TrackingSettings trackingSettings;

    @Before
    public void beforeTest() {
        trackingSettings = new TrackingSettings(myLocationView, uiSettings, focalPointChangeListener);
    }

    @Test
    public void testSanity() {
        assertNotNull("trackingsettings should not be null", trackingSettings);
    }
    
    @Test
    public void testDismissTrackingModesOnGesture() {
        trackingSettings.setDismissTrackingOnGesture(false);
        assertFalse("DismissTrackingOnGesture should be false", trackingSettings.isDismissTrackingOnGesture());
    }

    @Test
    public void testValidateGesturesForTrackingModes() {
        trackingSettings.setDismissTrackingOnGesture(false);
        trackingSettings.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW);
        assertFalse("DismissTrackingOnGesture should be false", trackingSettings.isDismissTrackingOnGesture());
    }

    @Test
    public void testMyLocationEnabled() {
        // setup mock context to provide accepted location permission
        Context context = mock(Context.class);
        when(myLocationView.getContext()).thenReturn(context);
        when(context.checkPermission(eq(Manifest.permission.ACCESS_COARSE_LOCATION), anyInt(), anyInt())).thenReturn(PackageManager.PERMISSION_GRANTED);

        assertFalse("Location should be disabled by default.", trackingSettings.isMyLocationEnabled());
        trackingSettings.setMyLocationEnabled(true);
        assertTrue("Location should be enabled", trackingSettings.isMyLocationEnabled());
    }
}
