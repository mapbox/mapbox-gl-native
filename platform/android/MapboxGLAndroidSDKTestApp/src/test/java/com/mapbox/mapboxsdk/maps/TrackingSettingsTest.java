package com.mapbox.mapboxsdk.maps;

import com.mapbox.mapboxsdk.constants.MyBearingTracking;
import com.mapbox.mapboxsdk.constants.MyLocationTracking;

import org.junit.Test;
import org.mockito.InjectMocks;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.mockito.Mockito.mock;

public class TrackingSettingsTest {

    @InjectMocks
    MapView mMapView = mock(MapView.class);

    @Test
    public void testSanity() {
        TrackingSettings trackingSettings = new TrackingSettings(mMapView, new UiSettings(mMapView));
        assertNotNull("trackingsettings should not be null", trackingSettings);
    }

    @Test
    public void testMyLocationTrackingMode() {
        TrackingSettings trackingSettings = new TrackingSettings(mMapView, new UiSettings(mMapView));
        trackingSettings.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW);
        assertEquals("MyLocationTrackingMode should match", MyLocationTracking.TRACKING_FOLLOW, trackingSettings.getMyLocationTrackingMode());
    }

    @Test
    public void testMyBearingTrackingMode() {
        TrackingSettings trackingSettings = new TrackingSettings(mMapView, new UiSettings(mMapView));
        trackingSettings.setMyBearingTrackingMode(MyBearingTracking.COMPASS);
        assertEquals("MyLocationTrackingMode should match", MyBearingTracking.COMPASS, trackingSettings.getMyBearingTrackingMode());
    }

    @Test
    public void testDismissTrackingModesOnGesture() {
        TrackingSettings trackingSettings = new TrackingSettings(mMapView, new UiSettings(mMapView));
        trackingSettings.setDismissTrackingOnGesture(false);
        assertFalse("DismissTrackingOnGesture should be false", trackingSettings.isDismissTrackingOnGesture());
    }

    @Test
    public void testValidateGesturesForTrackingModes(){
        TrackingSettings trackingSettings = new TrackingSettings(mMapView, new UiSettings(mMapView));
        trackingSettings.setDismissTrackingOnGesture(false);
        trackingSettings.setMyLocationTrackingMode(MyLocationTracking.TRACKING_FOLLOW);
        assertFalse("DismissTrackingOnGesture should be false", trackingSettings.isDismissTrackingOnGesture());
    }
}
