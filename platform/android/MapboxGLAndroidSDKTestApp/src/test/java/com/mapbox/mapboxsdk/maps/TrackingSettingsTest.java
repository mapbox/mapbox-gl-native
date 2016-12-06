package com.mapbox.mapboxsdk.maps;

import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;
import com.mapbox.mapboxsdk.maps.widgets.MyLocationView;

import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.mockito.Mockito.mock;

public class TrackingSettingsTest {

    @InjectMocks
    MyLocationView myLocationView = mock(MyLocationView.class);

    @InjectMocks
    UiSettings uiSettings = mock(UiSettings.class);

    @InjectMocks
    FocalPointChangeListener focalPointChangeListener = mock(FocalPointChangeListener.class);

    private TrackingSettings trackingSettings;

    @Before
    public void beforeTest(){
        trackingSettings = new TrackingSettings(myLocationView, uiSettings, focalPointChangeListener);
    }

    @Test
    public void testSanity() {
        assertNotNull("trackingsettings should not be null", trackingSettings);
    }

    @Test
    public void testMyLocationTrackingMode() {
        trackingSettings.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW);
        assertEquals("MyLocationTrackingMode should match", MyLocationTracking.TRACKING_FOLLOW, trackingSettings.getMyLocationTrackingMode());
    }

    @Test
    public void testMyBearingTrackingMode() {
        trackingSettings.setMyBearingTrackingMode(MyBearingTracking.COMPASS);
        assertEquals("MyLocationTrackingMode should match", MyBearingTracking.COMPASS, trackingSettings.getMyBearingTrackingMode());
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
}
