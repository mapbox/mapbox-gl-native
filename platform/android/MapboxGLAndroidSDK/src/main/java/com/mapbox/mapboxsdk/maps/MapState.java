package com.mapbox.mapboxsdk.maps;

import android.arch.lifecycle.ViewModel;
import android.graphics.drawable.Drawable;
import android.view.Gravity;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.camera.CameraUpdateFactory;

import static com.mapbox.mapboxsdk.maps.MapboxMapOptions.UNDEFINED_COLOR;

public class MapState extends ViewModel {

  private String styleUrl;
  private CameraPosition cameraPosition;
  private boolean nativeDebug;

  // Gestures
  private boolean zoomGesturesEnabled = true;
  private boolean zoomGesturesChangeAllowed = true;
  private boolean scrollGesturesEnabled = true;
  private boolean scrollGesturesChangeAllowed = true;
  private boolean rotateGesturesEnabled = true;
  private boolean rotateGesturesChangeAllowed = true;
  private boolean tiltGesturesEnabled = true;
  private boolean tiltGesturesChangeAllowed = true;
  private boolean doubleTapGesturesEnabled = true;
  private boolean doubleTapGesturesChangeAllowed = true;

  // Compass
  private boolean compassEnabled = true;
  private boolean compassFadingNorth = true;
  private int compassGravity = Gravity.TOP | Gravity.END;
  private int[] compassMargins;
  private Drawable compassImage;

  // Logo
  private boolean logoEnabled = true;
  private int logoGravity = Gravity.BOTTOM | Gravity.START;
  private int[] logoMargins;

  // Attribution
  private boolean attributionEnabled = true
  private int attributionGravity = Gravity.BOTTOM | Gravity.START;
  private int[] attributionMargins;
  private int attributionTintColor = UNDEFINED_COLOR;

  // Zoom control
  private boolean zoomControlsEnabled;

  public void update(MapboxMap mapboxMap) {
    UiSettings uiSettings = mapboxMap.getUiSettings();

    restoreCameraPosition(mapboxMap);
    restoreGestureSettings(uiSettings);
    restoreCompass(uiSettings);
    restoreLogo(uiSettings);
    restoreAttribution(uiSettings);
    restoreZoomControl(uiSettings);
    restoreStyle(mapboxMap);
  }

  private void restoreCameraPosition(MapboxMap mapboxMap) {
    if (cameraPosition != null) {
      mapboxMap.moveCamera(CameraUpdateFactory.newCameraPosition(
        new CameraPosition.Builder(cameraPosition).build())
      );
    }
  }

  private void restoreGestureSettings(UiSettings uiSettings) {
    uiSettings.setZoomGesturesEnabled(zoomGesturesEnabled);
    uiSettings.setZoomGestureChangeAllowed(zoomGesturesChangeAllowed);
    uiSettings.setScrollGesturesEnabled(scrollGesturesEnabled);
    uiSettings.setScrollGestureChangeAllowed(scrollGesturesChangeAllowed);
    uiSettings.setRotateGesturesEnabled(rotateGesturesEnabled);
    uiSettings.setRotateGestureChangeAllowed(rotateGesturesChangeAllowed);
    uiSettings.setTiltGesturesEnabled(tiltGesturesEnabled);
    uiSettings.setTiltGestureChangeAllowed(tiltGesturesChangeAllowed);
    uiSettings.setDoubleTapGesturesEnabled(doubleTapGesturesEnabled);
    uiSettings.setDoubleTapGestureChangeAllowed(doubleTapGesturesChangeAllowed);
  }

  private void restoreCompass(UiSettings uiSettings) {
    uiSettings.setCompassEnabled(compassEnabled);
    uiSettings.setCompassGravity(compassGravity);
    uiSettings.setCompassMargins(compassMargins[0], compassMargins[1], compassMargins[2], compassMargins[3]);
    uiSettings.setCompassFadeFacingNorth(compassFadingNorth);
    uiSettings.setCompassImage(compassImage);
  }

  private void restoreLogo(UiSettings uiSettings) {
    uiSettings.setLogoEnabled(logoEnabled);
    uiSettings.setLogoGravity(logoGravity);
    uiSettings.setLogoMargins(logoMargins[0], logoMargins[1], logoMargins[2], logoMargins[3]);
  }

  private void restoreAttribution(UiSettings uiSettings) {
    uiSettings.setAttributionEnabled(attributionEnabled);
    uiSettings.setAttributionGravity(attributionGravity);
    uiSettings.setAttributionMargins(attributionMargins[0], attributionMargins[1], attributionMargins[2], attributionMargins[3]);
    uiSettings.setAttributionTintColor(attributionTintColor);
  }

  private void restoreZoomControl(UiSettings uiSettings) {
    uiSettings.setZoomControlsEnabled(zoomControlsEnabled);
  }

  private void restoreStyle(MapboxMap mapboxMap) {
    if (styleUrl != null && !mapboxMap.getStyleUrl().equals(styleUrl)) {
      mapboxMap.setStyle(styleUrl);
    }
  }

  // Getters Setters

  public String getStyleUrl() {
    return styleUrl;
  }

  public void setStyleUrl(String styleUrl) {
    this.styleUrl = styleUrl;
  }

  public CameraPosition getCameraPosition() {
    return cameraPosition;
  }

  public void setCameraPosition(CameraPosition cameraPosition) {
    this.cameraPosition = cameraPosition;
  }

