package com.mapbox.mapboxsdk.style.sources;

import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.UiThread;
import android.support.annotation.WorkerThread;

import com.mapbox.geojson.Feature;
import com.mapbox.geojson.FeatureCollection;
import com.mapbox.mapboxsdk.geometry.LatLngBounds;
import com.mapbox.mapboxsdk.style.layers.Filter;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Custom Vector Source, allows using FeatureCollections.
 *
 */
@UiThread
public class CustomGeometrySource extends Source {
  private ExecutorService executor;
  private GeometryTileProvider provider;
  private final Map<TileID, AtomicBoolean> cancelledTileRequests = new ConcurrentHashMap<>();

  /**
   * Create a CustomGeometrySource
   *
   * @param id The source id.
   * @param provider The tile provider that returns geometry data for this source.
   */
  public CustomGeometrySource(String id, GeometryTileProvider provider) {
    this(id, provider, new CustomGeometrySourceOptions());
  }

  /**
   * Create a CustomGeometrySource with non-default CustomGeometrySourceOptions.
   * <p>Supported options are minZoom, maxZoom, buffer, and tolerance.</p>
   *
   * @param id      The source id.
   * @param provider The tile provider that returns geometry data for this source.
   * @param options CustomGeometrySourceOptions.
   */
  public CustomGeometrySource(String id, GeometryTileProvider provider, CustomGeometrySourceOptions options) {
    this.provider = provider;
    executor = Executors.newFixedThreadPool(4);
    initialize(id, options);
  }

  /**
   *  Invalidate previously provided features within a given bounds at all zoom levels.
   *  Invoking this method will result in new requests to `GeometryTileProvider` for regions
   *  that contain, include, or intersect with the provided bounds.
   *
   * @param bounds The region in which features should be invalidated at all zoom levels
   */
  public void invalidateRegion(LatLngBounds bounds) {
    nativeInvalidateBounds(bounds);
  }

  /**
   * Invalidate the geometry contents of a specific tile. Invoking this method will result
   * in new requests to `GeometryTileProvider` for visible tiles.
   *
   * @param zoomLevel Tile zoom level.
   * @param x Tile X coordinate.
   * @param y Tile Y coordinate.
   */
  public void invalidateTile(int zoomLevel, int x, int y) {
    nativeInvalidateTile(zoomLevel, x, y);
  }

  /**
   * Set or update geometry contents of a specific tile. Use this method to update tiles
   * for which `GeometryTileProvider` was previously invoked. This method can be called from
   * background threads.
   *
   * @param zoomLevel Tile zoom level.
   * @param x Tile X coordinate.
   * @param y Tile Y coordinate.
   * @param data Feature collection for the tile.
   */
  public void setTileData(int zoomLevel, int x, int y, FeatureCollection data) {
    nativeSetTileData(zoomLevel, x, y, data);
  }

  /**
   * Queries the source for features.
   *
   * @param filter an optional filter statement to filter the returned Features
   * @return the features
   */
  @NonNull
  public List<Feature> querySourceFeatures(@Nullable Filter.Statement filter) {
    Feature[] features = querySourceFeatures(filter != null ? filter.toArray() : null);
    return features != null ? Arrays.asList(features) : new ArrayList<Feature>();
  }

  protected native void initialize(String sourceId, Object options);

  private native Feature[] querySourceFeatures(Object[] filter);

  private native void nativeSetTileData(int z, int x, int y, FeatureCollection data);

  private native void nativeInvalidateTile(int z, int x, int y);

  private native void nativeInvalidateBounds(LatLngBounds bounds);

  @Override
  protected native void finalize() throws Throwable;

  private void setTileData(TileID tileId, FeatureCollection data) {
    cancelledTileRequests.remove(tileId);
    nativeSetTileData(tileId.z, tileId.x, tileId.y, data);
  }

  @WorkerThread
  private void fetchTile(int z, int x, int y) {
    AtomicBoolean cancelFlag = new AtomicBoolean(false);
    TileID tileID = new TileID(z, x, y);
    cancelledTileRequests.put(tileID, cancelFlag);
    GeometryTileRequest request = new GeometryTileRequest(tileID, provider, this, cancelFlag);
    executor.execute(request);
  }

  @WorkerThread
  private void cancelTile(int z, int x, int y) {
    AtomicBoolean cancelFlag = cancelledTileRequests.get(new TileID(z, x, y));
    if (cancelFlag != null) {
      cancelFlag.compareAndSet(false, true);
    }
  }

  private static class TileID {
    public int z;
    public int x;
    public int y;

    public TileID(int _z, int _x, int _y) {
      z = _z;
      x = _x;
      y = _y;
    }

    public int hashCode() {
      return Arrays.hashCode(new int[]{z, x, y});
    }

    public boolean equals(Object object) {
      if (object == this) {
        return true;
      }

      if (object == null || getClass() != object.getClass()) {
        return false;
      }

      if (object instanceof TileID) {
        TileID other = (TileID)object;
        return this.z == other.z && this.x == other.x && this.y == other.y;
      }
      return false;
    }
  }

  private static class GeometryTileRequest implements Runnable {
    private TileID id;
    private GeometryTileProvider provider;
    private WeakReference<CustomGeometrySource> sourceRef;
    private AtomicBoolean cancelled;

    public GeometryTileRequest(TileID _id, GeometryTileProvider p,
                               CustomGeometrySource _source, AtomicBoolean _cancelled) {
      id = _id;
      provider = p;
      sourceRef = new WeakReference<>(_source);
      cancelled = _cancelled;
    }

    public void run() {
      if (isCancelled()) {
        return;
      }

      FeatureCollection data = provider.getFeaturesForBounds(LatLngBounds.from(id.z, id.x, id.y), id.z);
      CustomGeometrySource source = sourceRef.get();
      if (!isCancelled() && source != null && data != null)  {
        source.setTileData(id, data);
      }
    }

    private Boolean isCancelled() {
      return cancelled.get();
    }
  }
}
