package com.mapbox.mapboxsdk.maps

import android.content.Context
import com.mapbox.mapboxsdk.maps.renderer.MapRenderer
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.mockito.Mockito.*
import org.mockito.junit.MockitoJUnitRunner

@RunWith(MockitoJUnitRunner::class)
class MapPresenterTest {
  private lateinit var mapPresenter: MapPresenter
  private lateinit var mapView: MapView
  private lateinit var context: Context
  private lateinit var mapboxMapOptions: MapboxMapOptions
  private lateinit var mapRenderer: MapRenderer
  private lateinit var mapChangeReceiver: MapChangeReceiver

  @Before
  fun setup() {
    mapView = mock(MapView::class.java)
    context = mock(Context::class.java)
    mapChangeReceiver = mock(MapChangeReceiver::class.java)

    mapboxMapOptions = mock(MapboxMapOptions::class.java)
    doReturn(0xFFF0E9E1.toInt()).`when`(mapboxMapOptions).foregroundLoadColor
    doReturn(false).`when`(mapboxMapOptions).textureMode
    doReturn(null).`when`(mapboxMapOptions).localIdeographFontFamily
    doReturn(1f).`when`(mapboxMapOptions).pixelRatio

    doReturn(mapRenderer).`when`(mapView).createSurfaceView(any(), any(), any())
    doReturn(mapRenderer).`when`(mapView).createTextureView(any(), any(), any())

    mapPresenter = MapPresenterImpl(mapView, context, mapboxMapOptions, mapChangeReceiver)
  }

  @Test
  fun init_foregroundColorSet() {
    verify(mapView).setForegroundColor(0xFFF0E9E1.toInt())
  }

  @Test
  fun init_inflateInternalViews() {
    verify(mapView).inflateInternalViews()
  }

  @Test
  fun init_setViewOptions() {
    verify(mapView).setViewOptions()
  }

  @Test
  fun init_initializeTextureView() {
    reset(mapView)
    doReturn(true).`when`(mapboxMapOptions).textureMode
    doReturn(false).`when`(mapboxMapOptions).translucentTextureSurface
    doReturn(null).`when`(mapboxMapOptions).localIdeographFontFamily
    mapPresenter = MapPresenterImpl(mapView, context, mapboxMapOptions, mapChangeReceiver)
    verify(mapView).createTextureView(mapPresenter, null, false)
  }

  @Test
  fun init_initializeTextureView_translucent() {
    reset(mapView)
    doReturn(true).`when`(mapboxMapOptions).textureMode
    doReturn(true).`when`(mapboxMapOptions).translucentTextureSurface
    doReturn(null).`when`(mapboxMapOptions).localIdeographFontFamily
    mapPresenter = MapPresenterImpl(mapView, context, mapboxMapOptions, mapChangeReceiver)
    verify(mapView).createTextureView(mapPresenter, null, true)
  }

  @Test
  fun init_initializeTextureView_localFonts() {
    reset(mapView)
    doReturn(true).`when`(mapboxMapOptions).textureMode
    doReturn(true).`when`(mapboxMapOptions).translucentTextureSurface
    doReturn("comic-sans").`when`(mapboxMapOptions).localIdeographFontFamily
    mapPresenter = MapPresenterImpl(mapView, context, mapboxMapOptions, mapChangeReceiver)
    verify(mapView).createTextureView(mapPresenter, "comic-sans", true)
  }

  @Test
  fun init_initializeSurfaceView() {
    reset(mapView)
    doReturn(false).`when`(mapboxMapOptions).textureMode
    doReturn(null).`when`(mapboxMapOptions).localIdeographFontFamily
    doReturn(false).`when`(mapboxMapOptions).renderSurfaceOnTop
    mapPresenter = MapPresenterImpl(mapView, context, mapboxMapOptions, mapChangeReceiver)
    verify(mapView).createSurfaceView(mapPresenter, null, false)
  }

  @Test
  fun init_initializeSurfaceView_LocalFonts() {
    reset(mapView)
    doReturn(false).`when`(mapboxMapOptions).textureMode
    doReturn("comic-sans").`when`(mapboxMapOptions).localIdeographFontFamily
    doReturn(true).`when`(mapboxMapOptions).renderSurfaceOnTop
    mapPresenter = MapPresenterImpl(mapView, context, mapboxMapOptions, mapChangeReceiver)
    verify(mapView).createSurfaceView(mapPresenter, "comic-sans", true)
  }

  @Test
  fun init_initializeNativeMapView() {
    reset(mapView)
    doReturn(false).`when`(mapboxMapOptions).crossSourceCollisions
    mapPresenter = MapPresenterImpl(mapView, context, mapboxMapOptions, mapChangeReceiver)
    verify(mapView).createNativeMapView(1f, false, mapChangeReceiver, mapRenderer)
  }

  @Test
  fun init_initializeNativeMapView_crossSourceCollisions() {
    reset(mapView)
    doReturn(true).`when`(mapboxMapOptions).crossSourceCollisions
    mapPresenter = MapPresenterImpl(mapView, context, mapboxMapOptions, mapChangeReceiver)
    verify(mapView).createNativeMapView(1f, true, mapChangeReceiver, mapRenderer)
  }
}