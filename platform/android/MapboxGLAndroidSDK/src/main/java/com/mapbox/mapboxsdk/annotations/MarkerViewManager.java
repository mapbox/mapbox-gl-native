package com.mapbox.mapboxsdk.annotations;

import android.content.Context;
import android.graphics.PointF;
import android.graphics.RectF;
import android.os.SystemClock;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.util.LongSparseArray;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;
import android.widget.ImageView;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.constants.MapboxConstants;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.utils.AnimatorUtils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

/**
 * Interface for interacting with ViewMarkers objects inside of a MapView.
 * <p>
 * This class is responsible for managing a {@link MarkerView} item.
 * </p>
 * @deprecated Use a {@link com.mapbox.mapboxsdk.style.layers.SymbolLayer} instead. An example of converting Android
 * SDK views to be used as a symbol see https://github.com/mapbox/mapbox-gl-native/blob/68f32bc104422207c64da8d90e8411b138d87f04/platform/android/MapboxGLAndroidSDKTestApp/src/main/java/com/mapbox/mapboxsdk/testapp/activity/style/SymbolGeneratorActivity.java
 */
@Deprecated
public class MarkerViewManager implements MapView.OnMapChangedListener {

  private final ViewGroup markerViewContainer;
  private final ViewTreeObserver.OnPreDrawListener markerViewPreDrawObserver =
    new ViewTreeObserver.OnPreDrawListener() {
      @Override
      public boolean onPreDraw() {
        invalidateViewMarkersInVisibleRegion();
        markerViewContainer.getViewTreeObserver().removeOnPreDrawListener(markerViewPreDrawObserver);
        return false;
      }
    };
  private final Map<MarkerView, View> markerViewMap = new HashMap<>();
  private final LongSparseArray<OnMarkerViewAddedListener> markerViewAddedListenerMap = new LongSparseArray<>();
  private final List<MapboxMap.MarkerViewAdapter> markerViewAdapters = new ArrayList<>();

  // TODO refactor MapboxMap out for Projection and Transform
  // Requires removing MapboxMap from Annotations by using Peer model from #6912
  private MapboxMap mapboxMap;

  private boolean enabled;
  private long updateTime;
  private MapboxMap.OnMarkerViewClickListener onMarkerViewClickListener;
  private boolean isWaitingForRenderInvoke;

  /**
   * Creates an instance of MarkerViewManager.
   *
   * @param container the ViewGroup associated with the MarkerViewManager
   */
  public MarkerViewManager(@NonNull ViewGroup container) {
    this.markerViewContainer = container;
    this.markerViewAdapters.add(new ImageMarkerViewAdapter(container.getContext()));
  }

  // TODO refactor MapboxMap out for Projection and Transform
  // Requires removing MapboxMap from Annotations by using Peer model from #6912
  public void bind(MapboxMap mapboxMap) {
    this.mapboxMap = mapboxMap;
  }

  @Override
  public void onMapChanged(@MapView.MapChange int change) {
    if (isWaitingForRenderInvoke && change == MapView.DID_FINISH_RENDERING_FRAME_FULLY_RENDERED) {
      isWaitingForRenderInvoke = false;
      invalidateViewMarkersInVisibleRegion();
    }
  }

  /**
   * Called to enable or disable MarkerView management.
   *
   * @param enabled true if management should be enabled
   */
  public void setEnabled(boolean enabled) {
    this.enabled = enabled;
  }

  /**
   * Called with true to wait for the next render invocation.
   *
   * @param waitingForRenderInvoke true if waiting for next render event
   */
  public void setWaitingForRenderInvoke(boolean waitingForRenderInvoke) {
    isWaitingForRenderInvoke = waitingForRenderInvoke;
  }

  /**
   * Animate a MarkerView to a given rotation.
   * <p>
   * The {@link MarkerView} will be rotated from its current rotation to the given rotation.
   * </p>
   *
   * @param marker   the MarkerView to rotate.
   * @param rotation the rotation value.
   */
  public void animateRotation(@NonNull MarkerView marker, float rotation) {
    View convertView = markerViewMap.get(marker);
    if (convertView != null) {
      AnimatorUtils.rotate(convertView, rotation);
    }
  }

