package com.mapbox.mapboxsdk.maps;

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

class AnnotationManager {

    private MapboxMap mapboxMap;
    private MapView mapView;
    private InfoWindowManager infoWindowManager;

    private LongSparseArray<Annotation> annotations;
    private List<Marker> selectedMarkers;
    private MarkerViewManager markerViewManager;

    private MapboxMap.OnMarkerClickListener onMarkerClickListener;

    AnnotationManager(MapboxMap map, MapView view, InfoWindowManager manager) {
        mapView = view;
        mapboxMap = map;
        infoWindowManager = manager;
        markerViewManager = new MarkerViewManager(map, view);
        selectedMarkers = new ArrayList<>();
        annotations = new LongSparseArray<>();
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
        mapView.removeAnnotation(id);
        annotations.remove(id);
    }

    void removeAnnotation(long id) {
        mapView.removeAnnotation(id);
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
        mapView.removeAnnotations(ids);
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
        mapView.removeAnnotations(ids);
        annotations.clear();
    }

    //
    // Markers
    //

    Marker addMarker(@NonNull BaseMarkerOptions markerOptions) {
        Marker marker = prepareMarker(markerOptions);
        long id = mapView.addMarker(marker);
        marker.setMapboxMap(mapboxMap);
        marker.setId(id);
        annotations.put(id, marker);
        return marker;
    }

    List<Marker> addMarkers(@NonNull List<? extends BaseMarkerOptions> markerOptionsList) {
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
                long[] ids = mapView.addMarkers(markers);

                // if unittests or markers are correctly added to map
                if (ids == null || ids.length == markers.size()) {
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
        }
        return markers;
    }

    private Marker prepareMarker(BaseMarkerOptions markerOptions) {
        Marker marker = markerOptions.getMarker();
        Icon icon = mapView.loadIconForMarker(marker);
        marker.setTopOffsetPixels(mapView.getTopOffsetPixelsForIcon(icon));
        return marker;
    }

    MarkerView addMarker(@NonNull BaseMarkerViewOptions markerOptions) {
        MarkerView marker = prepareViewMarker(markerOptions);
        marker.setMapboxMap(mapboxMap);
        long id = mapView.addMarker(marker);
        marker.setId(id);
        annotations.put(id, marker);
        markerViewManager.invalidateViewMarkersInVisibleRegion();
        return marker;
    }

    List<MarkerView> addMarkerViews(@NonNull List<? extends BaseMarkerViewOptions> markerViewOptions) {
        List<MarkerView> markers = new ArrayList<>();
        for (BaseMarkerViewOptions markerViewOption : markerViewOptions) {
            MarkerView marker = prepareViewMarker(markerViewOption);
            marker.setMapboxMap(mapboxMap);
            long id = mapView.addMarker(marker);
            marker.setId(id);
            annotations.put(id, marker);
            markers.add(marker);
        }
        markerViewManager.invalidateViewMarkersInVisibleRegion();
        return markers;
    }

    private MarkerView prepareViewMarker(BaseMarkerViewOptions markerViewOptions) {
        MarkerView marker = markerViewOptions.getMarker();
        mapView.loadIconForMarkerView(marker);
        return marker;
    }

    void updateMarker(@NonNull Marker updatedMarker) {
        mapView.updateMarker(updatedMarker);

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

    void selectMarker(@NonNull Marker marker) {
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
                infoWindowManager.getInfoWindows().add(marker.showInfoWindow(mapboxMap, mapView));
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

    //
    // Polygons
    //

    Polygon addPolygon(@NonNull PolygonOptions polygonOptions) {
        Polygon polygon = polygonOptions.getPolygon();
        if (!polygon.getPoints().isEmpty()) {
            long id = mapView.addPolygon(polygon);
            polygon.setId(id);
            polygon.setMapboxMap(mapboxMap);
            annotations.put(id, polygon);
        }
        return polygon;
    }

    List<Polygon> addPolygons(@NonNull List<PolygonOptions> polygonOptionsList) {
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

            long[] ids = mapView.addPolygons(polygons);

            // if unit tests or polygons correctly added to map
            if (ids == null || ids.length == polygons.size()) {
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
        }
        return polygons;
    }

    void updatePolygon(Polygon polygon) {
        mapView.updatePolygon(polygon);

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

    Polyline addPolyline(@NonNull PolylineOptions polylineOptions) {
        Polyline polyline = polylineOptions.getPolyline();
        if (!polyline.getPoints().isEmpty()) {
            long id = mapView.addPolyline(polyline);
            polyline.setMapboxMap(mapboxMap);
            polyline.setId(id);
            annotations.put(id, polyline);
        }
        return polyline;
    }

    List<Polyline> addPolylines(@NonNull List<PolylineOptions> polylineOptionsList) {
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

            long[] ids = mapView.addPolylines(polylines);

            // if unit tests or polylines are correctly added to map
            if (ids == null || ids.length == polylines.size()) {
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
        }
        return polylines;
    }

    void updatePolyline(Polyline polyline) {
        mapView.updatePolyline(polyline);

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

    MarkerViewManager getMarkerViewManager() {
        return markerViewManager;
    }

}