  public boolean isNativeDebug() {
    return nativeDebug;
  }

  public void setNativeDebug(boolean nativeDebug) {
    this.nativeDebug = nativeDebug;
  }

  public boolean isZoomGesturesEnabled() {
    return zoomGesturesEnabled;
  }

  public void setZoomGesturesEnabled(boolean zoomGesturesEnabled) {
    this.zoomGesturesEnabled = zoomGesturesEnabled;
  }

  public boolean isZoomGesturesChangeAllowed() {
    return zoomGesturesChangeAllowed;
  }

  public void setZoomGesturesChangeAllowed(boolean zoomGesturesChangeAllowed) {
    this.zoomGesturesChangeAllowed = zoomGesturesChangeAllowed;
  }

  public boolean isScrollGesturesEnabled() {
    return scrollGesturesEnabled;
  }

  public void setScrollGesturesEnabled(boolean scrollGesturesEnabled) {
    this.scrollGesturesEnabled = scrollGesturesEnabled;
  }

  public boolean isScrollGesturesChangeAllowed() {
    return scrollGesturesChangeAllowed;
  }

  public void setScrollGesturesChangeAllowed(boolean scrollGesturesChangeAllowed) {
    this.scrollGesturesChangeAllowed = scrollGesturesChangeAllowed;
  }

  public boolean isRotateGesturesEnabled() {
    return rotateGesturesEnabled;
  }

  public void setRotateGesturesEnabled(boolean rotateGesturesEnabled) {
    this.rotateGesturesEnabled = rotateGesturesEnabled;
  }

  public boolean isRotateGesturesChangeAllowed() {
    return rotateGesturesChangeAllowed;
  }

  public void setRotateGesturesChangeAllowed(boolean rotateGesturesChangeAllowed) {
    this.rotateGesturesChangeAllowed = rotateGesturesChangeAllowed;
  }

  public boolean isTiltGesturesEnabled() {
    return tiltGesturesEnabled;
  }

  public void setTiltGesturesEnabled(boolean tiltGesturesEnabled) {
    this.tiltGesturesEnabled = tiltGesturesEnabled;
  }

  public boolean isTiltGesturesChangeAllowed() {
    return tiltGesturesChangeAllowed;
  }

  public void setTiltGesturesChangeAllowed(boolean tiltGesturesChangeAllowed) {
    this.tiltGesturesChangeAllowed = tiltGesturesChangeAllowed;
  }

  public boolean isDoubleTapGesturesEnabled() {
    return doubleTapGesturesEnabled;
  }

  public void setDoubleTapGesturesEnabled(boolean doubleTapGesturesEnabled) {
    this.doubleTapGesturesEnabled = doubleTapGesturesEnabled;
  }

  public boolean isDoubleTapGesturesChangeAllowed() {
    return doubleTapGesturesChangeAllowed;
  }

  public void setDoubleTapGesturesChangeAllowed(boolean doubleTapGesturesChangeAllowed) {
    this.doubleTapGesturesChangeAllowed = doubleTapGesturesChangeAllowed;
  }

  public boolean isCompassEnabled() {
    return compassEnabled;
  }

  public void setCompassEnabled(boolean compassEnabled) {
    this.compassEnabled = compassEnabled;
  }

  public boolean isCompassFadingNorth() {
    return compassFadingNorth;
  }

  public void setCompassFadingNorth(boolean compassFadingNorth) {
    this.compassFadingNorth = compassFadingNorth;
  }

  public int getCompassGravity() {
    return compassGravity;
  }

  public void setCompassGravity(int compassGravity) {
    this.compassGravity = compassGravity;
  }

  public int[] getCompassMargins() {
    return compassMargins;
  }

  public void setCompassMargins(int[] compassMargins) {
    this.compassMargins = compassMargins;
  }

  public Drawable getCompassImage() {
    return compassImage;
  }

  public void setCompassImage(Drawable compassImage) {
    this.compassImage = compassImage;
  }

  public boolean isLogoEnabled() {
    return logoEnabled;
  }

  public void setLogoEnabled(boolean logoEnabled) {
    this.logoEnabled = logoEnabled;
  }

  public int getLogoGravity() {
    return logoGravity;
  }

  public void setLogoGravity(int logoGravity) {
    this.logoGravity = logoGravity;
  }

  public int[] getLogoMargins() {
    return logoMargins;
  }

  public void setLogoMargins(int[] logoMargins) {
    this.logoMargins = logoMargins;
  }

  public boolean isAttributionEnabled() {
    return attributionEnabled;
  }

  public void setAttributionEnabled(boolean attributionEnabled) {
    this.attributionEnabled = attributionEnabled;
  }

  public int getAttributionGravity() {
    return attributionGravity;
  }

  public void setAttributionGravity(int attributionGravity) {
    this.attributionGravity = attributionGravity;
  }

  public int[] getAttributionMargins() {
    return attributionMargins;
  }

  public void setAttributionMargins(int[] attributionMargins) {
    this.attributionMargins = attributionMargins;
  }

  public int getAttributionTintColor() {
    return attributionTintColor;
  }

  public void setAttributionTintColor(int attributionTintColor) {
    this.attributionTintColor = attributionTintColor;
  }

  public boolean isZoomControlsEnabled() {
    return zoomControlsEnabled;
  }

  public void setZoomControlsEnabled(boolean zoomControlsEnabled) {
    this.zoomControlsEnabled = zoomControlsEnabled;
  }
}
