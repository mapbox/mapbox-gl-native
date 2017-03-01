package com.mapbox.mapboxsdk.annotations;

import android.content.res.Resources;
import android.graphics.PointF;
import android.support.annotation.LayoutRes;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.mapbox.mapboxsdk.R;
import com.mapbox.mapboxsdk.geometry.LatLng;
import com.mapbox.mapboxsdk.maps.MapView;
import com.mapbox.mapboxsdk.maps.MapboxMap;

import java.lang.ref.WeakReference;

/**
 * {@code InfoWindow} is a tooltip shown when a {@link Marker} or {@link MarkerView} is tapped. Only
 * one info window is displayed at a time. When the user clicks on a marker, the currently open info
 * window will be closed and the new info window will be displayed. If the user clicks the same
 * marker while its info window is currently open, the info window will be closed.
 * <p>
 * The info window is drawn oriented against the device's screen, centered above its associated
 * marker by default. The info window anchoring can be adjusted using
 * {@link MarkerView#setInfoWindowAnchor(float, float)} for {@link MarkerView}. The default info
 * window contains the title in bold and snippet text below the title. While either the title and
 * snippet are optional, at least one is required to open the info window.
 * </p>
 */
public class InfoWindow {

  private WeakReference<Marker> boundMarker;
  private WeakReference<MapboxMap> mapboxMap;
  protected WeakReference<View> view;

  private float markerHeightOffset;
  private float markerWidthOffset;
  private float viewWidthOffset;
  private PointF coordinates;
  private boolean isVisible;

  @LayoutRes
  private int layoutRes;

  InfoWindow(MapView mapView, int layoutResId, MapboxMap mapboxMap) {
    layoutRes = layoutResId;
    View view = LayoutInflater.from(mapView.getContext()).inflate(layoutResId, mapView, false);
    initialize(view, mapboxMap);
  }

  InfoWindow(View view, MapboxMap mapboxMap) {
    initialize(view, mapboxMap);
  }

