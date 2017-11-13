package com.mapbox.mapboxsdk.testapp.activity.snapshot;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.PointF;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.view.ViewTreeObserver;
import android.widget.ImageView;
import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.snapshotter.MapSnapshot;
import com.mapbox.mapboxsdk.snapshotter.MapSnapshotter;
import com.mapbox.mapboxsdk.testapp.R;
import timber.log.Timber;

/**
 * Test activity showing how to use a the {@link MapSnapshotter} and overlay
 * {@link android.graphics.Bitmap}s on top.
 */
public class MapSnapshotterMarkerActivity extends AppCompatActivity implements MapSnapshotter.SnapshotReadyCallback {

  private MapSnapshotter mapSnapshotter;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_snapshotter_marker);

    final View container = findViewById(R.id.container);
    container.getViewTreeObserver()
      .addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
        @Override
        public void onGlobalLayout() {
          //noinspection deprecation
          container.getViewTreeObserver().removeGlobalOnLayoutListener(this);

          Timber.i("Starting snapshot");

          mapSnapshotter = new MapSnapshotter(
            getApplicationContext(),
            new MapSnapshotter
              .Options(Math.min(container.getMeasuredWidth(), 1024), Math.min(container.getMeasuredHeight(), 1024))
              .withStyle(Style.OUTDOORS)
              .withCameraPosition(new CameraPosition.Builder().target(new LatLng(52.090737, 5.121420)).zoom(15).build())
          );
          mapSnapshotter.start(MapSnapshotterMarkerActivity.this);
        }
      });
  }

  @Override
  protected void onStop() {
    super.onStop();
    mapSnapshotter.cancel();
  }

  @Override
  public void onSnapshotReady(MapSnapshot snapshot) {
    Timber.i("Snapshot ready");
    ImageView imageView = (ImageView) findViewById(R.id.snapshot_image);
    Bitmap image = addMarker(snapshot);
    imageView.setImageBitmap(image);
  }

  private Bitmap addMarker(MapSnapshot snapshot) {
    Canvas canvas = new Canvas(snapshot.getBitmap());
    Bitmap marker = BitmapFactory.decodeResource(getResources(), R.drawable.mapbox_marker_icon_default, null);
    // Dom toren
    PointF markerLocation = snapshot.pixelForLatLng(new LatLng(52.090649433011315, 5.121310651302338));
    canvas.drawBitmap(marker,
      markerLocation.x,
      /* Subtract height (in dp) so the bottom of the marker aligns correctly */
      markerLocation.y - (marker.getHeight() / getResources().getDisplayMetrics().density),
      null
    );
    return snapshot.getBitmap();
  }

}
