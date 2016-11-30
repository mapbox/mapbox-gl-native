package com.mapbox.mapboxsdk.maps;

import android.graphics.RectF;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;

import com.mapbox.mapboxsdk.annotations.BaseMarkerOptions;
import com.mapbox.mapboxsdk.annotations.BaseMarkerViewOptions;
import com.mapbox.mapboxsdk.annotations.Icon;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerView;
import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.annotations.Polygon;
import com.mapbox.mapboxsdk.annotations.PolygonOptions;
import com.mapbox.mapboxsdk.annotations.Polyline;
import com.mapbox.mapboxsdk.annotations.PolylineOptions;

import java.util.ArrayList;
import java.util.List;

/**
 * Responsible for managing and tracking state of Annotations linked to Map. All events related to
 * annotations that occur on {@link MapboxMap} are forwarded to this class.
 * <p>
 * Responsible for referencing {@link InfoWindowManager} and {@link MarkerViewManager}.
 * </p>
 * <p>
 * Exposes convenience methods to add/remove/update all subtypes of annotations found in com.mapbox.mapboxsdk.annotations.
 * </p>
 */
class AnnotationManager implements MapView.OnMapChangedListener {

    private NativeMapView nativeMapView;
    private IconManager iconManager;
    private InfoWindowManager infoWindowManager;
    private MarkerViewManager markerViewManager;

    private List<Marker> markers;
    private List<Polyline> polylines;
    private List<Polygon> polygons;
    private List<Marker> selectedMarkers;

    private MapboxMap.OnMarkerClickListener onMarkerClickListener;
    private boolean isWaitingForRenderInvoke;

    AnnotationManager(NativeMapView view, IconManager iconManager, InfoWindowManager manager) {
        this.nativeMapView = view;
        this.iconManager = iconManager;
        this.infoWindowManager = manager;
        this.selectedMarkers = new ArrayList<>();
        this.markers = new ArrayList<>();
        this.polylines = new ArrayList<>();
        this.polygons = new ArrayList<>();

        if (view != null) {
            // null checking needed for unit tests
            view.addOnMapChangedListener(this);
        }
    }

    @Override
    public void onMapChanged(@MapView.MapChange int change) {
        if (isWaitingForRenderInvoke && change == MapView.DID_FINISH_RENDERING_FRAME_FULLY_RENDERED) {
            isWaitingForRenderInvoke = false;
            markerViewManager.invalidateViewMarkersInVisibleRegion();
        }
    }

    //
    // Annotations
    //


    void removeMarker(@NonNull Marker marker){
        marker.hideInfoWindow();
        if(marker instanceof MarkerView){
            markerViewManager.removeMarkerView((MarkerView) marker);
        }
        long id = marker.getId();
        if (id!=-1 && nativeMapView != null) {
            nativeMapView.removeAnnotation(id);
        }
        markers.remove(marker);
    }

    void removePolygon(@NonNull Polygon polygon){
        long id = polygon.getId();
        if (id!=-1 && nativeMapView != null) {
            nativeMapView.removeAnnotation(id);
        }
        polygons.remove(polygon);
    }

    void removePolyline(@NonNull Polyline polyline){
        long id = polyline.getId();
        if (id!=-1 && nativeMapView != null) {
            nativeMapView.removeAnnotation(id);
        }
        polylines.remove(polyline);
    }

    //
    // Markers
    //

    Marker addMarker(@NonNull BaseMarkerOptions markerOptions, @NonNull MapboxMap mapboxMap) {
        Marker marker = prepareMarker(markerOptions);
        long id = nativeMapView != null ? nativeMapView.addMarker(marker) : 0;
        marker.setMapboxMap(mapboxMap);
        marker.setId(id);
        markers.add(marker);
        return marker;
    }

    List<Marker> addMarkers(@NonNull List<? extends BaseMarkerOptions> markerOptionsList, @NonNull MapboxMap mapboxMap) {
        int count = markerOptionsList.size();
        List<Marker> markersList = new ArrayList<>(count);
        if (count > 0) {
            BaseMarkerOptions markerOptions;
            Marker marker;
            for (int i = 0; i < count; i++) {
                markerOptions = markerOptionsList.get(i);
                marker = prepareMarker(markerOptions);
                markersList.add(marker);
            }

            if (markersList.size() > 0) {
                long[] ids = null;
                if (nativeMapView != null) {
                    ids = nativeMapView.addMarkers(markersList);
                }

                long id = 0;
                Marker m;
                for (int i = 0; i < markersList.size(); i++) {
                    m = markersList.get(i);
                    m.setMapboxMap(mapboxMap);
                    if (ids != null) {
                        id = ids[i];
                    } else {
                        //unit test
                        id++;
                    }
                    m.setId(id);
                    markers.add(m);
                }

            }
        }
        return markersList;
    }

    private Marker prepareMarker(BaseMarkerOptions markerOptions) {
        Marker marker = markerOptions.getMarker();
        Icon icon = iconManager.loadIconForMarker(marker);
        marker.setTopOffsetPixels(iconManager.getTopOffsetPixelsForIcon(icon));
        return marker;
    }

