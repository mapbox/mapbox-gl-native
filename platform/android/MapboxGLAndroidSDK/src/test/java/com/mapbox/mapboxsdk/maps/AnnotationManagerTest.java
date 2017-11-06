package com.mapbox.mapboxsdk.maps;

import android.support.v4.util.LongSparseArray;

import com.mapbox.mapboxsdk.annotations.Annotation;
import com.mapbox.mapboxsdk.annotations.BaseMarkerOptions;
import com.mapbox.mapboxsdk.annotations.Marker;
import com.mapbox.mapboxsdk.annotations.MarkerOptions;
import com.mapbox.mapboxsdk.annotations.MarkerViewManager;
import com.mapbox.mapboxsdk.geometry.LatLng;

import org.junit.Test;
import org.mockito.ArgumentMatchers;

import java.util.ArrayList;
import java.util.List;

import static junit.framework.Assert.assertEquals;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class AnnotationManagerTest {

  @Test
  public void checksAddAMarker() throws Exception {
    NativeMapView aNativeMapView = mock(NativeMapView.class);
    MapView aMapView = mock(MapView.class);
    LongSparseArray<Annotation> annotationsArray = new LongSparseArray<>();
    MarkerViewManager aMarkerViewManager = mock(MarkerViewManager.class);
    IconManager aIconManager = mock(IconManager.class);
    Annotations annotations = new AnnotationContainer(aNativeMapView, annotationsArray);
    Markers markers = new MarkerContainer(aNativeMapView, aMapView, annotationsArray, aIconManager, aMarkerViewManager);
    Polygons polygons = new PolygonContainer(aNativeMapView, annotationsArray);
    Polylines polylines = new PolylineContainer(aNativeMapView, annotationsArray);
    ShapeAnnotations shapeAnnotations = new ShapeAnnotationContainer(aNativeMapView, annotationsArray);
    AnnotationManager annotationManager = new AnnotationManager(aNativeMapView, aMapView, annotationsArray,
      aMarkerViewManager, aIconManager, annotations, markers, polygons, polylines, shapeAnnotations);
    Marker aMarker = mock(Marker.class);
    long aId = 5L;
    when(aNativeMapView.addMarker(aMarker)).thenReturn(aId);
    BaseMarkerOptions aMarkerOptions = mock(BaseMarkerOptions.class);
    MapboxMap aMapboxMap = mock(MapboxMap.class);
    when(aMarkerOptions.getMarker()).thenReturn(aMarker);

    annotationManager.addMarker(aMarkerOptions, aMapboxMap);

    assertEquals(aMarker, annotationManager.getAnnotations().get(0));
    assertEquals(aMarker, annotationManager.getAnnotation(aId));
  }

  @Test
  public void checksAddMarkers() throws Exception {
    NativeMapView aNativeMapView = mock(NativeMapView.class);
    MapView aMapView = mock(MapView.class);
    LongSparseArray<Annotation> annotationsArray = new LongSparseArray<>();
    MarkerViewManager aMarkerViewManager = mock(MarkerViewManager.class);
    IconManager aIconManager = mock(IconManager.class);
    Annotations annotations = new AnnotationContainer(aNativeMapView, annotationsArray);
    Markers markers = new MarkerContainer(aNativeMapView, aMapView, annotationsArray, aIconManager, aMarkerViewManager);
    Polygons polygons = new PolygonContainer(aNativeMapView, annotationsArray);
    Polylines polylines = new PolylineContainer(aNativeMapView, annotationsArray);
    ShapeAnnotations shapeAnnotations = new ShapeAnnotationContainer(aNativeMapView, annotationsArray);
    AnnotationManager annotationManager = new AnnotationManager(aNativeMapView, aMapView, annotationsArray,
      aMarkerViewManager, aIconManager, annotations, markers, polygons, polylines, shapeAnnotations);
    long firstId = 1L;
    long secondId = 2L;
    List<BaseMarkerOptions> markerList = new ArrayList<>();
    MarkerOptions firstMarkerOption = new MarkerOptions().position(new LatLng()).title("first");
    MarkerOptions secondMarkerOption = new MarkerOptions().position(new LatLng()).title("second");

    markerList.add(firstMarkerOption);
    markerList.add(secondMarkerOption);
    MapboxMap aMapboxMap = mock(MapboxMap.class);
    when(aNativeMapView.addMarker(any(Marker.class))).thenReturn(firstId, secondId);

    when(aNativeMapView.addMarkers(ArgumentMatchers.<Marker>anyList()))
            .thenReturn(new long[]{firstId, secondId});

    annotationManager.addMarkers(markerList, aMapboxMap);

    assertEquals(2, annotationManager.getAnnotations().size());
    assertEquals("first", ((Marker) annotationManager.getAnnotations().get(0)).getTitle());
    assertEquals("second", ((Marker) annotationManager.getAnnotations().get(1)).getTitle());
    assertEquals("first", ((Marker) annotationManager.getAnnotation(firstId)).getTitle());
    assertEquals("second", ((Marker) annotationManager.getAnnotation(secondId)).getTitle());
  }
}