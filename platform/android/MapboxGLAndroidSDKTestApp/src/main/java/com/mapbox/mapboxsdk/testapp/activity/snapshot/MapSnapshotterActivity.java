package com.mapbox.mapboxsdk.testapp.activity.snapshot;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.ViewTreeObserver;
import android.widget.GridLayout;
import android.widget.ImageView;

import com.mapbox.mapboxsdk.camera.CameraPosition;
import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.snapshotter.MapSnapshotter;
import com.mapbox.mapboxsdk.testapp.R;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import timber.log.Timber;

/**
 * Test activity showing how to use a the {@link com.mapbox.mapboxsdk.snapshotter.MapSnapshotter}
 */
public class MapSnapshotterActivity extends AppCompatActivity {

  private GridLayout grid;
  private List<MapSnapshotter> snapshotters = new ArrayList<>();


  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_snapshotter);

    // Find the grid view and start snapshotting as soon
    // as the view is measured
    grid = (GridLayout) findViewById(R.id.snapshot_grid);
    grid.getViewTreeObserver()
      .addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {
        @Override
        public void onGlobalLayout() {
          //noinspection deprecation
          grid.getViewTreeObserver().removeGlobalOnLayoutListener(this);
          addSnapshots();
        }
      });
  }

  private void addSnapshots() {
    Timber.i("Creating snapshotters");

    for (int row = 0; row < grid.getRowCount(); row++) {
      for (int column = 0; column < grid.getColumnCount(); column++) {
        startSnapShot(row, column);
      }
    }
  }

  private void startSnapShot(final int row, final int column) {

    // Define the dimensions
    MapSnapshotter.Options options = new MapSnapshotter.Options(
      grid.getMeasuredWidth() / grid.getColumnCount(),
      grid.getMeasuredHeight() / grid.getRowCount()
    )
      // Optionally the pixel ratio
      .withPixelRatio(1)

      // Optionally the style
      .withStyle((column + row) % 2 == 0 ? Style.MAPBOX_STREETS : Style.DARK);

    // Optionally the visible region
    if (row % 2 == 0) {
      options.withRegion(new LatLngBounds.Builder()
        .include(new LatLng(randomInRange(-80, 80), randomInRange(-160, 160)))
        .include(new LatLng(randomInRange(-80, 80), randomInRange(-160, 160)))
        .build()
      );
    }

    // Optionally the camera options
    if (column % 2 == 0) {
      options.withCameraPosition(new CameraPosition.Builder()
        .target(options.getRegion() != null
          ? options.getRegion().getCenter()
          : new LatLng(randomInRange(-80, 80), randomInRange(-160, 160)))
        .bearing(randomInRange(0, 360))
        .tilt(randomInRange(0, 60))
        .zoom(randomInRange(0, 20))
        .build()
      );
    }

    MapSnapshotter snapshotter = new MapSnapshotter(MapSnapshotterActivity.this, options);

    snapshotter.start(snapshot -> {
      Timber.i("Got the snapshot");
      ImageView imageView = new ImageView(MapSnapshotterActivity.this);
      imageView.setImageBitmap(snapshot.getBitmap());
      grid.addView(
        imageView,
        new GridLayout.LayoutParams(GridLayout.spec(row), GridLayout.spec(column))
      );
    });
    snapshotters.add(snapshotter);
  }

  @Override
  public void onPause() {
    super.onPause();

    // Make sure to stop the snapshotters on pause
    for (MapSnapshotter snapshotter : snapshotters) {
      snapshotter.cancel();
    }
    snapshotters.clear();
  }

  private static Random random = new Random();

  public static float randomInRange(float min, float max) {
    return (random.nextFloat() * (max - min)) + min;
  }

}