  /**
   * Animate a MarkerView with a given rotation.
   *
   * @param marker   the MarkerView to rotate by.
   * @param rotation the rotation by value, limited to 0 - 360 degrees.
   */
  public void animateRotationBy(@NonNull MarkerView marker, float rotation) {
    View convertView = markerViewMap.get(marker);
    if (convertView != null) {
      convertView.animate().cancel();
      // calculate new direction
      float diff = rotation - convertView.getRotation();
      if (diff > 180.0f) {
        diff -= 360.0f;
      } else if (diff < -180.0f) {
        diff += 360.f;
      }
      AnimatorUtils.rotateBy(convertView, diff);
    }
  }

  /**
   * Set the rotation of a MarkerView to a given rotation value.
   *
   * @param marker   The MarkerView to change its rotation value
   * @param rotation The rotation value
   */
  public void setRotation(@NonNull MarkerView marker, float rotation) {
    View convertView = markerViewMap.get(marker);
    if (convertView != null) {
      convertView.animate().cancel();
      convertView.setRotation(rotation);
    }
  }

  /**
   * Animate a MarkerView to a given alpha value.
   * <p>
   * The {@link MarkerView} will be transformed from its current alpha value to the given value.
   * </p>
   *
   * @param marker the MarkerView to change its alpha value.
   * @param alpha  the alpha value.
   */
  public void animateAlpha(@NonNull MarkerView marker, float alpha) {
    View convertView = markerViewMap.get(marker);
    if (convertView != null) {
      AnimatorUtils.alpha(convertView, alpha);
    }
  }

  /**
   * Animate a MarkerVIew to be visible or invisible
   * <p>
   * The {@link MarkerView} will be made {@link View#VISIBLE} or {@link View#GONE}.
   * </p>
   *
   * @param marker  the MarkerView to change its visibility
   * @param visible the flag indicating if MarkerView is visible
   */
  public void animateVisible(@NonNull MarkerView marker, boolean visible) {
    View convertView = markerViewMap.get(marker);
    if (convertView != null) {
      convertView.setVisibility(visible ? View.VISIBLE : View.GONE);
    }
  }

  /**
   * Updates the position of MarkerViews currently found in the viewport.
   * <p>
   * The collection of {@link MarkerView} will be iterated and each item position will be updated.
   * If an item is View state is not visible and its related flag is set to visible, the
   * {@link MarkerView} will be animated to visible using alpha animation.
   * </p>
   */
  public void updateMarkerViewsPosition() {
    for (final MarkerView marker : markerViewMap.keySet()) {
      final View convertView = markerViewMap.get(marker);
      if (convertView != null) {
        PointF point = mapboxMap.getProjection().toScreenLocation(marker.getPosition());
        if (marker.getOffsetX() == MapboxConstants.UNMEASURED) {
          // ensure view is measured first
          // #6805 invalidate marker views to ensure convertView width and height
          // values are properly measured and up to date
          if (marker.getWidth() == 0 && marker.isVisible()) {
            convertView.getViewTreeObserver().addOnPreDrawListener(markerViewPreDrawObserver);
          }
        }

        marker.setWidth(convertView.getWidth());
        marker.setHeight(convertView.getHeight());

        if (marker.getWidth() != 0) {
          int x = (int) (marker.getAnchorU() * marker.getWidth());
          int y = (int) (marker.getAnchorV() * marker.getHeight());
          marker.setOffset(x, y);
        }

        convertView.setX(point.x - marker.getOffsetX());
        convertView.setY(point.y - marker.getOffsetY());

        // animate visibility
        if (marker.isVisible() && convertView.getVisibility() == View.GONE) {
          animateVisible(marker, true);
        }
      }
    }
  }

