package com.mapbox.mapboxsdk.maps

import android.graphics.Bitmap
import com.mapbox.mapboxsdk.constants.MapboxConstants
import com.mapbox.mapboxsdk.style.layers.SymbolLayer
import com.mapbox.mapboxsdk.style.layers.TransitionOptions
import com.mapbox.mapboxsdk.style.sources.GeoJsonSource
import io.mockk.every
import io.mockk.mockk
import io.mockk.spyk
import io.mockk.verify
import org.junit.Before
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner

@RunWith(RobolectricTestRunner::class)
class StyleTest {

    private lateinit var mapboxMap: MapboxMap

    private lateinit var nativeMapView: NativeMapView

    @Before
    fun setup() {
        nativeMapView = mockk()
        mapboxMap = MapboxMap(nativeMapView, null, null, null, null, null)
        every { nativeMapView.styleUrl = any() } answers {}
        every { nativeMapView.styleJson = any() } answers {}
        every { nativeMapView.addLayerBelow(any(), any()) } answers {}
        every { nativeMapView.addLayerAbove(any(), any()) } answers {}
        every { nativeMapView.addLayerAt(any(), any()) } answers {}
        every { nativeMapView.addSource(any()) } answers {}
        every { nativeMapView.addImage(any(), any(), any()) } answers {}
        every { nativeMapView.transitionDuration = any() } answers {}
        every { nativeMapView.transitionDelay = any() } answers {}
        mapboxMap.injectLocationComponent(spyk())
    }

