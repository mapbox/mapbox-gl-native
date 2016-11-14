package com.mapbox.mapboxsdk.maps;

import android.graphics.RectF;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.util.LongSparseArray;

import com.mapbox.mapboxsdk.annotations.Annotation;
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
class AnnotationManager {

    private NativeMapView nativeMapView;
    private IconManager iconManager;
    private InfoWindowManager infoWindowManager;
    private MarkerViewManager markerViewManager;

    private LongSparseArray<Annotation> annotations;
    private List<Marker> selectedMarkers;

    private MapboxMap.OnMarkerClickListener onMarkerClickListener;

    AnnotationManager(NativeMapView view, IconManager iconManager, InfoWindowManager manager) {
        this.nativeMapView = view;
        this.iconManager = iconManager;
        this.infoWindowManager = manager;
        this.selectedMarkers = new ArrayList<>();
        this.annotations = new LongSparseArray<>();
    }

    //
    // Annotations
    //

    Annotation getAnnotation(long id) {
        return annotations.get(id);
    }

    List<Annotation> getAnnotations() {
        List<Annotation> annotations = new ArrayList<>();
        for (int i = 0; i < this.annotations.size(); i++) {
            annotations.add(this.annotations.get(this.annotations.keyAt(i)));
        }
        return annotations;
    }

    void removeAnnotation(@NonNull Annotation annotation) {
        if (annotation instanceof Marker) {
            Marker marker = (Marker) annotation;
            marker.hideInfoWindow();
            if (marker instanceof MarkerView) {
                markerViewManager.removeMarkerView((MarkerView) marker);
            }
        }
        long id = annotation.getId();
        if (nativeMapView != null) {
            nativeMapView.removeAnnotation(id);
        }
        annotations.remove(id);
    }

    void removeAnnotation(long id) {
        if (nativeMapView != null) {
            nativeMapView.removeAnnotation(id);
        }
        annotations.remove(id);
    }

    void removeAnnotations(@NonNull List<? extends Annotation> annotationList) {
        int count = annotationList.size();
        long[] ids = new long[count];
        for (int i = 0; i < count; i++) {
            Annotation annotation = annotationList.get(i);
            if (annotation instanceof Marker) {
                Marker marker = (Marker) annotation;
                marker.hideInfoWindow();
                if (marker instanceof MarkerView) {
                    markerViewManager.removeMarkerView((MarkerView) marker);
                }
            }
            ids[i] = annotationList.get(i).getId();
        }

        if (nativeMapView != null) {
            nativeMapView.removeAnnotations(ids);
        }

        for (long id : ids) {
            annotations.remove(id);
        }
    }

    void removeAnnotations() {
        Annotation annotation;
        int count = annotations.size();
        long[] ids = new long[count];
        for (int i = 0; i < count; i++) {
            ids[i] = annotations.keyAt(i);
            annotation = annotations.get(ids[i]);
            if (annotation instanceof Marker) {
                Marker marker = (Marker) annotation;
                marker.hideInfoWindow();
                if (marker instanceof MarkerView) {
                    markerViewManager.removeMarkerView((MarkerView) marker);
                }
            }
        }

        if (nativeMapView != null) {
            nativeMapView.removeAnnotations(ids);
        }

        annotations.clear();
    }

    //
    // Markers
    //

    Marker addMarker(@NonNull BaseMarkerOptions markerOptions, @NonNull MapboxMap mapboxMap) {
        Marker marker = prepareMarker(markerOptions);
        long id = nativeMapView != null ? nativeMapView.addMarker(marker) : 0;
        marker.setMapboxMap(mapboxMap);
        marker.setId(id);
        annotations.put(id, marker);
        return marker;
    }

    List<Marker> addMarkers(@NonNull List<? extends BaseMarkerOptions> markerOptionsList, @NonNull MapboxMap mapboxMap) {
        int count = markerOptionsList.size();
        List<Marker> markers = new ArrayList<>(count);
        if (count > 0) {
            BaseMarkerOptions markerOptions;
            Marker marker;
            for (int i = 0; i < count; i++) {
                markerOptions = markerOptionsList.get(i);
                marker = prepareMarker(markerOptions);
                markers.add(marker);
            }

            if (markers.size() > 0) {
                long[] ids = null;
                if (nativeMapView != null) {
                    ids = nativeMapView.addMarkers(markers);
                }

                long id = 0;
                Marker m;
                for (int i = 0; i < markers.size(); i++) {
                    m = markers.get(i);
                    m.setMapboxMap(mapboxMap);
                    if (ids != null) {
                        id = ids[i];
                    } else {
                        //unit test
                        id++;
                    }
                    m.setId(id);
                    annotations.put(id, m);
                }

            }
        }
        return markers;
    }