  /**
   * Set tilt on every non flat MarkerView currently shown in the Viewport.
   *
   * @param tilt the tilt value.
   */
  public void setTilt(float tilt) {
    View convertView;
    for (MarkerView markerView : markerViewMap.keySet()) {
      if (markerView.isFlat()) {
        convertView = markerViewMap.get(markerView);
        if (convertView != null) {
          markerView.setTilt(tilt);
          convertView.setRotationX(tilt);
        }
      }
    }
  }

  /**
   * Update and invalidate the MarkerView icon.
   *
   * @param markerView the marker view to updates.
   */
  public void updateIcon(@NonNull MarkerView markerView) {
    View convertView = markerViewMap.get(markerView);
    if (convertView != null && convertView instanceof ImageView) {
      ((ImageView) convertView).setImageBitmap(markerView.getIcon().getBitmap());
      markerView.invalidate();
    }
  }

  /**
   * Animate a MarkerView to a deselected state.
   * <p>
   * The {@link com.mapbox.mapboxsdk.maps.MapboxMap.MarkerViewAdapter#onDeselect(MarkerView, View)}
   * will be called to execute an animation.
   * </p>
   *
   * @param marker the MarkerView to deselect.
   */
  public void deselect(@NonNull MarkerView marker) {
    deselect(marker, true);
  }

  /**
   * Animate a MarkerView to a deselected state.
   * <p>
   * The {@link com.mapbox.mapboxsdk.maps.MapboxMap.MarkerViewAdapter#onDeselect(MarkerView, View)}
   * will be called to execute an animation.
   * </p>
   *
   * @param marker        the MarkerView to deselect.
   * @param callbackToMap indicates if deselect marker must be called on MapboxMap.
   */
  public void deselect(@NonNull MarkerView marker, boolean callbackToMap) {
    final View convertView = markerViewMap.get(marker);
    if (convertView != null) {
      for (MapboxMap.MarkerViewAdapter adapter : markerViewAdapters) {
        if (adapter.getMarkerClass().equals(marker.getClass())) {
          adapter.onDeselect(marker, convertView);
        }
      }
    }
    if (callbackToMap) {
      mapboxMap.deselectMarker(marker);
    }
    marker.setSelected(false);
  }

  /**
   * Animate a MarkerView to a selected state.
   *
   * @param marker the MarkerView object to select.
   */
  public void select(@NonNull MarkerView marker) {
    select(marker, true);
  }

  /**
   * Animate a MarkerView to a selected state.
   *
   * @param marker        the MarkerView object to select.
   * @param callbackToMap indicates if select marker must be called on {@link MapboxMap}.
   */
  public void select(@NonNull MarkerView marker, boolean callbackToMap) {
    final View convertView = markerViewMap.get(marker);
    for (MapboxMap.MarkerViewAdapter adapter : markerViewAdapters) {
      if (adapter.getMarkerClass().equals(marker.getClass())) {
        select(marker, convertView, adapter, callbackToMap);
      }
    }
  }

  /**
   * Animate a MarkerView to a selected state.
   * <p>
   * The {@link com.mapbox.mapboxsdk.maps.MapboxMap.MarkerViewAdapter#onSelect(MarkerView, View, boolean)}
   * will be called to execute an animation.
   * </p>
   *
   * @param marker      the MarkerView object to select.
   * @param convertView the View presentation of the MarkerView.
   * @param adapter     the adapter used to adapt the marker to the convertView.
   */
  public void select(@NonNull MarkerView marker, View convertView, MapboxMap.MarkerViewAdapter adapter) {
    select(marker, convertView, adapter, true);
  }


