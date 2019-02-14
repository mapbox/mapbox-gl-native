package com.mapbox.mapboxsdk.maps

import android.content.Context
import org.junit.After
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.mockito.Mockito
import org.mockito.Mockito.*
import org.mockito.junit.MockitoJUnitRunner

@RunWith(MockitoJUnitRunner::class)
class MapPresenterTest {
  private lateinit var mapPresenter: MapPresenter
  private lateinit var mapView: MapView
  private lateinit var context: Context
  private lateinit var mapboxMapOptions: MapboxMapOptions

  @Before
  fun setup() {
    mapView = mock(MapView::class.java)
    context = mock(Context::class.java)

    mapboxMapOptions = mock(MapboxMapOptions::class.java)
    doReturn(0xFFF0E9E1.toInt()).`when`(mapboxMapOptions).foregroundLoadColor
    doReturn(false).`when`(mapboxMapOptions).textureMode
    doReturn(null).`when`(mapboxMapOptions).localIdeographFontFamily

    mapPresenter = MapPresenterImpl(mapView, context, mapboxMapOptions)
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
    doReturn(true).`when`(mapboxMapOptions).textureMode
    doReturn(false).`when`(mapboxMapOptions).translucentTextureSurface
    doReturn(null).`when`(mapboxMapOptions).localIdeographFontFamily
    mapPresenter = MapPresenterImpl(mapView, context, mapboxMapOptions)
    verify(mapView).initializeTextureView(null, false)
  }

  @Test
  fun init_initializeTranslucentTextureView() {
    doReturn(true).`when`(mapboxMapOptions).textureMode
    doReturn(true).`when`(mapboxMapOptions).translucentTextureSurface
    doReturn(null).`when`(mapboxMapOptions).localIdeographFontFamily
    mapPresenter = MapPresenterImpl(mapView, context, mapboxMapOptions)
    verify(mapView).initializeTextureView(null, true)
  }

  @Test
  fun init_initializeTextureViewWithLocalFonts() {
    doReturn(true).`when`(mapboxMapOptions).textureMode
    doReturn(true).`when`(mapboxMapOptions).translucentTextureSurface
    doReturn("comic-sans").`when`(mapboxMapOptions).localIdeographFontFamily
    mapPresenter = MapPresenterImpl(mapView, context, mapboxMapOptions)
    verify(mapView).initializeTextureView("comic-sans", true)
  }

  @Test
  fun init_initializeSurfaceView() {
    doReturn(false).`when`(mapboxMapOptions).textureMode
    doReturn(null).`when`(mapboxMapOptions).localIdeographFontFamily
    mapPresenter = MapPresenterImpl(mapView, context, mapboxMapOptions)
    verify(mapView).initializeSurfaceView(null)
  }

  @Test
  fun init_initializeSurfaceViewWithLocalFonts() {
    doReturn(false).`when`(mapboxMapOptions).textureMode
    doReturn("comic-sans").`when`(mapboxMapOptions).localIdeographFontFamily
//    mapPresenter = MapPresenterImpl(mapView, context, mapboxMapOptions)
//    verify(mapView).initializeSurfaceView("comic-sans")
  }
}