    private Marker prepareMarker(BaseMarkerOptions markerOptions) {
        Marker marker = markerOptions.getMarker();
        Icon icon = iconManager.loadIconForMarker(marker);
        marker.setTopOffsetPixels(iconManager.getTopOffsetPixelsForIcon(icon));
        return marker;
    }

    MarkerView addMarker(@NonNull BaseMarkerViewOptions markerOptions, @NonNull MapboxMap mapboxMap) {
        MarkerView marker = prepareViewMarker(markerOptions);
        marker.setMapboxMap(mapboxMap);
        long id = nativeMapView.addMarker(marker);
        marker.setId(id);
        annotations.put(id, marker);
        markerViewManager.invalidateViewMarkersInVisibleRegion();
        return marker;
    }

    List<MarkerView> addMarkerViews(@NonNull List<? extends BaseMarkerViewOptions> markerViewOptions, @NonNull MapboxMap mapboxMap) {
        List<MarkerView> markers = new ArrayList<>();
        for (BaseMarkerViewOptions markerViewOption : markerViewOptions) {
            MarkerView marker = prepareViewMarker(markerViewOption);
            marker.setMapboxMap(mapboxMap);
            long id = nativeMapView.addMarker(marker);
            marker.setId(id);
            annotations.put(id, marker);
            markers.add(marker);
        }
        markerViewManager.invalidateViewMarkersInVisibleRegion();
        return markers;
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

        int index = annotations.indexOfKey(updatedMarker.getId());
        if (index > -1) {
            annotations.setValueAt(index, updatedMarker);
        }
    }

    List<Marker> getMarkers() {
        List<Marker> markers = new ArrayList<>();
        Annotation annotation;
        for (int i = 0; i < annotations.size(); i++) {
            annotation = annotations.get(annotations.keyAt(i));
            if (annotation instanceof Marker) {
                markers.add((Marker) annotation);
            }
        }
        return markers;
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
                infoWindowManager.getInfoWindows().add(marker.showInfoWindow(mapboxMap, mapboxMap.getMapView()));
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
        // convert Rectangle to be density depedent
        float pixelRatio = nativeMapView.getPixelRatio();
        RectF rect = new RectF(rectangle.left / pixelRatio,
                rectangle.top / pixelRatio,
                rectangle.right / pixelRatio,
                rectangle.bottom / pixelRatio);

        long[] ids = nativeMapView.queryPointAnnotations(rect);

        List<Long> idsList = new ArrayList<>(ids.length);
        for (long id : ids) {
            idsList.add(id);
        }

        List<Marker> annotations = new ArrayList<>(ids.length);
        List<Annotation> annotationList = getAnnotations();
        int count = annotationList.size();
        for (int i = 0; i < count; i++) {
            Annotation annotation = annotationList.get(i);
            if (annotation instanceof com.mapbox.mapboxsdk.annotations.Marker && idsList.contains(annotation.getId())) {
                annotations.add((com.mapbox.mapboxsdk.annotations.Marker) annotation);
            }
        }

        return new ArrayList<>(annotations);
    }