  /**
   * Animate a MarkerView to a selected state.
   * <p>
   * The {@link com.mapbox.mapboxsdk.maps.MapboxMap.MarkerViewAdapter#onSelect(MarkerView, View, boolean)}
   * will be called to execute an animation.
   * </p>
   *
   * @param marker        the MarkerView object to select.
   * @param convertView   the View presentation of the MarkerView.
   * @param adapter       the adapter used to adapt the marker to the convertView.
   * @param callbackToMap indicates if select marker must be called on MapboxMap.
   */
  public void select(@NonNull MarkerView marker, View convertView, MapboxMap.MarkerViewAdapter adapter,
                     boolean callbackToMap) {
    if (convertView != null) {
      if (adapter.onSelect(marker, convertView, false)) {
        if (callbackToMap) {
          mapboxMap.selectMarker(marker);
        }
      }
      marker.setSelected(true);
      convertView.bringToFront();
    }
  }

  /**
   * Get view representation from a MarkerView. If marker is not found in current viewport,
   * {@code null} is returned.
   *
   * @param marker the marker to get the view.
   * @return the Android SDK View object.
   */
  @Nullable
  public View getView(MarkerView marker) {
    return markerViewMap.get(marker);
  }

  /**
   * Get the view adapter for a marker.
   *
   * @param markerView the marker to get the view adapter.
   * @return the MarkerView adapter.
   */
  @Nullable
  public MapboxMap.MarkerViewAdapter getViewAdapter(MarkerView markerView) {
    MapboxMap.MarkerViewAdapter adapter = null;
    for (MapboxMap.MarkerViewAdapter a : markerViewAdapters) {
      if (a.getMarkerClass().equals(markerView.getClass())) {
        adapter = a;
      }
    }
    return adapter;
  }

  /**
   * Remove a MarkerView from a map.
   * <p>
   * The {@link MarkerView} will be removed using an alpha animation and related {@link View}
   * will be released to the android.support.v4.util.Pools.SimplePool from the related
   * {@link com.mapbox.mapboxsdk.maps.MapboxMap.MarkerViewAdapter}. It's possible to remove
   * the {@link MarkerView} from the underlying collection if needed.
   * </p>
   *
   * @param marker the MarkerView to remove.
   */
  public void removeMarkerView(MarkerView marker) {
    final View viewHolder = markerViewMap.get(marker);
    if (viewHolder != null && marker != null) {
      for (final MapboxMap.MarkerViewAdapter<?> adapter : markerViewAdapters) {
        if (adapter.getMarkerClass().equals(marker.getClass())) {
          if (adapter.prepareViewForReuse(marker, viewHolder)) {
            // reset offset for reuse
            marker.setOffset(MapboxConstants.UNMEASURED, MapboxConstants.UNMEASURED);
            adapter.releaseView(viewHolder);
          }
        }
      }
    }
    marker.setMapboxMap(null);
    markerViewMap.remove(marker);
  }

  /**
   * Add a MarkerViewAdapter to the MarkerViewManager.
   * <p>
   * The provided MarkerViewAdapter must supply a generic subclass of MarkerView.
   * </p>
   *
   * @param markerViewAdapter the MarkerViewAdapter to add.
   */
  public void addMarkerViewAdapter(MapboxMap.MarkerViewAdapter markerViewAdapter) {
    if (markerViewAdapter.getMarkerClass().equals(MarkerView.class)) {
      throw new RuntimeException("Providing a custom MarkerViewAdapter requires subclassing MarkerView");
    }

    if (!markerViewAdapters.contains(markerViewAdapter)) {
      markerViewAdapters.add(markerViewAdapter);
      invalidateViewMarkersInVisibleRegion();
    }
  }

  /**
   * Get all MarkerViewAdapters associated with this MarkerViewManager.
   *
   * @return a List of MarkerViewAdapters.
   */
  public List<MapboxMap.MarkerViewAdapter> getMarkerViewAdapters() {
    return markerViewAdapters;
  }

  /**
   * Register a callback to be invoked when this view is clicked.
   *
   * @param listener the callback to be invoked.
   */
  public void setOnMarkerViewClickListener(@Nullable MapboxMap.OnMarkerViewClickListener listener) {
    onMarkerViewClickListener = listener;
  }