    @Test
    fun testFromUrl() {
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUrl = Style.MAPBOX_STREETS }
    }

    @Test
    fun testFromJson() {
        val builder = Style.Builder().fromJson("{}")
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleJson = "{}" }
    }

    @Test
    fun testWithLayer() {
        val layer = mockk<SymbolLayer>()
        every { layer.id } returns "1"
        val builder = Style.Builder().withLayer(layer)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.addLayerBelow(layer, MapboxConstants.LAYER_ID_ANNOTATIONS) }
    }

    @Test
    fun testWithLayerAbove() {
        val layer = mockk<SymbolLayer>()
        every { layer.id } returns "1"
        val builder = Style.Builder().withLayerAbove(layer, "id")
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.addLayerAbove(layer, "id") }
    }

    @Test
    fun testWithLayerBelow() {
        val layer = mockk<SymbolLayer>()
        every { layer.id } returns "1"
        val builder = Style.Builder().withLayerBelow(layer, "id")
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.addLayerBelow(layer, "id") }
    }

    @Test
    fun testWithLayerAt() {
        val layer = mockk<SymbolLayer>()
        every { layer.id } returns "1"
        val builder = Style.Builder().withLayerAt(layer, 1)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.addLayerAt(layer, 1) }
    }

    @Test
    fun testWithSource() {
        val source = mockk<GeoJsonSource>()
        every { source.id } returns "1"
        val builder = Style.Builder().withSource(source)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.addSource(source) }
    }

    @Test
    fun testWithTransitionOptions() {
        val transitionOptions = TransitionOptions(100, 200)
        val builder = Style.Builder().withTransition(transitionOptions)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.transitionDuration = 100 }
        verify(exactly = 1) { nativeMapView.transitionDelay = 200 }
    }

    @Test
    fun testWithImage() {
        val image = mockk<Bitmap>()
        val builder = Style.Builder().withImage("id", image)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.addImage("id", image, false) }
    }

    @Test
    fun testWithImageSdf() {
        val image = mockk<Bitmap>()
        val builder = Style.Builder().withImage("id", image, true)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.addImage("id", image, true) }
    }

    @Test
    fun testWithFromLoadingSource() {
        val source = mockk<GeoJsonSource>()
        every { source.id } returns "1"
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withSource(source)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUrl = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.addSource(source) }
    }

    @Test
    fun testWithFromLoadingLayer() {
        val layer = mockk<SymbolLayer>()
        every { layer.id } returns "1"
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withLayer(layer)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUrl = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.addLayerBelow(layer, MapboxConstants.LAYER_ID_ANNOTATIONS) }
    }

    @Test
    fun testWithFromLoadingLayerAt() {
        val layer = mockk<SymbolLayer>()
        every { layer.id } returns "1"
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withLayerAt(layer, 1)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUrl = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.addLayerAt(layer, 1) }
    }

    @Test
    fun testWithFromLoadingLayerBelow() {
        val layer = mockk<SymbolLayer>()
        every { layer.id } returns "1"
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withLayerBelow(layer, "below")
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUrl = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.addLayerBelow(layer, "below") }
    }

    @Test
    fun testWithFromLoadingLayerAbove() {
        val layer = mockk<SymbolLayer>()
        every { layer.id } returns "1"
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withLayerBelow(layer, "below")
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUrl = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.addLayerBelow(layer, "below") }
    }

    @Test
    fun testWithFromLoadingTransitionOptions() {
        val transitionOptions = TransitionOptions(100, 200)
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withTransition(transitionOptions)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUrl = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.transitionDuration = 100 }
        verify(exactly = 1) { nativeMapView.transitionDelay = 200 }
    }

    @Test
    fun testWithFromImage() {
        val bitmap = mockk<Bitmap>()
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withImage("id", bitmap)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUrl = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.addImage("id", bitmap, false) }
    }

    @Test
    fun testWithFromImageSdf() {
        val bitmap = mockk<Bitmap>()
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withImage("id", bitmap, true)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUrl = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.addImage("id", bitmap, true) }
    }

    @Test
    fun testFromCallback() {
        val callback = mockk<Style.OnStyleLoaded>()
        every { callback.onStyleLoaded(any()) } answers {}
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS)
        mapboxMap.setStyle(builder, callback)
        verify(exactly = 1) { nativeMapView.styleUrl = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { callback.onStyleLoaded(any()) }
    }

    @Test
    fun testWithCallback() {
        val callback = mockk<Style.OnStyleLoaded>()
        every { callback.onStyleLoaded(any()) } answers {}
        val source = mockk<GeoJsonSource>()
        every { source.id } returns "1"
        val builder = Style.Builder().withSource(source)
        mapboxMap.setStyle(builder, callback)
        verify(exactly = 1) { nativeMapView.addSource(source) }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { callback.onStyleLoaded(any()) }
    }

    @Test
    fun testGetAsyncWith() {
        val callback = mockk<Style.OnStyleLoaded>()
        every { callback.onStyleLoaded(any()) } answers {}
        mapboxMap.getStyle(callback)
        val source = mockk<GeoJsonSource>()
        every { source.id } returns "1"
        val builder = Style.Builder().withSource(source)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.addSource(source) }
        verify(exactly = 1) { callback.onStyleLoaded(any()) }
    }

    @Test
    fun testGetAsyncFrom() {
        val callback = mockk<Style.OnStyleLoaded>()
        every { callback.onStyleLoaded(any()) } answers {}
        mapboxMap.getStyle(callback)
        val source = mockk<GeoJsonSource>()
        every { source.id } returns "1"
        val builder = Style.Builder().fromJson("{}")
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleJson = "{}" }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { callback.onStyleLoaded(any()) }
    }

    @Test
    fun testGetAsyncWithFrom() {
        val callback = mockk<Style.OnStyleLoaded>()
        every { callback.onStyleLoaded(any()) } answers {}
        mapboxMap.getStyle(callback)
        val source = mockk<GeoJsonSource>()
        every { source.id } returns "1"
        val builder = Style.Builder().fromUrl(Style.MAPBOX_STREETS).withSource(source)
        mapboxMap.setStyle(builder)
        verify(exactly = 1) { nativeMapView.styleUrl = Style.MAPBOX_STREETS }
        mapboxMap.notifyStyleLoaded()
        verify(exactly = 1) { nativeMapView.addSource(source) }
        verify(exactly = 1) { callback.onStyleLoaded(any()) }
    }
}