    MarkerView addMarker(@NonNull BaseMarkerViewOptions markerOptions, @NonNull MapboxMap mapboxMap) {
        isWaitingForRenderInvoke = true;
        MarkerView marker = prepareViewMarker(markerOptions);
        marker.setMapboxMap(mapboxMap);
        long id = nativeMapView.addMarker(marker);
        marker.setId(id);
        markers.add(marker);
        return marker;
    }

    List<MarkerView> addMarkerViews(@NonNull List<? extends BaseMarkerViewOptions> markerViewOptions, @NonNull MapboxMap mapboxMap) {
        List<MarkerView> markersList = new ArrayList<>();
        for (BaseMarkerViewOptions markerViewOption : markerViewOptions) {
            // if last marker
            if (markerViewOptions.indexOf(markerViewOption) == markerViewOptions.size() - 1) {
                // get notified when render occurs to invalidate and draw MarkerViews
                isWaitingForRenderInvoke = true;
            }
            // add marker to map
            MarkerView marker = prepareViewMarker(markerViewOption);
            marker.setMapboxMap(mapboxMap);
            long id = nativeMapView.addMarker(marker);
            marker.setId(id);
            markers.add(marker);
            markersList.add(marker);
        }
        markerViewManager.invalidateViewMarkersInVisibleRegion();
        return markersList;
    }

    private MarkerView prepareViewMarker(BaseMarkerViewOptions markerViewOptions) {
        MarkerView marker = markerViewOptions.getMarker();
        iconManager.loadIconForMarkerView(marker);
        return marker;
    }

    void updateMarker(@NonNull Marker updatedMarker, @NonNull MapboxMap mapboxMap) {
        if (updatedMarker == null) {
            return;
        }

        if (updatedMarker.getId() == -1) {
            return;
        }

        if (!(updatedMarker instanceof MarkerView)) {
            iconManager.ensureIconLoaded(updatedMarker, mapboxMap);
        }

        nativeMapView.updateMarker(updatedMarker);
    }

    List<Marker> getMarkers() {
        return markers;
    }

    List<Polygon> getPolygons(){
        return polygons;
    }

    List<Polyline> getPolylines(){
        return polylines;
    }

    void setOnMarkerClickListener(@Nullable MapboxMap.OnMarkerClickListener listener) {
        onMarkerClickListener = listener;
    }

    void selectMarker(@NonNull Marker marker, @NonNull MapboxMap mapboxMap) {
        if (selectedMarkers.contains(marker)) {
            return;
        }

        // Need to deselect any currently selected annotation first
        if (!infoWindowManager.isAllowConcurrentMultipleOpenInfoWindows()) {
            deselectMarkers();
        }

        boolean handledDefaultClick = false;
        if (onMarkerClickListener != null) {
            // end developer has provided a custom click listener
            handledDefaultClick = onMarkerClickListener.onMarkerClick(marker);
        }

        if (!handledDefaultClick) {
            if (marker instanceof MarkerView) {
                markerViewManager.select((MarkerView) marker, false);
                markerViewManager.ensureInfoWindowOffset((MarkerView) marker);
            }

            if (infoWindowManager.isInfoWindowValidForMarker(marker) || infoWindowManager.getInfoWindowAdapter() != null) {
                infoWindowManager.add(marker.showInfoWindow(mapboxMap, mapboxMap.getMapView()));
            }
        }

        selectedMarkers.add(marker);
    }

    void deselectMarkers() {
        if (selectedMarkers.isEmpty()) {
            return;
        }

        for (Marker marker : selectedMarkers) {
            if (marker.isInfoWindowShown()) {
                marker.hideInfoWindow();
            }

            if (marker instanceof MarkerView) {
                markerViewManager.deselect((MarkerView) marker, false);
            }
        }

        // Removes all selected markers from the list
        selectedMarkers.clear();
    }

    void deselectMarker(@NonNull Marker marker) {
        if (!selectedMarkers.contains(marker)) {
            return;
        }

        if (marker.isInfoWindowShown()) {
            marker.hideInfoWindow();
        }

        if (marker instanceof MarkerView) {
            markerViewManager.deselect((MarkerView) marker, false);
        }

        selectedMarkers.remove(marker);
    }

    List<Marker> getSelectedMarkers() {
        return selectedMarkers;
    }

    public List<Marker> getMarkersInRect(@NonNull RectF rectangle) {
        List<Marker> markerList = new ArrayList<>();
        // convert Rectangle to be density depedent
        float pixelRatio = nativeMapView.getPixelRatio();
        RectF rect = new RectF(rectangle.left / pixelRatio,
                rectangle.top / pixelRatio,
                rectangle.right / pixelRatio,
                rectangle.bottom / pixelRatio);

        long[] ids = nativeMapView.queryPointAnnotations(rect);

        for (Marker marker : markers) {
            if(marker instanceof MarkerView){
                for (long id : ids) {
                    if(marker.getId()==id){
                        markerList.add(marker);
                    }
                }
            }
        }
        return  markerList;
    }