  /**
   * Schedule that ViewMarkers found in the viewport are invalidated.
   * <p>
   * This method is rate limited, and {@link #invalidateViewMarkersInVisibleRegion} will only be called
   * once each 250 ms.
   * </p>
   */
  public void update() {
    if (enabled) {
      long currentTime = SystemClock.elapsedRealtime();
      if (currentTime < updateTime) {
        updateMarkerViewsPosition();
        return;
      }
      invalidateViewMarkersInVisibleRegion();
      updateTime = currentTime + 250;
    }
  }

  /**
   * Invalidate the ViewMarkers found in the viewport.
   * <p>
   * This method will remove any markers that aren't in the viewport anymore and will add new
   * ones for each found Marker in the changed viewport.
   * </p>
   */
  public void invalidateViewMarkersInVisibleRegion() {
    RectF mapViewRect = new RectF(0, 0, markerViewContainer.getWidth(), markerViewContainer.getHeight());
    List<MarkerView> markers = mapboxMap.getMarkerViewsInRect(mapViewRect);
    View convertView;

    // remove old markers
    Iterator<MarkerView> iterator = markerViewMap.keySet().iterator();
    while (iterator.hasNext()) {
      MarkerView marker = iterator.next();
      if (!markers.contains(marker)) {
        // remove marker
        convertView = markerViewMap.get(marker);
        for (MapboxMap.MarkerViewAdapter adapter : markerViewAdapters) {
          if (adapter.getMarkerClass().equals(marker.getClass())) {
            adapter.prepareViewForReuse(marker, convertView);
            adapter.releaseView(convertView);
            iterator.remove();
          }
        }
      }
    }

    // introduce new markers
    for (final MarkerView marker : markers) {
      if (!markerViewMap.containsKey(marker)) {
        for (final MapboxMap.MarkerViewAdapter adapter : markerViewAdapters) {
          if (adapter.getMarkerClass().equals(marker.getClass())) {

            // Inflate View
            convertView = (View) adapter.getViewReusePool().acquire();
            final View adaptedView = adapter.getView(marker, convertView, markerViewContainer);
            if (adaptedView != null) {
              adaptedView.setRotationX(marker.getTilt());
              adaptedView.setRotation(marker.getRotation());
              adaptedView.setAlpha(marker.getAlpha());
              adaptedView.setVisibility(View.GONE);

              if (mapboxMap.getSelectedMarkers().contains(marker)) {
                // if a marker to be shown was selected
                // replay that animation with duration 0
                if (adapter.onSelect(marker, adaptedView, true)) {
                  mapboxMap.selectMarker(marker);
                }
              }

              marker.setMapboxMap(mapboxMap);
              markerViewMap.put(marker, adaptedView);
              if (convertView == null) {
                adaptedView.setVisibility(View.GONE);
                markerViewContainer.addView(adaptedView);
              }
            }

            // notify listener is marker view is rendered
            OnMarkerViewAddedListener onViewAddedListener = markerViewAddedListenerMap.get(marker.getId());
            if (onViewAddedListener != null) {
              onViewAddedListener.onViewAdded(marker);
              markerViewAddedListenerMap.remove(marker.getId());
            }
          }
        }
      }
    }

    // clear map, don't keep references to MarkerView listeners that are not found in the bounds of the map.
    markerViewAddedListenerMap.clear();

    // trigger update to make newly added ViewMarker visible,
    // these would only be updated when the map is moved.
    updateMarkerViewsPosition();
  }

  /**
   * When the provided MarkerView is clicked on by a user, we check if a custom click
   * event has been created and if not, display a InfoWindow.
   *
   * @param markerView that the click event occurred
   * @return true if the marker view click has been handled, false if not
   */
  public boolean onClickMarkerView(MarkerView markerView) {
    boolean clickHandled = false;

    MapboxMap.MarkerViewAdapter adapter = getViewAdapter(markerView);
    View view = getView(markerView);
    if (adapter == null || view == null) {
      // not a valid state
      return true;
    }

    if (onMarkerViewClickListener != null) {
      clickHandled = onMarkerViewClickListener.onMarkerClick(markerView, view, adapter);
    }

    return clickHandled;
  }

