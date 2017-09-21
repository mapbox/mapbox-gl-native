package com.mapbox.mapboxsdk.testapp.activity.render;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.snapshotter.MapSnapshotter;

public class RenderTestDefinition {

  private String name;
  private int width;
  private int height;
  private double[] center;
  private double zoom;
  private double tilt;
  private double bearing;
  private String styleUrl;

  public String getName() {
    return name.replaceAll(" ", "_").toLowerCase();
  }

  public MapSnapshotter.Options toOptions() {
    return new MapSnapshotter
      .Options(width, height)
      .withStyle(styleUrl)
      .withCameraPosition(
        new CameraPosition.Builder()
          .target(new LatLng(center[0], center[1]))
          .zoom(zoom)
          .tilt(tilt)
          .bearing(bearing)
          .build()
      );
  }
}