    public List<MarkerView> getMarkerViewsInRect(@NonNull RectF rectangle) {
        List<MarkerView> markerViews = new ArrayList<>();
        float pixelRatio = nativeMapView.getPixelRatio();
        RectF rect = new RectF(rectangle.left / pixelRatio,
                rectangle.top / pixelRatio,
                rectangle.right / pixelRatio,
                rectangle.bottom / pixelRatio);

        long[] ids = nativeMapView.queryPointAnnotations(rect);

        for (Marker marker : markers) {
            if(marker instanceof MarkerView){
                for (long id : ids) {
                    if(marker.getId()==id){
                        markerViews.add((MarkerView)marker);
                    }
                }
            }
        }
        return  markerViews;
    }

    //
    // Polygons
    //

    Polygon addPolygon(@NonNull PolygonOptions polygonOptions, @NonNull MapboxMap mapboxMap) {
        Polygon polygon = polygonOptions.getPolygon();
        if (!polygon.getPoints().isEmpty()) {
            long id = nativeMapView != null ? nativeMapView.addPolygon(polygon) : 0;
            polygon.setId(id);
            polygon.setMapboxMap(mapboxMap);
            polygons.add(polygon);
        }
        return polygon;
    }

    List<Polygon> addPolygons(@NonNull List<PolygonOptions> polygonOptionsList, @NonNull MapboxMap mapboxMap) {
        int count = polygonOptionsList.size();

        Polygon polygon;
        List<Polygon> polygonList = new ArrayList<>(count);
        if (count > 0) {
            for (PolygonOptions polygonOptions : polygonOptionsList) {
                polygon = polygonOptions.getPolygon();
                if (!polygon.getPoints().isEmpty()) {
                    polygonList.add(polygon);
                }
            }

            long[] ids = null;
            if (nativeMapView != null) {
                ids = nativeMapView.addPolygons(polygonList);
            }

            long id = 0;
            for (int i = 0; i < polygonList.size(); i++) {
                polygon = polygonList.get(i);
                polygon.setMapboxMap(mapboxMap);
                if (ids != null) {
                    id = ids[i];
                } else {
                    // unit test
                    id++;
                }
                polygon.setId(id);
                polygons.add(polygon);
            }
        }
        return polygonList;
    }

    void updatePolygon(Polygon polygon) {
        if (polygon == null) {
            return;
        }

        if (polygon.getId() == -1) {
            return;
        }

        nativeMapView.updatePolygon(polygon);
    }

    //
    // Polylines
    //

    Polyline addPolyline(@NonNull PolylineOptions polylineOptions, @NonNull MapboxMap mapboxMap) {
        Polyline polyline = polylineOptions.getPolyline();
        if (!polyline.getPoints().isEmpty()) {
            long id = nativeMapView != null ? nativeMapView.addPolyline(polyline) : 0;
            polyline.setMapboxMap(mapboxMap);
            polyline.setId(id);
            polylines.add(polyline);
        }
        return polyline;
    }

    List<Polyline> addPolylines(@NonNull List<PolylineOptions> polylineOptionsList, @NonNull MapboxMap mapboxMap) {
        int count = polylineOptionsList.size();
        Polyline polyline;
        List<Polyline> polylinesList = new ArrayList<>(count);

        if (count > 0) {
            for (PolylineOptions options : polylineOptionsList) {
                polyline = options.getPolyline();
                if (!polyline.getPoints().isEmpty()) {
                    polylinesList.add(polyline);
                }
            }

            long[] ids = null;
            if (nativeMapView != null) {
                ids = nativeMapView.addPolylines(polylinesList);
            }

            long id = 0;
            Polyline p;

            for (int i = 0; i < polylinesList.size(); i++) {
                p = polylinesList.get(i);
                p.setMapboxMap(mapboxMap);
                if (ids != null) {
                    id = ids[i];
                } else {
                    // unit test
                    id++;
                }
                p.setId(id);
                polylines.add(p);
            }
        }
        return polylinesList;
    }

    void updatePolyline(Polyline polyline) {
        if (polyline == null) {
            return;
        }

        if (polyline.getId() == -1) {
            return;
        }

        nativeMapView.updatePolyline(polyline);
    }

    //
    // MarkerViewManager
    //

    MarkerViewManager getMarkerViewManager(MapboxMap mapboxMap) {
        if (markerViewManager == null) {
            this.markerViewManager = new MarkerViewManager(mapboxMap, mapboxMap.getMapView());
        }
        return markerViewManager;
    }

    void adjustTopOffsetPixels(MapboxMap mapboxMap) {
        for (Marker marker : markers) {
            marker.setTopOffsetPixels(iconManager.getTopOffsetPixelsForIcon(marker.getIcon()));
        }

        // reopen infoWindows
        for (Marker marker : selectedMarkers) {
            if (marker.isInfoWindowShown()) {
                marker.hideInfoWindow();
                marker.showInfoWindow(mapboxMap, mapboxMap.getMapView());
            }
        }
    }

    void reloadMarkers() {
        for (Marker marker : markers) {
            nativeMapView.removeAnnotation(marker.getId());
            long newId = nativeMapView.addMarker(marker);
            marker.setId(newId);
        }
    }

    public void clear() {
        throw new RuntimeException("To be implemented");
    }
}