  /**
   * Handles the MarkerView info window offset.
   *
   * @param marker that we are ensuring info window offset
   */
  public void ensureInfoWindowOffset(MarkerView marker) {
    View view = null;
    if (markerViewMap.containsKey(marker)) {
      view = markerViewMap.get(marker);
    } else {
      for (final MapboxMap.MarkerViewAdapter adapter : markerViewAdapters) {
        if (adapter.getMarkerClass().equals(marker.getClass())) {
          View convertView = (View) adapter.getViewReusePool().acquire();
          view = adapter.getView(marker, convertView, markerViewContainer);
          break;
        }
      }
    }

    if (view != null) {
      if (marker.getWidth() == 0) {
        if (view.getMeasuredWidth() == 0) {
          // Ensure the marker's view is measured first
          view.measure(View.MeasureSpec.UNSPECIFIED, View.MeasureSpec.UNSPECIFIED);
        }
        marker.setWidth(view.getMeasuredWidth());
        marker.setHeight(view.getMeasuredHeight());
      }

      // update position on map
      if (marker.getOffsetX() == MapboxConstants.UNMEASURED) {
        int x = (int) (marker.getAnchorU() * marker.getWidth());
        int y = (int) (marker.getAnchorV() * marker.getHeight());
        marker.setOffset(x, y);
      }

      // InfoWindow offset
      int infoWindowOffsetX = (int) ((view.getMeasuredWidth() * marker.getInfoWindowAnchorU()) - marker.getOffsetX());
      int infoWindowOffsetY = (int) ((view.getMeasuredHeight() * marker.getInfoWindowAnchorV()) - marker.getOffsetY());
      marker.setTopOffsetPixels(infoWindowOffsetY);
      marker.setRightOffsetPixels(infoWindowOffsetX);
    }
  }

  public ViewGroup getMarkerViewContainer() {
    return markerViewContainer;
  }

  public void addOnMarkerViewAddedListener(MarkerView markerView, OnMarkerViewAddedListener onMarkerViewAddedListener) {
    markerViewAddedListenerMap.put(markerView.getId(), onMarkerViewAddedListener);
  }

  /**
   * Default MarkerViewAdapter used for base class of MarkerView to adapt a MarkerView to
   * an ImageView.
   */
  private static class ImageMarkerViewAdapter extends MapboxMap.MarkerViewAdapter<MarkerView> {

    private LayoutInflater inflater;

    ImageMarkerViewAdapter(Context context) {
      super(context);
      inflater = LayoutInflater.from(context);
    }

    @Nullable
    @Override
    public View getView(@NonNull MarkerView marker, @Nullable View convertView, @NonNull ViewGroup parent) {
      ViewHolder viewHolder;
      if (convertView == null) {
        viewHolder = new ViewHolder();
        convertView = inflater.inflate(R.layout.mapbox_view_image_marker, parent, false);
        viewHolder.imageView = (ImageView) convertView.findViewById(R.id.image);
        convertView.setTag(viewHolder);
      } else {
        viewHolder = (ViewHolder) convertView.getTag();
      }
      viewHolder.imageView.setImageBitmap(marker.getIcon().getBitmap());
      viewHolder.imageView.setContentDescription(marker.getTitle());
      return convertView;
    }

    private static class ViewHolder {
      ImageView imageView;
    }
  }

  /**
   * Interface definition invoked when the View of a MarkerView has been added to the map.
   * <p>
   * {@link MapboxMap#addMarker(BaseMarkerOptions)}
   * and only when the related MarkerView is found in the viewport of the map.
   * </p>
   */
  public interface OnMarkerViewAddedListener {

    /**
     * Invoked when the View of a MarkerView has been added to the Map.
     *
     * @param markerView The MarkerView the View was added for
     */
    void onViewAdded(@NonNull MarkerView markerView);
  }
}