    public List<MarkerView> getMarkerViewsInRect(@NonNull RectF rectangle) {
        float pixelRatio = nativeMapView.getPixelRatio();
        RectF rect = new RectF(rectangle.left / pixelRatio,
                rectangle.top / pixelRatio,
                rectangle.right / pixelRatio,
                rectangle.bottom / pixelRatio);

        long[] ids = nativeMapView.queryPointAnnotations(rect);

        List<Long> idsList = new ArrayList<>(ids.length);
        for (long id : ids) {
            idsList.add(id);
        }

        List<MarkerView> annotations = new ArrayList<>(ids.length);
        List<Annotation> annotationList = getAnnotations();
        int count = annotationList.size();
        for (int i = 0; i < count; i++) {
            Annotation annotation = annotationList.get(i);
            if (annotation instanceof MarkerView && idsList.contains(annotation.getId())) {
                annotations.add((MarkerView) annotation);
            }
        }

        return new ArrayList<>(annotations);
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
            annotations.put(id, polygon);
        }
        return polygon;
    }

    List<Polygon> addPolygons(@NonNull List<PolygonOptions> polygonOptionsList, @NonNull MapboxMap mapboxMap) {
        int count = polygonOptionsList.size();

        Polygon polygon;
        List<Polygon> polygons = new ArrayList<>(count);
        if (count > 0) {
            for (PolygonOptions polygonOptions : polygonOptionsList) {
                polygon = polygonOptions.getPolygon();
                if (!polygon.getPoints().isEmpty()) {
                    polygons.add(polygon);
                }
            }

            long[] ids = null;
            if (nativeMapView != null) {
                ids = nativeMapView.addPolygons(polygons);
            }

            long id = 0;
            for (int i = 0; i < polygons.size(); i++) {
                polygon = polygons.get(i);
                polygon.setMapboxMap(mapboxMap);
                if (ids != null) {
                    id = ids[i];
                } else {
                    // unit test
                    id++;
                }
                polygon.setId(id);
                annotations.put(id, polygon);
            }
        }
        return polygons;
    }

    void updatePolygon(Polygon polygon) {
        if (polygon == null) {
            return;
        }

        if (polygon.getId() == -1) {
            return;
        }

        nativeMapView.updatePolygon(polygon);

        int index = annotations.indexOfKey(polygon.getId());
        if (index > -1) {
            annotations.setValueAt(index, polygon);
        }
    }

    List<Polygon> getPolygons() {
        List<Polygon> polygons = new ArrayList<>();
        Annotation annotation;
        for (int i = 0; i < annotations.size(); i++) {
            annotation = annotations.get(annotations.keyAt(i));
            if (annotation instanceof Polygon) {
                polygons.add((Polygon) annotation);
            }
        }
        return polygons;
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
            annotations.put(id, polyline);
        }
        return polyline;
    }

    List<Polyline> addPolylines(@NonNull List<PolylineOptions> polylineOptionsList, @NonNull MapboxMap mapboxMap) {
        int count = polylineOptionsList.size();
        Polyline polyline;
        List<Polyline> polylines = new ArrayList<>(count);

        if (count > 0) {
            for (PolylineOptions options : polylineOptionsList) {
                polyline = options.getPolyline();
                if (!polyline.getPoints().isEmpty()) {
                    polylines.add(polyline);
                }
            }

            long[] ids = null;
            if (nativeMapView != null) {
                ids = nativeMapView.addPolylines(polylines);
            }

            long id = 0;
            Polyline p;

            for (int i = 0; i < polylines.size(); i++) {
                p = polylines.get(i);
                p.setMapboxMap(mapboxMap);
                if (ids != null) {
                    id = ids[i];
                } else {
                    // unit test
                    id++;
                }
                p.setId(id);
                annotations.put(id, p);
            }
        }
        return polylines;
    }

    void updatePolyline(Polyline polyline) {
        if (polyline == null) {
            return;
        }

        if (polyline.getId() == -1) {
            return;
        }

        nativeMapView.updatePolyline(polyline);

        int index = annotations.indexOfKey(polyline.getId());
        if (index > -1) {
            annotations.setValueAt(index, polyline);
        }
    }

    List<Polyline> getPolylines() {
        List<Polyline> polylines = new ArrayList<>();
        Annotation annotation;
        for (int i = 0; i < annotations.size(); i++) {
            annotation = annotations.get(annotations.keyAt(i));
            if (annotation instanceof Polyline) {
                polylines.add((Polyline) annotation);
            }
        }
        return polylines;
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
        int count = annotations.size();
        for (int i = 0; i < count; i++) {
            Annotation annotation = annotations.get(i);
            if (annotation instanceof Marker) {
                Marker marker = (Marker) annotation;
                marker.setTopOffsetPixels(
                        iconManager.getTopOffsetPixelsForIcon(marker.getIcon()));
            }
        }

        for (Marker marker : selectedMarkers) {
            if (marker.isInfoWindowShown()) {
                marker.hideInfoWindow();
                marker.showInfoWindow(mapboxMap, mapboxMap.getMapView());
            }
        }
    }

    void reloadMarkers() {
        int count = annotations.size();
        for (int i = 0; i < count; i++) {
            Annotation annotation = annotations.get(i);
            if (annotation instanceof Marker) {
                Marker marker = (Marker) annotation;
                nativeMapView.removeAnnotation(annotation.getId());
                long newId = nativeMapView.addMarker(marker);
                marker.setId(newId);
            }
        }
    }
}