  private void initialize(View view, MapboxMap mapboxMap) {
    this.mapboxMap = new WeakReference<>(mapboxMap);
    isVisible = false;
    this.view = new WeakReference<>(view);

    view.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        MapboxMap mapboxMap = InfoWindow.this.mapboxMap.get();
        if (mapboxMap != null) {
          MapboxMap.OnInfoWindowClickListener onInfoWindowClickListener = mapboxMap.getOnInfoWindowClickListener();
          boolean handledDefaultClick = false;
          if (onInfoWindowClickListener != null) {
            handledDefaultClick = onInfoWindowClickListener.onInfoWindowClick(getBoundMarker());
          }

          if (!handledDefaultClick) {
            // default behavior: close it when clicking on the tooltip:
            closeInfoWindow();
          }
        }
      }
    });

    view.setOnLongClickListener(new View.OnLongClickListener() {
      @Override
      public boolean onLongClick(View v) {
        MapboxMap mapboxMap = InfoWindow.this.mapboxMap.get();
        if (mapboxMap != null) {
          MapboxMap.OnInfoWindowLongClickListener listener = mapboxMap.getOnInfoWindowLongClickListener();
          if (listener != null) {
            listener.onInfoWindowLongClick(getBoundMarker());
          }
        }
        return true;
      }
    });
  }

  private void closeInfoWindow() {
    MapboxMap mapbox = mapboxMap.get();
    Marker marker = boundMarker.get();
    if (marker != null && mapbox != null) {
      mapbox.deselectMarker(marker);
    }
    close();
  }

  /**
   * Open the info window at the specified position.
   *
   * @param boundMarker The marker on which is hooked the view.
   * @param position    to place the window on the map.
   * @param offsetX     The offset of the view to the position, in pixels. This allows to offset
   *                    the view from the object position.
   * @param offsetY     The offset of the view to the position, in pixels. This allows to offset
   *                    the view from the object position.
   * @return this {@link InfoWindow}.
   */
  InfoWindow open(MapView mapView, Marker boundMarker, LatLng position, int offsetX, int offsetY) {
    setBoundMarker(boundMarker);

    MapView.LayoutParams lp = new MapView.LayoutParams(MapView.LayoutParams.WRAP_CONTENT,
      MapView.LayoutParams.WRAP_CONTENT);

    MapboxMap mapboxMap = this.mapboxMap.get();
    View view = this.view.get();
    if (view != null && mapboxMap != null) {
      view.measure(View.MeasureSpec.UNSPECIFIED, View.MeasureSpec.UNSPECIFIED);

      // Calculate y-offset for update method
      markerHeightOffset = -view.getMeasuredHeight() + offsetY;
      markerWidthOffset = -offsetX;

      // Calculate default Android x,y coordinate
      coordinates = mapboxMap.getProjection().toScreenLocation(position);
      float x = coordinates.x - (view.getMeasuredWidth() / 2) + offsetX;
      float y = coordinates.y - view.getMeasuredHeight() + offsetY;

      if (view instanceof BubbleLayout) {
        // only apply repositioning/margin for InfoWindowView
        Resources resources = mapView.getContext().getResources();

        // get right/left popup window
        float rightSideInfowWindow = x + view.getMeasuredWidth();
        float leftSideInfoWindow = x;

        // get right/left map view
        final float mapRight = mapView.getRight();
        final float mapLeft = mapView.getLeft();

        float marginHorizontal = resources.getDimension(R.dimen.mapbox_infowindow_margin);
        float tipViewOffset = resources.getDimension(R.dimen.mapbox_infowindow_tipview_width) / 2;
        float tipViewMarginLeft = view.getMeasuredWidth() / 2 - tipViewOffset;

        boolean outOfBoundsLeft = false;
        boolean outOfBoundsRight = false;

        // only optimise margins if view is inside current viewport
        if (coordinates.x >= 0 && coordinates.x <= mapView.getWidth()
          && coordinates.y >= 0 && coordinates.y <= mapView.getHeight()) {

          // if out of bounds right
          if (rightSideInfowWindow > mapRight) {
            outOfBoundsRight = true;
            x -= rightSideInfowWindow - mapRight;
            tipViewMarginLeft += rightSideInfowWindow - mapRight + tipViewOffset;
            rightSideInfowWindow = x + view.getMeasuredWidth();
          }

          // fit screen left
          if (leftSideInfoWindow < mapLeft) {
            outOfBoundsLeft = true;
            x += mapLeft - leftSideInfoWindow;
            tipViewMarginLeft -= mapLeft - leftSideInfoWindow + tipViewOffset;
            leftSideInfoWindow = x;
          }

          // Add margin right
          if (outOfBoundsRight && mapRight - rightSideInfowWindow < marginHorizontal) {
            x -= marginHorizontal - (mapRight - rightSideInfowWindow);
            tipViewMarginLeft += marginHorizontal - (mapRight - rightSideInfowWindow) - tipViewOffset;
            leftSideInfoWindow = x;
          }

          // Add margin left
          if (outOfBoundsLeft && leftSideInfoWindow - mapLeft < marginHorizontal) {
            x += marginHorizontal - (leftSideInfoWindow - mapLeft);
            tipViewMarginLeft -= (marginHorizontal - (leftSideInfoWindow - mapLeft)) - tipViewOffset;
          }
        }

        // Adjust tipView
        ((BubbleLayout) view).setArrowPosition(tipViewMarginLeft);
      }

      // set anchor popupwindowview
      view.setX(x);
      view.setY(y);

      // Calculate x-offset for update method
      viewWidthOffset = x - coordinates.x - offsetX;

      close(); // if it was already opened
      mapView.addView(view, lp);
      isVisible = true;
    }
    return this;
  }

  /**
   * Close this {@link InfoWindow} if it is visible, otherwise calling this will do nothing.
   *
   * @return This {@link InfoWindow}
   */
  InfoWindow close() {
    MapboxMap mapboxMap = this.mapboxMap.get();
    if (isVisible && mapboxMap != null) {
      isVisible = false;
      View view = this.view.get();
      if (view != null && view.getParent() != null) {
        ((ViewGroup) view.getParent()).removeView(view);
      }

      Marker marker = getBoundMarker();
      MapboxMap.OnInfoWindowCloseListener listener = mapboxMap.getOnInfoWindowCloseListener();
      if (listener != null) {
        listener.onInfoWindowClose(marker);
      }

      setBoundMarker(null);
    }
    return this;
  }

  /**
   * Constructs the view that is displayed when the InfoWindow opens. This retrieves data from
   * overlayItem and shows it in the tooltip.
   *
   * @param overlayItem the tapped overlay item
   */
  void adaptDefaultMarker(Marker overlayItem, MapboxMap mapboxMap, MapView mapView) {
    View view = this.view.get();
    if (view == null) {
      view = LayoutInflater.from(mapView.getContext()).inflate(layoutRes, mapView, false);
      initialize(view, mapboxMap);
    }
    this.mapboxMap = new WeakReference<>(mapboxMap);
    String title = overlayItem.getTitle();
    TextView titleTextView = ((TextView) view.findViewById(R.id.infowindow_title));
    if (!TextUtils.isEmpty(title)) {
      titleTextView.setText(title);
      titleTextView.setVisibility(View.VISIBLE);
    } else {
      titleTextView.setVisibility(View.GONE);
    }

    String snippet = overlayItem.getSnippet();
    TextView snippetTextView = ((TextView) view.findViewById(R.id.infowindow_description));
    if (!TextUtils.isEmpty(snippet)) {
      snippetTextView.setText(snippet);
      snippetTextView.setVisibility(View.VISIBLE);
    } else {
      snippetTextView.setVisibility(View.GONE);
    }
  }

  InfoWindow setBoundMarker(Marker boundMarker) {
    this.boundMarker = new WeakReference<>(boundMarker);
    return this;
  }

  Marker getBoundMarker() {
    if (boundMarker == null) {
      return null;
    }
    return boundMarker.get();
  }

  /**
   * Will result in getting this {@link InfoWindow} and updating the view being displayed.
   */
  public void update() {
    MapboxMap mapboxMap = this.mapboxMap.get();
    Marker marker = boundMarker.get();
    View view = this.view.get();
    if (mapboxMap != null && marker != null && view != null) {
      coordinates = mapboxMap.getProjection().toScreenLocation(marker.getPosition());

      if (view instanceof BubbleLayout) {
        view.setX(coordinates.x + viewWidthOffset - markerWidthOffset);
      } else {
        view.setX(coordinates.x - (view.getMeasuredWidth() / 2) - markerWidthOffset);
      }
      view.setY(coordinates.y + markerHeightOffset);
    }
  }

  /**
   * Retrieve this {@link InfoWindow}'s current view being used.
   *
   * @return This {@link InfoWindow}'s current View.
   */
  public View getView() {
    return view != null ? view.get() : null;
  }

  boolean isVisible() {
    return isVisible;
  }

}
